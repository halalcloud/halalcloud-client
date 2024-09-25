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
#ifndef BAIDU_BOS_CPPSDK_BOS_RESPONSE_LIST_OBJECTS_RESPONSE_H
#define BAIDU_BOS_CPPSDK_BOS_RESPONSE_LIST_OBJECTS_RESPONSE_H

#include <vector>
#include <string>
#include <sstream>

#include "bcesdk/model/json_response.h"

BEGIN_CPPSDK_NAMESPACE

struct ObjectSummary {
    std::string key;
    time_t last_modified;
    std::string etag;
    long long size;
    std::string storage_class;
    std::string owner_id;
    std::string owner_display_name;
};

class ListObjectsResponse : public JsonResponse {
public:
    ListObjectsResponse() {
        _max_keys = 1000;
        _is_truncated = false;
    }

    virtual ~ListObjectsResponse() {}

    const std::string &delimiter() const {
        return _delimiter;
    }

    bool is_truncated() const {
        return _is_truncated;
    }

    int max_keys() const {
        return _max_keys;
    }

    const std::string &marker() const {
        return _marker;
    }

    const std::string &name() const {
        return _name;
    }

    const std::string &next_marker() const {
        return _next_marker;
    }

    const std::string &prefix() const {
        return _prefix;
    }

    const std::vector<ObjectSummary> &contents() const {
        return _contents;
    }
    std::vector<ObjectSummary> &contents() {
        return _contents;
    }

    const std::vector<std::string> &common_prefixes() const {
        return _common_prefixes;
    }
    std::vector<std::string> &common_prefixes() {
        return _common_prefixes;
    }

    virtual int deserialize(Json::Value &root);

private:
    std::string _prefix;
    std::string _next_marker;
    std::string _name;
    std::string _marker;
    int _max_keys;
    bool _is_truncated;
    std::string _delimiter;
    std::vector<std::string> _common_prefixes;
    std::vector<ObjectSummary> _contents;
};

END_CPPSDK_NAMESPACE
#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
