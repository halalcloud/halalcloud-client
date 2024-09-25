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

#include "bcesdk/bos/response/get_bucket_location_response.h"
#include <json/json.h>

BEGIN_CPPSDK_NAMESPACE

int GetBucketLocationResponse::deserialize(Json::Value &root) {
    _location = root["locationConstraint"].asString();
    return RET_OK;
}

END_CPPSDK_NAMESPACE

