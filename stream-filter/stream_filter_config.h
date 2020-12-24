#pragma once

#include "stream-filter/stream_filter.pb.h"
#include "stream-filter/stream_filter.pb.validate.h"
#include "stream_filter.h"

#include "extensions/filters/http/common/factory_base.h"
#include "extensions/filters/http/well_known_names.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace MGW {

class SampleStreamFilterConfig : public Common::FactoryBase<sample::Decoder>{

public:
  SampleStreamFilterConfig() : FactoryBase("envoy.filters.http.mgwstream") {}

private:
  Http::FilterFactoryCb createFilterFactoryFromProtoTyped(
      const sample::Decoder& proto_config, 
      const std::string& stats_prefix, 
      Server::Configuration::FactoryContext& context) override;
};



} // namespace MGW
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy