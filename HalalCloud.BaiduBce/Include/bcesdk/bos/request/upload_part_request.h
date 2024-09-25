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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_UPLOAD_PART_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_UPLOAD_PART_REQUEST_H

#include "bcesdk/bos/model/object_request.h"
#include "bcesdk/common/memory_stream.h"

BEGIN_CPPSDK_NAMESPACE

class UploadPartRequest : public ObjectRequest, public UploadRequest {
public:
    UploadPartRequest(const std::string &bucket_name, const std::string &object_name,
            int part_number, const std::string &upload_id) :
        ObjectRequest(bucket_name, object_name), _upload_id(upload_id), _part_number(part_number) {
    }
    UploadPartRequest(const std::string &bucket_name, const std::string &object_name,
            const std::string &data, int part_number, const std::string &upload_id) :
        ObjectRequest(bucket_name, object_name), _upload_id(upload_id), _part_number(part_number) {
        set_allocated_stream(new MemoryInputStream(data.data(), data.size()));
    }

    UploadPartRequest(const std::string &bucket_name, const std::string &object_name,
            InputStream *stream, int part_number, const std::string &upload_id) :
        ObjectRequest(bucket_name, object_name), _upload_id(upload_id), _part_number(part_number) {
        set_stream(stream);
    }

    virtual ~UploadPartRequest() {}

    void set_upload_id(const std::string &upload_id) {
        _upload_id = upload_id;
    }

    void set_part_number(int part_number) {
        _part_number = part_number;
    }
    int part_number() const {
        return _part_number;
    }

    int build_command_specific(HttpRequest *request) {
        request->set_method(HTTP_METHOD_PUT);
        request->add_parameter("partNumber", _part_number);
        request->add_parameter("uploadId", _upload_id);
        return UploadRequest::fill_http_request(request);
    }

private:
    std::string _upload_id;
    int _part_number;
};

END_CPPSDK_NAMESPACE
#endif

