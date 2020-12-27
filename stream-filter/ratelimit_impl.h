#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

#include "envoy/config/core/v3/grpc_service.pb.h"
#include "envoy/grpc/async_client.h"
#include "envoy/grpc/async_client_manager.h"
#include "envoy/server/filter_config.h"
#include "stream-filter/stream_filter.pb.h"
#include "envoy/stats/scope.h"
#include "envoy/tracing/http_tracer.h"
#include "envoy/upstream/cluster_manager.h"

#include "common/common/logger.h"
#include "common/grpc/typed_async_client.h"
#include "common/singleton/const_singleton.h"

#include "stream-filter/ratelimit.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace MGW {

using RateLimitAsyncCallbacks = Grpc::AsyncRequestCallbacks<sample::RateLimitResponse>;

struct ConstantValues {
  const std::string TraceStatus = "ratelimit_status";
  const std::string TraceOverLimit = "over_limit";
  const std::string TraceOk = "ok";
};

using Constants = ConstSingleton<ConstantValues>;

class GrpcClientImpl : public Client,
                       public RateLimitAsyncCallbacks,
                       public Logger::Loggable<Logger::Id::config>{

public:
    GrpcClientImpl(Grpc::RawAsyncClientPtr&& async_client, const absl::optional<std::chrono::milliseconds>& timeout,
    envoy::config::core::v3::ApiVersion transport_api_version);
    ~ GrpcClientImpl() override;

    static void createRequest(sample::RateLimitRequest& request,
                            const std::string& domain);
                    
    // Filters::Common::RateLimit::Client
    void cancel() override;
    void limit(RequestCallbacks& callbacks, const std::string& domain,Tracing::Span& parent_span,
             const StreamInfo::StreamInfo& stream_info) override;

    // Grpc::AsyncRequestCallbacks
    void onCreateInitialMetadata(Http::RequestHeaderMap&) override {}
    void onSuccess(std::unique_ptr<sample::RateLimitResponse>&& response,
                 Tracing::Span& span) override;
    void onFailure(Grpc::Status::GrpcStatus status, const std::string& message,
                 Tracing::Span& span) override;

private:
  Grpc::AsyncClient<sample::RateLimitRequest,
                    sample::RateLimitResponse>
      async_client_;
  Grpc::AsyncRequest* request_{};
  absl::optional<std::chrono::milliseconds> timeout_;
  RequestCallbacks* callbacks_{};
  const Protobuf::MethodDescriptor& service_method_;
  const envoy::config::core::v3::ApiVersion transport_api_version_;
  static const Protobuf::MethodDescriptor& getMethodDescriptor();


};


ClientPtr rateLimitClient(Server::Configuration::FactoryContext& context,
                          const envoy::config::core::v3::GrpcService& grpc_service,
                          const std::chrono::milliseconds timeout,
                          envoy::config::core::v3::ApiVersion transport_api_version);

} // namespace MGW
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
