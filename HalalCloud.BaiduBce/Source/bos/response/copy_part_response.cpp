﻿/**
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
#include "bcesdk/bos/response/copy_part_response.h"
#include "bcesdk/util/util.h"
#include <json/json.h>

BEGIN_CPPSDK_NAMESPACE

int CopyPartResponse::deserialize(Json::Value &root) {
    Json::Value &etag = root["eTag"];
    if (etag.isNull()) {
        set_error_by_json(STATUS_INTERNAL_ERROR, buffer());
        return 0;
    }
    _etag = etag.asString();
    _last_modified = DateUtil::parse_iso8601_date(root["lastModified"].asString());
    return RET_OK;
}

END_CPPSDK_NAMESPACE

