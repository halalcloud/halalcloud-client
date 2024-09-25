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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_BUCKET_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_BUCKET_REQUEST_H

#include <sstream>
#include <string>

#include "bcesdk/common/common.h"
#include "bcesdk/bos/client_options.h"
#include "bcesdk/model/bce_request.h"

BEGIN_CPPSDK_NAMESPACE

class BucketRequest : public BceRequest {
public:
    BucketRequest() {}
    BucketRequest(const std::string &bucket_name) {
        set_bucket_name(bucket_name);
    }

    virtual std::string get_uri(bool cname_enabled) const
    {
        std::stringstream ss;
        if (!cname_enabled) {
            ss << "/" << bucket_name();
        } else {
            ss << "/";
        }
        return ss.str();
    }
};

END_CPPSDK_NAMESPACE
#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
