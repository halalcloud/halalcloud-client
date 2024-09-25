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
#ifndef BAIDU_BOS_CPPSDK_MODEL_JSON_RESPONSE_H
#define BAIDU_BOS_CPPSDK_MODEL_JSON_RESPONSE_H

#include "bcesdk/common/common.h"
#include "bcesdk/common/memory_stream.h"
#include "bcesdk/model/bce_response.h"

namespace Json {
class Value;
}

BEGIN_CPPSDK_NAMESPACE

class JsonResponse : public BceResponse {
public:
    JsonResponse() : _str_output_stream(&_buffer) {
        set_output_stream(&_str_output_stream);
    }

    virtual ~JsonResponse() {}

    virtual void do_handle_chain(HttpResponse &response);

    virtual int deserialize(Json::Value &root) = 0;

    const std::string &buffer() const {
        return _buffer;
    }

private:
    std::string _buffer;
    StringOutputStream _str_output_stream;
};
END_CPPSDK_NAMESPACE 
#endif

