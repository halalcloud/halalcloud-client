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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_PUT_OBJECT_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_PUT_OBJECT_REQUEST_H

#include "bcesdk/util/util.h"
#include "bcesdk/bos/model/object_meta_data.h"
#include "bcesdk/bos/model/object_meta_request.h"
#include "bcesdk/bos/model/upload_request.h"

BEGIN_CPPSDK_NAMESPACE

class PutObjectRequest : public ObjectMetaRequest, public UploadRequest {
public:
    PutObjectRequest(const std::string &bucket_name, const std::string &object_name) :
            ObjectMetaRequest(bucket_name, object_name) {}
    PutObjectRequest(const std::string &bucket_name, const std::string &object_name,
            const std::string &data) : ObjectMetaRequest(bucket_name, object_name) {
        set_data(data);
    }
    PutObjectRequest(const std::string &bucket_name, const std::string &object_name,
            InputStream *stream) : ObjectMetaRequest(bucket_name, object_name) {
        set_stream(stream);
    }
    PutObjectRequest(const std::string &bucket_name, const std::string &object_name,
            const std::string &data,
            const std::pair<std::string, std::string>  special_header) : 
        ObjectMetaRequest(bucket_name, object_name),
        _special_header(special_header) {
        set_data(data);
    }

    virtual ~PutObjectRequest() {}

    void add_object_tagging(const std::string& key, const std::string& value) {
        TagInfo tag_info(key, value);
        _tag_list.push_back(tag_info);
    }

    virtual int build_command_specific(HttpRequest *request) {
        request->set_method(HTTP_METHOD_PUT);
        append_all_meta_to(request);
        if (!_special_header.first.empty()) {
            request->append_header(_special_header.first, _special_header.second);
        }
        if (!_tag_list.empty()) {
            std::string tag_str;
            for (const auto& tag : _tag_list) {
                tag_str += StringUtil::url_encode(tag.key, false) + 
                    "=" + StringUtil::url_encode(tag.value, false) + "&";
            }
            request->append_header("x-bce-tagging", tag_str);
        }
        return UploadRequest::fill_http_request(request);
    }

protected:
    std::vector<TagInfo> _tag_list;

private:
    std::pair<std::string, std::string> _special_header;

};

END_CPPSDK_NAMESPACE
#endif

