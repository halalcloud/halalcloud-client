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
#ifndef BAIDU_BOS_CPPSDK_BOS_MODEL_OBJECT_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_MODEL_OBJECT_REQUEST_H

#include <sstream>
#include <string>

#include "bcesdk/model/bce_request.h"
#include "bcesdk/bos/client_options.h"

BEGIN_CPPSDK_NAMESPACE

class ObjectRequest : public BceRequest {
public:
    ObjectRequest() {}
    ObjectRequest(const std::string &bucket_name, const std::string &object_name) {
        set_bucket_name(bucket_name);
        _object_name = object_name;
    }
    virtual ~ObjectRequest() {
    }

    virtual std::string get_uri() const {
        return get_uri(false);
    }

    virtual std::string get_uri(bool cname_enabled) const
    {
        std::stringstream ss;
        if (!cname_enabled) {
            ss << "/" << bucket_name() << "/" << _object_name;
        } else {
            ss << "/" << _object_name;
        }
        return ss.str();
    }

    void set_object_name(const std::string &object) {
        _object_name = object;
    }
    const std::string &object_name() const {
        return _object_name;
    }

    virtual bool is_object_request() override {
        return true;
    }

private:
    std::string _object_name;
};

END_CPPSDK_NAMESPACE
#endif

