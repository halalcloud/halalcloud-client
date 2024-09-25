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
#ifndef BAIDU_BOS_CPPSDK_BOS_RESPONSE_COPY_OBJECT_RESPONSE_H
#define BAIDU_BOS_CPPSDK_BOS_RESPONSE_COPY_OBJECT_RESPONSE_H

#include <string>
#include "bcesdk/model/json_response.h"

BEGIN_CPPSDK_NAMESPACE

class CopyObjectResponse : public JsonResponse {

public:
    CopyObjectResponse() {}

    virtual ~CopyObjectResponse() {}

    time_t last_modified() const {
        return _last_modified;
    }

    const std::string &etag() const {
        return _etag;
    }

    virtual int deserialize(Json::Value &root);

private:
    time_t _last_modified;
    std::string _etag;
};

END_CPPSDK_NAMESPACE
#endif

