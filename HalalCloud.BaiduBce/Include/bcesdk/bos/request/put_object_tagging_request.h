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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_PUT_OBJECT_TAGGING_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_PUT_OBJECT_TAGGING_REQUEST_H

#include "bcesdk/util/util.h"
#include "bcesdk/bos/model/object_request.h"
#include "bcesdk/bos/model/object_meta_data.h"

BEGIN_CPPSDK_NAMESPACE

class PutObjectTaggingRequest : public  PutObjectRequest {
public:
    PutObjectTaggingRequest(const std::string &bucket_name, const std::string &object_name) :
        PutObjectRequest(bucket_name, object_name) {
    }

    virtual ~PutObjectTaggingRequest() {
    }

    void add_object_tagging(const std::string& key, const std::string& value) {
        TagInfo tag_info(key, value);
        _tag_list.push_back(tag_info);
    }

    int build_command_specific(HttpRequest *request) {
        request->set_method(HTTP_METHOD_PUT);
        request->add_parameter("tagging");
        if (!_tag_list.empty()) {
            std::string tag_str;
            for (const auto& tag : _tag_list) {
                tag_str += StringUtil::url_encode(tag.key, false) + 
                    "=" + StringUtil::url_encode(tag.value, false) + "&";
            }
            request->append_header("x-bce-tagging", tag_str);
        }
        UploadRequest::set_calc_md5_on(false); 
        return UploadRequest::fill_http_request(request);
    }

private:
};

END_CPPSDK_NAMESPACE
#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
