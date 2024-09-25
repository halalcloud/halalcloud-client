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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_LIST_PARTS_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_LIST_PARTS_REQUEST_H

#include "bcesdk/bos/model/object_request.h"

BEGIN_CPPSDK_NAMESPACE

class ListPartsRequest : public ObjectRequest {
public:
    ListPartsRequest(const std::string &bucket_name, const std::string &object_name,
            const std::string &upload_id) :
        ObjectRequest(bucket_name, object_name), _upload_id(upload_id), _max_parts(-1) {
    }

    virtual ~ListPartsRequest() {}

    void set_max_parts(int max_parts) {
        _max_parts = max_parts;
    }

    void set_marker(const std::string &marker) {
        _marker = marker;
    }

    int build_command_specific(HttpRequest *request) {
        request->set_method(HTTP_METHOD_GET);
        request->add_parameter("uploadId", _upload_id);
        if (!_marker.empty()) {
            request->add_parameter("partNumberMarker", _marker);
        }
        if (_max_parts >= 0) {
            request->add_parameter("maxParts", _max_parts);
        }
        return 0;
    }
private:
    std::string _upload_id;
    std::string _marker;
    int _max_parts;
};

END_CPPSDK_NAMESPACE
#endif

