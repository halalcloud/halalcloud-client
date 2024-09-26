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
#ifndef BAIDU_BOS_CPPSDK_MODEL_BCE_REQUEST_H
#define BAIDU_BOS_CPPSDK_MODEL_BCE_REQUEST_H

#include <map>
#include <string>

#include "bcesdk/common/common.h"
#include "bcesdk/common/stream.h"
#include "bcesdk/http/http_request.h"
#include "bcesdk/bos/client_options.h"

BEGIN_CPPSDK_NAMESPACE

class BceRequest {
public:
    BceRequest() {}

    virtual ~BceRequest() {}

    void set_request_header(const std::string &key, const std::string &val);
    const std::map<std::string, std::string> & get_request_header() const
    {
        return _headers;
    }

    int build_http_request(HttpRequest *request, bool cname_enabled);

    int build_http_request(HttpRequest *request) {
        return build_http_request(request, false);
    };

    virtual std::string get_uri() const {
        return "/";
    }

    virtual std::string get_uri(bool cname_enabled) const {
        (void)cname_enabled;
        return "/";
    }

    virtual int build_command_specific(HttpRequest *request)
    {
        (void)request;
        return 0;
    }
    
    virtual bool is_object_request() {
        return false;
    } 

    void set_bucket_name(const std::string &bucket) {
        _bucket_name = bucket;
    }

    const std::string &bucket_name() const {
        return _bucket_name;
    }

private:
    std::map<std::string, std::string> _headers;
    std::string                        _request_id;
    std::string                        _bucket_name;
};

END_CPPSDK_NAMESPACE
#endif

