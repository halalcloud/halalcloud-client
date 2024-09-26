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
#include "bcesdk/bos/response/list_parts_response.h"
#include "bcesdk/util/util.h"
#include <json/json.h>

BEGIN_CPPSDK_NAMESPACE

int ListPartsResponse::deserialize(Json::Value &root) {
    _bucket = root["bucket"].asString();
    _key = root["key"].asString();
    _upload_id = root["uploadId"].asString();

    _initiated = DateUtil::parse_iso8601_date(root["initiated"].asString());
    _owner_id = root["owner"]["id"].asString();
    _owner_display_name = root["owner"]["displayName"].asString();
    _marker = root["partNumberMarker"].asInt();
    _next_marker = root["nextPartNumberMarker"].asInt();
    _max_parts = root["maxParts"].asInt();
    _is_truncated = root["isTruncated"].asBool();
    _storage_class = root["storageClass"].asString();

    Json::Value &array = root["parts"];
    _parts.resize(array.size());
    for (unsigned int i = 0; i < array.size(); i++) {
        PartSummary &part = _parts[i];
        part.part_number = array[i]["partNumber"].asInt();
        std::string iso8601_date = array[i]["lastModified"].asString();
        part.last_modified = DateUtil::parse_iso8601_date(iso8601_date);
        part.etag = array[i]["eTag"].asString();
        part.size = array[i]["size"].asInt();
    }
    return RET_OK;
}

END_CPPSDK_NAMESPACE

