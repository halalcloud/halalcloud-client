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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_ABORT_MULTIPART_UPLOAD_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_ABORT_MULTIPART_UPLOAD_REQUEST_H

#include "bcesdk/bos/model/object_request.h"

BEGIN_CPPSDK_NAMESPACE

class AbortMultipartUploadRequest : public ObjectRequest {
public:
    AbortMultipartUploadRequest(const std::string &bucket_name, const std::string &object_name,
            const std::string &upload_id) : ObjectRequest(bucket_name, object_name),
            _upload_id(upload_id) {
    }

    virtual ~AbortMultipartUploadRequest() {}

    virtual int build_command_specific(HttpRequest *request) {
        request->set_method(HTTP_METHOD_DELETE);
        request->add_parameter("uploadId", _upload_id);
        return 0;
    }
private:
    std::string _upload_id;
};

END_CPPSDK_NAMESPACE
#endif

