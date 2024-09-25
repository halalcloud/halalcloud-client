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
#ifndef BAIDU_BOS_CPPSDK_BOS_RESPONSE_INIT_MULTI_UPLOAD_RESPONSE_H
#define BAIDU_BOS_CPPSDK_BOS_RESPONSE_INIT_MULTI_UPLOAD_RESPONSE_H

#include <string>

#include "bcesdk/model/json_response.h"

BEGIN_CPPSDK_NAMESPACE

class OutputStream;

class InitMultiUploadResponse : public JsonResponse {
public:
    InitMultiUploadResponse() {}

    virtual ~InitMultiUploadResponse() {}

    const std::string &bucket() const {
        return _bucket;
    }

    const std::string &key() const {
        return _key;
    }

    const std::string &upload_id() const {
        return _upload_id;
    }

    virtual int deserialize(Json::Value &root);

private:
    std::string   _bucket;
    std::string   _key;
    std::string   _upload_id;
};

END_CPPSDK_NAMESPACE
#endif

