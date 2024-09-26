/**
 * Copyright 2019 (c) Baidu, Inc.
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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_RESTORE_OBJECT_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_RESTORE_OBJECT_REQUEST_H

#include "bcesdk/bos/model/object_request.h"

BEGIN_CPPSDK_NAMESPACE

class RestoreObjectRequest : public ObjectRequest {
public:
    RestoreObjectRequest(const std::string &bucket_name, const std::string &object_name): 
      ObjectRequest(bucket_name, object_name) {
        _restore_days = 0;        
    }

    virtual ~RestoreObjectRequest() {}

    void set_restore_tier(const std::string &tier) {
        _restore_tire = tier;
    }

    void set_restore_days(int days) {
        _restore_days = days;
    }

    virtual int build_command_specific(HttpRequest *request) {
        request->set_method(HTTP_METHOD_POST);
        request->add_parameter("restore");
        if (!_restore_tire.empty())  {
            request->append_header("x-bce-restore-tier", _restore_tire);
        }
        if (_restore_days > 0) {
            request->append_header("x-bce-restore-days", _restore_days);
        }
        return 0;
    }
private:
    std::string _restore_tire;
    int _restore_days;
};

END_CPPSDK_NAMESPACE
#endif

