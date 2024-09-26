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
#include <sstream>
#include <algorithm>

#include <uuid/uuid.h>

#include "bcesdk/model/bce_request.h"

BEGIN_CPPSDK_NAMESPACE

void BceRequest::set_request_header(const std::string &key, const std::string &val)
{
    _headers[key] = val;
}

int BceRequest::build_http_request(HttpRequest *request, bool cname_enabled)
{
    int ret = build_command_specific(request);
    if (ret != 0) {
        return ret;
    }
    if (_request_id.size() == 0) {
        uuid_t out;
        uuid_generate(out);
        _request_id.resize(36);
        uuid_unparse(out, const_cast<char *>(_request_id.data()));
        std::transform(_request_id.begin(), _request_id.end(), _request_id.begin(), tolower);
    }
    request->append_header("x-bce-request-id", _request_id);
    request->set_uri(get_uri(cname_enabled));

    for (StringMap::iterator it = _headers.begin(); it != _headers.end(); it++) {
        request->append_header(it->first, it->second);
    }
    return 0;
}

END_CPPSDK_NAMESPACE

