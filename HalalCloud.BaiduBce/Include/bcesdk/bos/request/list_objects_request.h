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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_LIST_OBJECTS_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_LIST_OBJECTS_REQUEST_H

#include "bcesdk/bos/model/bucket_request.h"

BEGIN_CPPSDK_NAMESPACE

class ListObjectsRequest : public BucketRequest {
public:
    ListObjectsRequest(const std::string &bucket_name) : BucketRequest(bucket_name), _max_keys(-1) {
    }

    virtual ~ListObjectsRequest() {}

    void set_delimiter(const std::string &delimiter) {
        _delimiter = delimiter;
    }

    void set_marker(const std::string &marker) {
        _marker = marker;
    }

    void set_max_keys(int max_keys) {
        _max_keys = max_keys;
    }

    void set_prefix(const std::string &prefix) {
        _prefix = prefix;
    }

    virtual int build_command_specific(HttpRequest *request) {
        request->set_method(HTTP_METHOD_GET);
        if (!_prefix.empty()) {
            request->add_parameter("prefix", _prefix);
        }
        if (!_marker.empty()) {
            request->add_parameter("marker", _marker);
        }
        if (!_delimiter.empty()) {
            request->add_parameter("delimiter", _delimiter);
        }
        if (_max_keys >= 0) {
            request->add_parameter("maxKeys", _max_keys);
        }
        return 0;
    }
private:
    int _max_keys;
    std::string _prefix;
    std::string _marker;
    std::string _delimiter;
};

END_CPPSDK_NAMESPACE
#endif

