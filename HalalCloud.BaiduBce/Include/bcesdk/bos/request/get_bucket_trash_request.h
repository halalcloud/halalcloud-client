﻿/**
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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_GET_BUCKET_TRASH_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_GET_BUCKET_TRASH_REQUEST_H

#include "bcesdk/bos/model/bucket_request.h"

BEGIN_CPPSDK_NAMESPACE

class GetBucketTrashRequest : public  BucketRequest {
public:
    GetBucketTrashRequest(const std::string &bucket_name) : BucketRequest(bucket_name) {
    }

    virtual ~GetBucketTrashRequest() {}

protected:
    int build_command_specific(HttpRequest *request) {
        request->set_method(HTTP_METHOD_GET);
        request->add_parameter("trash", "");
        return 0;
    }
};

END_CPPSDK_NAMESPACE
#endif
