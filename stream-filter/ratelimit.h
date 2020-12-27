#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "envoy/common/pure.h"
#include "stream-filter/stream_filter.pb.h"
#include "envoy/singleton/manager.h"
#include "envoy/stream_info/stream_info.h"
#include "envoy/tracing/http_tracer.h"
#include "absl/types/optional.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace MGW {

enum class LimitStatus {
  // The request is not over limit.
  OK,
  // The rate limit service could not be queried.
  Error,
  // The request is over limit.
  OverLimit
};

class RequestCallbacks{
public:
    virtual ~RequestCallbacks() = default;

    virtual void complete(LimitStatus status) PURE;

};

class Client {

public:
 virtual ~Client() = default;

 virtual void cancel() PURE;

 virtual void limit(RequestCallbacks& callbacks, 
                    const std::string& domain,
                    Tracing::Span& parent_span,
                    const StreamInfo::StreamInfo& stream_info) PURE;

};


using ClientPtr = std::unique_ptr<Client>;


} // namespace MGW
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy