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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_PUT_BUCKET_TRASH_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_PUT_BUCKET_TRASH_REQUEST_H
#include "bcesdk/bos/model/bucket_request.h"
#include "bcesdk/bos/model/upload_request.h"

BEGIN_CPPSDK_NAMESPACE

class PutBucketTrashRequest : public  BucketRequest, public UploadRequest {
public:
    PutBucketTrashRequest(const std::string &bucket_name, const std::string &trash_dir = "") :
        BucketRequest(bucket_name), UploadRequest() {
            std::string json_body;
            json_body += "{ \"trashDir\":\""; 
            json_body += trash_dir;
            json_body += "\"}";
            this->set_data(json_body);
    }

    virtual ~PutBucketTrashRequest() {}

    int build_command_specific(HttpRequest *request) {
        request->set_method(HTTP_METHOD_PUT);
        request->add_parameter("trash");
        UploadRequest::fill_http_request(request);
        return RET_OK;
    }
};

END_CPPSDK_NAMESPACE
#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
