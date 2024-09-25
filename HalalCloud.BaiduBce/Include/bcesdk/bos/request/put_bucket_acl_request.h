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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_PUT_BUCKET_ACL_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_PUT_BUCKET_ACL_REQUEST_H

#include "bcesdk/bos/request/put_bucket_request.h"
#include "bcesdk/bos/model/upload_request.h"

BEGIN_CPPSDK_NAMESPACE

class PutBucketAclRequest : public PutBucketRequest, public UploadRequest {
public:
    PutBucketAclRequest(const std::string &bucket_name) :
        PutBucketRequest(bucket_name), _file_input_stream(NULL) {
    }

    PutBucketAclRequest(const std::string &bucket_name, const std::string &acl) :
        PutBucketRequest(bucket_name), _canned_acl(acl), _file_input_stream(NULL) {
    }

    virtual ~PutBucketAclRequest() {
        if (_file_input_stream) {
            delete _file_input_stream;
            _file_input_stream = NULL;
        }
    }

    void set_canned_acl(const std::string& canned_acl) {
        _canned_acl = canned_acl;
    }

    void set_json_acl(const std::string& json_acl) {
        _json_acl = json_acl;
    }

    void set_access_control_list(const std::vector<Grant>& access_control_list) {
        _access_control_list = access_control_list;
    }

    int set_acl_file(const std::string& file_path) {
        _file_input_stream = new FileInputStream(file_path);
        if (_file_input_stream && _file_input_stream->last_error() != RET_OK) {
            return _file_input_stream->last_error();
        }
        return RET_OK;
    }

    int build_command_specific(HttpRequest *request) {
        request->set_method(HTTP_METHOD_PUT);
        request->add_parameter("acl");

        if (!_canned_acl.empty()) {
            request->append_header("x-bce-acl", _canned_acl);
        } else if (!_json_acl.empty()) {
            UploadRequest::set_data(_json_acl);
        } else if (!_access_control_list.empty()) {
            std::string json_str;
            if (Grant::serialize_to_json(_access_control_list, json_str, true)) {
                return RET_ILLEGAL_ARGUMENT;
            } 
            UploadRequest::set_data(json_str); 
        } else if (_file_input_stream != NULL) {
            UploadRequest::set_stream(_file_input_stream);
        } else {
            return RET_ILLEGAL_ARGUMENT;
        }
        UploadRequest::set_calc_md5_on(false); 
        return UploadRequest::fill_http_request(request);;
    }

private:
    std::string _canned_acl;
    std::string _json_acl;
    std::vector<Grant> _access_control_list;
    FileInputStream * _file_input_stream;
};

END_CPPSDK_NAMESPACE
#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
