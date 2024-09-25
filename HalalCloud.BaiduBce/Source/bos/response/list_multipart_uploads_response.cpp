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
#include "bcesdk/bos/response/list_multipart_uploads_response.h"
#include "bcesdk/util/util.h"
#include <json/json.h>

BEGIN_CPPSDK_NAMESPACE

int ListMultipartUploadsResponse::deserialize(Json::Value &root) {
    _bucket = root["bucket"].asString();
    _marker = root["keyMarker"].asString();
    _next_marker = root["nextKeyMarker"].asString();
    _max_uploads = root["maxUploads"].asInt();
    _is_truncated = root["isTruncated"].asBool();
    _prefix = root["prefix"].asString();
    _delimiter = root["delimiter"].asString();

    Json::Value &prefixes = root["commonPrefixes"];
    for (unsigned int i = 0; i < prefixes.size(); i++){
        _common_prefixes.push_back(prefixes[i]["prefix"].asString());
    }
    Json::Value &array = root["uploads"];
    _uploads.resize(array.size());
    for (unsigned int i = 0; i < array.size(); i++) {
        Json::Value &upload = array[i];
        MultipartUploadSummary &tmp = _uploads[i];

        tmp.key = upload["key"].asString();
        tmp.upload_id = upload["uploadId"].asString();
        tmp.owner_id = upload["owner"]["id"].asString();
        tmp.owner_display_name = upload["owner"]["displayName"].asString();
        tmp.initiated = DateUtil::parse_iso8601_date(upload["initiated"].asString());
        tmp.storage_class = upload["storageClass"].asString();
    }
    return RET_OK;
}

END_CPPSDK_NAMESPACE

