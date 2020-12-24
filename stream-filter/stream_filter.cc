#include "stream_filter.h"
#include <string>
#include <vector>

#include "envoy/http/codes.h"
#include "common/common/assert.h"
#include "common/common/enum_to_int.h"
#include "common/common/fmt.h"
#include "common/http/codes.h"
//#include "common/http/header_utility.h"
//#include "common/router/config_impl.h"
//#include "extensions/filters/http/well_known_names.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace MGW {

Http::FilterHeadersStatus SampleStreamFilter::decodeHeaders(Http::RequestHeaderMap&, bool) {
    ENVOY_LOG(trace, "decodeHeaders called");
    return Http::FilterHeadersStatus::Continue;
  //request_headers_ = &headers;
  //initiateCall(headers);
//   return (state_ == State::Calling || state_ == State::Responded)
//              ? Http::FilterHeadersStatus::StopIteration
//              : Http::FilterHeadersStatus::Continue;
}

Http::FilterDataStatus SampleStreamFilter::decodeData(Buffer::Instance&, bool) {
    ENVOY_LOG(trace, "decodeData called");
    ENVOY_LOG(trace, "domain :{}", config_->domain());
//   if (state_ != State::Calling) {
//     return Http::FilterDataStatus::Continue;
//   }
  // If the request is too large, stop reading new data until the buffer drains.
  return Http::FilterDataStatus::Continue;
}

Http::FilterTrailersStatus SampleStreamFilter::decodeTrailers(Http::RequestTrailerMap&) {
  //ASSERT(state_ != State::Responded);
//   return state_ == State::Calling ? Http::FilterTrailersStatus::StopIteration
//                                   : Http::FilterTrailersStatus::Continue;
ENVOY_LOG(trace, "decodeTrailers called");
return Http::FilterTrailersStatus::Continue;
}

Http::FilterHeadersStatus SampleStreamFilter::encode100ContinueHeaders(Http::ResponseHeaderMap&) {
  ENVOY_LOG(trace, "encode100 called");
  return Http::FilterHeadersStatus::Continue;
}

Http::FilterHeadersStatus SampleStreamFilter::encodeHeaders(Http::ResponseHeaderMap&, bool) {
  //populateResponseHeaders(headers);
  ENVOY_LOG(trace, "encodeHeaders called");
  return Http::FilterHeadersStatus::Continue;
}

Http::FilterDataStatus SampleStreamFilter::encodeData(Buffer::Instance&, bool) {
    ENVOY_LOG(trace, "encodeData called");
  return Http::FilterDataStatus::Continue;
}

Http::FilterTrailersStatus SampleStreamFilter::encodeTrailers(Http::ResponseTrailerMap&) {
    ENVOY_LOG(trace, "encodeTrailers called");
  return Http::FilterTrailersStatus::Continue;
}

Http::FilterMetadataStatus SampleStreamFilter::encodeMetadata(Http::MetadataMap&) {
    ENVOY_LOG(trace, "encodeMetadata called");
  return Http::FilterMetadataStatus::Continue;
}

void SampleStreamFilter::setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks&) {}

void SampleStreamFilter::onDestroy() {
//   if (state_ == State::Calling) {
//     state_ = State::Complete;
//     client_->cancel();
//   }
ENVOY_LOG(trace, "onDestroy called");
}

void SampleStreamFilter::setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& callbacks) {
ENVOY_LOG(trace, "decoderCallbacks called");
  callbacks_ = &callbacks;
}



} // namespace MGW
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy