#include <string>
#include <chrono>
#include "envoy/registry/registry.h"
#include "envoy/server/filter_config.h"
#include "stream_filter_config.h"
#include "stream-filter/stream_filter.pb.h"
#include "stream-filter/stream_filter.pb.validate.h"
#include "stream-filter/stream_filter.h"
#include "stream-filter/ratelimit_impl.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace MGW {

Http::FilterFactoryCb SampleStreamFilterConfig::createFilterFactoryFromProtoTyped(
    const sample::Decoder& proto_config, 
    const std::string&, 
    Server::Configuration::FactoryContext& context){

    const std::chrono::milliseconds timeout = std::chrono::milliseconds(20);

    FilterConfigSharedPtr filter_config(new FilterConfig(proto_config, context.localInfo(),
                                        context.scope(), context.runtime(),
                                        context.httpContext()));

     return [proto_config, &context, timeout,filter_config](Http::FilterChainFactoryCallbacks& callbacks) -> void {
    callbacks.addStreamFilter(std::make_shared<SampleStreamFilter>(
        filter_config, rateLimitClient(context, proto_config.rate_limit_service().grpc_service(), timeout, proto_config.rate_limit_service().transport_api_version())));
  };

}

REGISTER_FACTORY(SampleStreamFilterConfig,
                 Server::Configuration::NamedHttpFilterConfigFactory){"envoy.mgwstream"};


} // namespace MGW
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy