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
#include "bcesdk/bos/response/list_objects_response.h"
#include "bcesdk/util/util.h"
#include <json/json.h>

BEGIN_CPPSDK_NAMESPACE

int ListObjectsResponse::deserialize(Json::Value &root) {
    _name = root["name"].asString();
    _prefix = root["prefix"].asString();
    _delimiter = root["delimiter"].asString();
    _marker = root["marker"].asString();
    _next_marker = root["nextMarker"].asString();
    _max_keys = root["maxKeys"].asInt();
    _is_truncated = root["isTruncated"].asBool();

    Json::Value &prefixes = root["commonPrefixes"];
    for (unsigned int i = 0; i < prefixes.size(); i++){
        _common_prefixes.push_back(prefixes[i]["prefix"].asString());
    }

    Json::Value &content = root["contents"];
    _contents.resize(content.size());
    for (unsigned int i = 0; i < content.size(); i++){
        ObjectSummary &ct = _contents[i];

        Json::Value &temp = content[i];
        ct.key = temp["key"].asString();
        ct.last_modified = DateUtil::parse_iso8601_date(temp["lastModified"].asString());
        ct.etag = temp["eTag"].asString();
        ct.size = temp["size"].asInt64();
        ct.storage_class = temp["storageClass"].asString();

        Json::Value &owner = temp["owner"];
        ct.owner_id = owner["id"].asString();
        ct.owner_display_name = owner["displayName"].asString();
    }
    return RET_OK;
}

END_CPPSDK_NAMESPACE
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
