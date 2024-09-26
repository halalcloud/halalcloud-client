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
#ifndef BAIDU_BOS_CPPSDK_BOS_MODEL_COPY_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_MODEL_COPY_REQUEST_H

#include "bcesdk/common/common.h"
#include "bcesdk/http/http_request.h"
BEGIN_CPPSDK_NAMESPACE

class CopyRequest {
public:
    CopyRequest() : _traffic_limit(0) {}
    CopyRequest(const std::string &bucket, const std::string &object) : _traffic_limit(0) {
        _source_bucket_name = bucket;
        _source_object_name = object;
    }
    void set_source_bucket_name(const std::string &name) {
        _source_bucket_name = name;
    }
    void set_source_object_name(const std::string &name) {
        _source_object_name = name;
    }
    void set_if_match(const std::string &value) {
        _if_match = value;
    }
    void set_if_none_match(const std::string &value) {
        _if_none_match = value;
    }
    void set_if_unmodifed_since(const std::string &value) {
        _if_unmodifed_since = value;
    }
    void set_if_modified_since(const std::string &value) {
        _if_modified_since = value;
    }

    int64_t traffic_limit() {
        return _traffic_limit;
    }
    void set_traffic_limit(int64_t traffic_limit) {
        if (traffic_limit < g_traffic_limit_min || traffic_limit > g_traffic_limit_max) {
            traffic_limit = g_traffic_limit_min;
        }
        _traffic_limit = traffic_limit;
    }

    int fill_http_request(HttpRequest *request);
private:
    std::string _source_bucket_name;
    std::string _source_object_name;

    std::string _if_match;
    std::string _if_none_match;
    std::string _if_unmodifed_since;
    std::string _if_modified_since;
    int64_t     _traffic_limit;
};

END_CPPSDK_NAMESPACE

#endif

