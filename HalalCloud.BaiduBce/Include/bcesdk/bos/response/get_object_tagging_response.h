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
#ifndef BAIDU_BOS_CPPSDK_BOS_RESPONSE_GET_OBJECT_TAGGING_RESPONSE_H
#define BAIDU_BOS_CPPSDK_BOS_RESPONSE_GET_OBJECT_TAGGING_RESPONSE_H

#include <vector>

#include "bcesdk/model/json_response.h"
#include "bcesdk/bos/model/object_meta_data.h"

BEGIN_CPPSDK_NAMESPACE

class GetObjectTaggingResponse : public JsonResponse {
public:
    GetObjectTaggingResponse() {}
    virtual ~GetObjectTaggingResponse() {}

    const std::vector<TagInfo> &tag_set() const {
        return _tag_list;
    }

    virtual int deserialize(Json::Value &root);

private:
    std::vector<TagInfo> _tag_list;
};

END_CPPSDK_NAMESPACE
#endif

