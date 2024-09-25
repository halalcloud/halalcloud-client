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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_COPY_PART_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_COPY_PART_REQUEST_H

#include "bcesdk/bos/model/object_request.h"
#include "bcesdk/bos/model/copy_request.h"
#include <sstream>

BEGIN_CPPSDK_NAMESPACE

class CopyPartRequest : public ObjectRequest, public CopyRequest {
public:
    CopyPartRequest() : _range_start(-1), _range_end(-1){}
    virtual ~CopyPartRequest() {}

    void set_upload_id(const std::string &upload_id) {
        _upload_id = upload_id;
    }

    void set_part_number(int part_number) {
        _part_number = part_number;
    }
    int part_number() const {
        return _part_number;
    }

    void set_range(int64_t from, int64_t to) {
        _range_start = from;
        _range_end = to;
    }

    int build_command_specific(HttpRequest *request) {
        request->set_method(HTTP_METHOD_PUT);
        request->add_parameter("partNumber", _part_number);
        request->add_parameter("uploadId", _upload_id);
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
            request->append_header("x-bce-copy-source-range", range.str());
        }
        return CopyRequest::fill_http_request(request);
    }

private:
    std::string _upload_id;
    int _part_number;
    std::string _range;
    int64_t _range_start;
    int64_t _range_end;
};

END_CPPSDK_NAMESPACE
#endif

