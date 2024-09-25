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
#ifndef BAIDU_BOS_CPPSDK_BOS_RESPONSE_APPEND_OBJECT_RESPONSE_H
#define BAIDU_BOS_CPPSDK_BOS_RESPONSE_APPEND_OBJECT_RESPONSE_H

#include "bcesdk/model/bce_response.h"

BEGIN_CPPSDK_NAMESPACE

class AppendObjectResponse : public BceResponse {
public:
    AppendObjectResponse() : _next_append_offset(0) {}

    const std::string &etag() const {
        return _etag;
    }

    unsigned long long next_append_offset() const {
        return _next_append_offset;
    }

    virtual void do_handle_chain(HttpResponse &response);
private:
    unsigned long long _next_append_offset;
    std::string _etag;
};

END_CPPSDK_NAMESPACE
#endif

