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
#ifndef BAIDU_BOS_CPPSDK_BOS_RESPONSE_LIST_PARTS_RESPONSE_H
#define BAIDU_BOS_CPPSDK_BOS_RESPONSE_LIST_PARTS_RESPONSE_H

#include <sstream>
#include <vector>

#include "bcesdk/model/json_response.h"

BEGIN_CPPSDK_NAMESPACE

struct PartSummary {
    int part_number;
    time_t last_modified;
    std::string etag;
    int size;
};

class ListPartsResponse : public JsonResponse {
public :
    ListPartsResponse() {
        _marker = -1;
        _next_marker = -1;
        _max_parts = -1;
        _initiated = 0;
        _is_truncated = false;
        _storage_class = "";
    }

    virtual ~ListPartsResponse() {}

    const std::string &bucket() const {
        return _bucket;
    }

    const std::string &key() const {
        return _key;
    }

    const std::string &upload_id() const {
        return _upload_id;
    }

    time_t initiated() const {
        return _initiated;
    }

    int marker() const {
        return _marker;
    }

    int next_marker() const {
        return _next_marker;
    }

    int max_parts() const {
        return _max_parts;
    }

    bool is_truncated() const {
        return _is_truncated;
    }

    const std::string &owner_id() const {
        return _owner_id;
    }

    const std::string &owner_display_name() const {
        return _owner_display_name;
    }

    const std::vector<PartSummary> &parts() const {
        return _parts;
    }
   
    const std::string& storage_class() const {
        return _storage_class;
    }

    virtual int deserialize(Json::Value &root);

private:
    std::string _bucket;
    std::string _key;
    std::string _upload_id;
    time_t _initiated;
    int _marker;
    int _next_marker;
    int _max_parts;
    bool _is_truncated;
    std::string _owner_id;
    std::string _owner_display_name;
    std::string _storage_class;
    std::vector<PartSummary> _parts;
};

END_CPPSDK_NAMESPACE
#endif

