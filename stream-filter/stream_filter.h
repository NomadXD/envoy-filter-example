#pragma once
#include <memory>
#include <string>
#include <vector>
#include "stream-filter/stream_filter.pb.h"

#include "envoy/http/context.h"
#include "envoy/http/filter.h"
#include "envoy/local_info/local_info.h"
#include "envoy/ratelimit/ratelimit.h"
#include "envoy/runtime/runtime.h"
#include "envoy/stats/scope.h"
#include "common/common/assert.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace MGW {

enum class RateLimitType { Default, Downstream, Upstream };

enum class RateLimitStatus { UnderLimit, OverLimit, FailureModeAllowed};

class FilterConfig {

public:
    FilterConfig(const sample::Decoder& config, 
    const LocalInfo::LocalInfo& local_info, Stats::Scope& scope,
    Runtime::Loader& runtime, Http::Context& http_context)
    : domain_(config.domain()), 
    rate_limit_type_(config.rate_limit_type().empty() ? stringToType("default") 
                                    : stringToType(config.rate_limit_type())),
    local_info_(local_info), scope_(scope), runtime_(runtime),
    failure_mode_deny_(config.failure_mode_deny()),
    http_context_(http_context) {}

    const std::string& domain() const { return domain_; }
    const LocalInfo::LocalInfo& localInfo() const { return local_info_; }
    Runtime::Loader& runtime() { return runtime_; }
    Stats::Scope& scope() { return scope_; }
    RateLimitType rateLimitType() const { return rate_limit_type_;}
    RateLimitStatus RateLimitStatus() const {return rate_limit_status_;}
    bool failureModeAllow() const { return !failure_mode_deny_; }
    Http::Context& httpContext() { return http_context_; }
private:

    static RateLimitType stringToType(const std::string& rate_limit_type){
        if (rate_limit_type == "upstream"){
            return RateLimitType::Upstream;
        }else if (rate_limit_type == "downstream"){
            return RateLimitType::Downstream;
        }else {
            ASSERT(rate_limit_type == "default");
            return RateLimitType::Default;
        }
    }
    
    const std::string domain_;
    const RateLimitType rate_limit_type_;
    const LocalInfo::LocalInfo& local_info_;
    Stats::Scope& scope_;
    Runtime::Loader& runtime_;
    const bool failure_mode_deny_;
    const absl::optional<Grpc::Status::GrpcStatus> rate_limited_grpc_status_;
    Http::Context& http_context_;
    enum RateLimitStatus rate_limit_status_;
};

using FilterConfigSharedPtr = std::shared_ptr<FilterConfig>;

class SampleStreamFilter : public Http::StreamFilter, public Logger::Loggable<Logger::Id::filter>{

public:
    SampleStreamFilter(FilterConfigSharedPtr config)
    : config_(config) {}

    // Http::StreamDecoderFilter
  Http::FilterHeadersStatus decodeHeaders(Http::RequestHeaderMap& headers,
                                          bool end_stream) override;
  Http::FilterDataStatus decodeData(Buffer::Instance& data, bool end_stream) override;
  Http::FilterTrailersStatus decodeTrailers(Http::RequestTrailerMap& trailers) override;
  void setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& callbacks) override;

  // Http::StreamEncoderFilter
  Http::FilterHeadersStatus encode100ContinueHeaders(Http::ResponseHeaderMap& headers) override;
  Http::FilterHeadersStatus encodeHeaders(Http::ResponseHeaderMap& headers,
                                          bool end_stream) override;
  Http::FilterDataStatus encodeData(Buffer::Instance& data, bool end_stream) override;
  Http::FilterTrailersStatus encodeTrailers(Http::ResponseTrailerMap& trailers) override;
  Http::FilterMetadataStatus encodeMetadata(Http::MetadataMap&) override;
  void setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks& callbacks) override;

  void onDestroy() override;

private:
FilterConfigSharedPtr config_;
Http::StreamDecoderFilterCallbacks* callbacks_{};
//Http::ResponseHeaderMapPtr response_headers_to_add_;
//Http::RequestHeaderMap* request_headers_{};
};



} // namespace MGW
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy