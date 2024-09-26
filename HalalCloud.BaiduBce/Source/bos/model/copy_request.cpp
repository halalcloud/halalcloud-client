/**
 * Copyright 2014 (c) Baidu, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#include "bcesdk/bos/model/copy_request.h"
#include "bcesdk/util/util.h"

BEGIN_CPPSDK_NAMESPACE

int CopyRequest::fill_http_request(HttpRequest *request) {
    std::stringstream ss;
    ss<< "/" << _source_bucket_name << "/" << _source_object_name;
    request->append_header("x-bce-copy-source", StringUtil::url_encode(ss.str(), false));
    if (!_if_match.empty()) {
        request->append_header("x-bce-copy-source-if-match", _if_match);
    }
    if (!_if_none_match.empty()) {
        request->append_header("x-bce-copy-source-if-none-match", _if_none_match);
    }
    if (!_if_unmodifed_since.empty()) {
        request->append_header("x-bce-copy-source-if-unmodified-since", _if_unmodifed_since);
    }
    if (!_if_modified_since.empty()) {
        request->append_header("x-bce-copy-source-if-modified-since", _if_modified_since);
    }
    if (_traffic_limit) {
        request->append_header("x-bce-traffic-limit", _traffic_limit);
    }
    return 0;
}

END_CPPSDK_NAMESPACE

