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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_GET_OBJECT_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_GET_OBJECT_REQUEST_H

#include "bcesdk/bos/model/object_request.h"

BEGIN_CPPSDK_NAMESPACE

class GetObjectRequest : public ObjectRequest {
public:
    GetObjectRequest() : _range_start(-1), _range_end(-1), _traffic_limit(0) {}
    GetObjectRequest(const std::string &bucket_name, const std::string &object_name)
        : ObjectRequest(bucket_name, object_name), _range_start(-1), _range_end(-1), _traffic_limit(0) {}

    virtual ~GetObjectRequest() {}

    void set_range(int64_t start, int64_t end) {
        _range_start = start;
        _range_end = end;
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

    int build_command_specific(HttpRequest *request) {
        request->set_method(HTTP_METHOD_GET);
        if (_range_start >= 0 || _range_end >= 0) {
            std::ostringstream range;
            range << "bytes=";
            if (_range_start >= 0) {
                range << _range_start;
            }
            range << '-';
            if (_range_end >= 0) {
                range << _range_end;
            }
            request->append_header("Range", range.str());
        }
        if (_traffic_limit) {
            request->append_header("x-bce-traffic-limit", _traffic_limit);
        }
        return 0;
    }

    long long range_start() const {
        return _range_start;
    }
    long long range_end() const {
        return _range_end;
    }

private:
    long long _range_start;
    long long _range_end;
    int64_t   _traffic_limit;
};

END_CPPSDK_NAMESPACE
#endif

