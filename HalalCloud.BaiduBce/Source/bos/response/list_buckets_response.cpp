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

#include "bcesdk/bos/response/list_buckets_response.h"
#include "bcesdk/util/util.h"
#include <json/json.h>

BEGIN_CPPSDK_NAMESPACE

int ListBucketsResponse::deserialize(Json::Value &root) {
    Json::Value &owner = root["owner"];
    _owner_id = owner["id"].asString();
    _owner_display_name = owner["displayName"].asString();

    Json::Value &buckets = root["buckets"];
    _buckets.resize(buckets.size());
    for (unsigned int i = 0; i < buckets.size(); i++) {
        BucketSummary &bucket = _buckets[i];
        bucket.name = buckets[i]["name"].asString();
        bucket.location = buckets[i]["location"].asString();
        bucket.creation_date = DateUtil::parse_iso8601_date(buckets[i]["creationDate"].asString());
    }
    return 0;
}

END_CPPSDK_NAMESPACE

