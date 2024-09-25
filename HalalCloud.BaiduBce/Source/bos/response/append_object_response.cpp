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
#include "bcesdk/bos/response/append_object_response.h"
#include "bcesdk/util/util.h"

BEGIN_CPPSDK_NAMESPACE

void AppendObjectResponse::do_handle_chain(HttpResponse &response) {
    const std::string &next_off = response.header("x-bce-next-append-offset");
    if (!StringUtil::str2uint64(next_off, &_next_append_offset)) {
        _next_append_offset = 0;
    }
    const std::string &etag = response.header("etag");
    if (etag.length() < 2 || etag[0] != '"' || etag[etag.length() - 1] != '"') {
        _etag = etag;
    }
    _etag = etag.substr(1, etag.length() - 2);
}

END_CPPSDK_NAMESPACE

