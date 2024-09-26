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

#include <json/json.h>

#include "bcesdk/common/common.h"
#include "bcesdk/util/util.h"
#include "bcesdk/bos/response/get_object_tagging_response.h"

BEGIN_CPPSDK_NAMESPACE

int GetObjectTaggingResponse::deserialize(Json::Value &root) {
    try {
        Json::Value &arr = root["tagSet"];
        _tag_list.clear();
        for (unsigned int i = 0; i < arr.size(); i++) {
            Json::Value &tag = arr[i]["tagInfo"];
            TagInfo ti;
            Json::Value::Members member = tag.getMemberNames();
            for (Json::Value::Members::iterator iter = member.begin(); 
                    iter != member.end(); ++iter) {
                ti.key = (*iter);
                ti.value = tag[(*iter)].asString();
                _tag_list.push_back(ti);
            }
        }
    } catch (...) {
        LOGF(ERROR, "deserialize json to tag_set exceptional!");
        return RET_ILLEGAL_OPERATION;
    }
    return RET_OK;
}

END_CPPSDK_NAMESPACE

