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
#ifndef BAIDU_BOS_CPPSDK_BOS_RESPONSE_LIST_MULTIPART_UPLOADS_RESPONSE_H
#define BAIDU_BOS_CPPSDK_BOS_RESPONSE_LIST_MULTIPART_UPLOADS_RESPONSE_H

#include <sstream>
#include <string>
#include <vector>

#include "bcesdk/model/json_response.h"

BEGIN_CPPSDK_NAMESPACE

struct MultipartUploadSummary {
    std::string key;
    std::string upload_id;
    time_t initiated;
    std::string owner_id;
    std::string owner_display_name;
    std::string storage_class;
};

class ListMultipartUploadsResponse : public JsonResponse {
public:
    ListMultipartUploadsResponse() {
    }

    virtual ~ListMultipartUploadsResponse() {}

    const std::string &bucket() const {
        return _bucket;
    }

    const std::string &marker() const {
        return _marker;
    }

    const std::string &next_marker() const {
        return _next_marker;
    }

    const std::string &delimiter() const {
        return _delimiter;
    }

    int max_uploads() const {
        return _max_uploads;
    }

    bool is_truncated() const {
        return _is_truncated;
    }

    const std::vector<MultipartUploadSummary> &uploads() const {
        return _uploads;
    }
    std::vector<MultipartUploadSummary> &uploads() {
        return _uploads;
    }

    const std::vector<std::string> &common_prefixes() const {
        return _common_prefixes;
    }

    const std::string &prefix() const {
        return _prefix;
    }

    virtual int deserialize(Json::Value &root);

private:
    std::string _delimiter;
    std::string _bucket;
    std::string _marker;
    std::string _next_marker;
    int _max_uploads;
    bool _is_truncated;
    std::vector<MultipartUploadSummary> _uploads;
    std::vector<std::string> _common_prefixes;
    std::string _prefix;
};

END_CPPSDK_NAMESPACE
#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
