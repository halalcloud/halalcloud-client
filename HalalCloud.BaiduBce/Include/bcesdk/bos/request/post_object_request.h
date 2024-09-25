/**
 * Copyright 2024 (c) Baidu, Inc.
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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_POST_OBJECT_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_POST_OBJECT_REQUEST_H

#include "bcesdk/util/util.h"
#include "bcesdk/bos/model/object_meta_data.h"
#include "bcesdk/bos/model/bucket_request.h"
#include "bcesdk/bos/model/upload_request.h"
#include "bcesdk/common/file_stream.h"

BEGIN_CPPSDK_NAMESPACE

class PostObjectRequest : public BucketRequest, public UploadRequest {

public:
    PostObjectRequest(const std::string &bucket_name, const std::string &object_name) : BucketRequest(bucket_name), _key(object_name) {
        _policy_bucket_name = bucket_name;
    }

    // with policy
    PostObjectRequest(const std::string &bucket_name, const std::string &object_name, const std::string expiration_time, 
                      const int content_length_range_start = 0, const int content_length_range_end = 0) : BucketRequest(bucket_name), 
                      _key(object_name), _expiration_time(expiration_time), _content_length_range_start(content_length_range_start), 
                      _content_length_range_end(content_length_range_end) {
        _policy_bucket_name = bucket_name;
    }

    virtual ~PostObjectRequest() {
        if (_file_input_stream != NULL) {
            delete _file_input_stream;
            _file_input_stream = NULL;
        }
        if (_body_stream) {
            delete _body_stream;
            _body_stream = NULL;
        }
    }

    void set_boundary(const std::string &boundary) {
        _boundary = boundary;
    }

    void set_access_key(const std::string &access_key) {
        _access_key = access_key;
    }   

    void set_secret_access_key(const std::string &secret_access_key) {
        _secret_access_key = secret_access_key;
    }

    void set_storage_class(const std::string &storage_class) {
        _storage_class = storage_class;
    }

    void set_object_name(const std::string &object_name) {
        _key = object_name;
    }

    void set_is_public_bucket(bool is_public) {
        _is_public = is_public;
    }
    
    void set_expiration_time(const std::string &expiration_time) {
        _expiration_time = expiration_time;
    }

    void set_content_length_range_start(int start) {
        _content_length_range_start = start;
    }

    void set_content_length_range_end(int end) {
        _content_length_range_end = end;
    }

    void set_cache_control(const std::string &cache_control) {
        _cache_control = cache_control;
    }

    void set_content_disposition(const std::string &content_disposition) {
        _content_disposition = content_disposition;
    }

    void set_expires(const std::string &expires) {
        _expires = expires;
    }

    std::string get_policy() {
        if (build_policy() == RET_OK) {
            return _policy;
        } else{
            return "";
        }
    }

    std::string get_signature() {
        if (build_signature() == RET_OK) {
            return _signature;
        } else{
            return "";
        }
    }

    int set_file_name(const std::string &file_name) {
        if (_file_input_stream != NULL) {
            delete _file_input_stream;
            _file_input_stream = NULL;
        }
        _file_name = file_name;
        _file_input_stream = new FileInputStream(_file_name);
        if (_file_input_stream && _file_input_stream->last_error() != RET_OK) {
            return _file_input_stream->last_error();
        }
        return RET_OK;
    }

    int build_policy();

    int build_signature();

    void append_form_data_part(std::string& body_string, const std::string& name, const std::string& value);

    virtual int build_command_specific(HttpRequest *request) {
        int ret = RET_OK;
        request->set_method(HTTP_METHOD_POST);
        request->append_header("Content-Type", "multipart/form-data; boundary=" + _boundary);
        request->set_is_post_object_request(true);

        //check if this post object request bucket is public,
        //if not, check all the required parameters for building policy and signature
        if (!_is_public) {
            if (_access_key.empty() || _secret_access_key.empty()) {
                return RET_ILLEGAL_OPERATION;
            } else {
                ret = build_policy();
                if (ret != RET_OK) {
                    return ret;
                }
                ret = build_signature();
                if (ret != RET_OK) {
                    return ret;
                }
            }
        }    
        
        std::string body = "\r\n\r\n"; 
        if (!_access_key.empty() && !_is_public) {
            append_form_data_part(body, "accessKey", _access_key);
        }
        if (!_key.empty()) {
            append_form_data_part(body, "key", _key);
        }
        if (!_policy.empty()&& !_is_public) {
            append_form_data_part(body, "policy", _policy);
        }
        if (!_signature.empty()&& !_is_public) {
            append_form_data_part(body, "signature", _signature);
        }
        if (!_storage_class.empty()) {
            append_form_data_part(body, "storageClass", _storage_class);
        }
        if (!_cache_control.empty()) {
            append_form_data_part(body, "Cache-Control", _cache_control);
        }
        if (!_content_disposition.empty()) {
            append_form_data_part(body, "Content-Disposition", _content_disposition);
        }
        if (!_expires.empty()) {
            append_form_data_part(body, "Expires", _expires);
        }
        if (!_file_name.empty()) {
            append_form_data_part(body, "file", _file_name);
        }

        // append file content to body
        std::string file_content;
        if (_file_input_stream) {
            _file_input_stream->read(_file_input_stream->get_size(), &file_content);
        }
        if (!file_content.empty()) {
            body += file_content;
        }

        body += "\r\n--" + _boundary + "--";

        _body_stream = new MemoryInputStream(body.data(), body.size());
        UploadRequest::set_stream(_body_stream);
        UploadRequest::fill_http_request(request);
        return RET_OK;
    }

private:
    std::string _key;
    std::string _file_name;
    std::string _boundary = "bound1234567890ary";
    
    std::string _expiration_time = "";
    int _content_length_range_start = 0;
    int _content_length_range_end = 0;
    
    FileInputStream* _file_input_stream = NULL;
    MemoryInputStream* _body_stream = NULL;

    std::string _access_key;
    std::string _secret_access_key;
    std::string _storage_class;

    std::string _cache_control;
    std::string _content_disposition;
    std::string _expires;

    std::string _policy_bucket_name;
    std::string _policy;
    std::string _signature;
    bool _is_public = false;
};

END_CPPSDK_NAMESPACE
#endif
