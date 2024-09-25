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
#ifndef BAIDU_BOS_CPPSDK_BOS_RESPONSE_LIST_BUCKET_RESPONSE_H
#define BAIDU_BOS_CPPSDK_BOS_RESPONSE_LIST_BUCKET_RESPONSE_H

#include <string>
#include <map>
#include <vector>

#include "bcesdk/model/json_response.h"

BEGIN_CPPSDK_NAMESPACE

struct BucketSummary {
   std::string name;
   std::string location;
   time_t creation_date;
};

class ListBucketsResponse : public JsonResponse {
public:
    ListBucketsResponse() {}

    virtual ~ListBucketsResponse() {}

    const std::string &owner_id() const {
        return _owner_id;
    }
    const std::string &owner_display_name() const {
        return _owner_display_name;
    }

    const std::vector<BucketSummary> buckets() const {
        return _buckets;
    }

    virtual int deserialize(Json::Value &root);

private:
    std::string _owner_id;
    std::string _owner_display_name;
    std::vector<BucketSummary> _buckets;
};

END_CPPSDK_NAMESPACE
#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
