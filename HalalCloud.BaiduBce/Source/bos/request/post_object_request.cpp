/**
 * Copyright 2034 (c) Baidu, Inc.
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

#include <sstream>
#include "bcesdk/bos/request/post_object_request.h"
#include "bcesdk/util/util.h"
#include <json/json.h>

BEGIN_CPPSDK_NAMESPACE

int PostObjectRequest::build_policy() {
    if (_key.empty() || _policy_bucket_name.empty() || _content_length_range_start < 0 
        || _content_length_range_end < _content_length_range_start) {
        return RET_ILLEGAL_OPERATION;
    }
    std::string json_str;
    Json::Value root;
    root["expiration"] = Json::Value(_expiration_time);
    
    Json::Value conditions;
    Json::Value bucket;
    Json::Value key;
    bucket["bucket"] = Json::Value(_policy_bucket_name);
    key["key"] = Json::Value(_key);
    conditions.append(bucket);
    conditions.append(key);
    if (_content_length_range_start != 0 && _content_length_range_end != 0){
        Json::Value length_range_array;
        length_range_array.append("content-length-range");
        length_range_array.append(_content_length_range_start);
        length_range_array.append(_content_length_range_end);;
        conditions.append(length_range_array);
    }
    root["conditions"] = conditions;

    std::string unencoded_policy;
    Json::FastWriter writer;
    unencoded_policy = writer.write(root);
    _policy = StringUtil::base64_encode(unencoded_policy);
    return RET_OK;
}

int PostObjectRequest::build_signature() {
    if (_policy.empty() || _secret_access_key.empty()) {
        return RET_ILLEGAL_OPERATION;
    }
    _signature = StringUtil::hmac_sha256_hex(_secret_access_key, _policy);
    return RET_OK;
}

void PostObjectRequest::append_form_data_part(std::string& body_string, 
            const std::string& name, const std::string& value) {
    std::stringstream ss;
    ss << "--" << _boundary << "\r\n";
    ss << "Content-Disposition: form-data; name=\"" << name << "\"";
    if (name == "file") {
        ss << "; filename=" << value << "\r\n" << "Content-Type: application/octet-stream\r\n\r\n";
        body_string += ss.str();
        return;
    }
    ss << "\r\n\r\n";
    ss << value << "\r\n";
    body_string += ss.str();
    return;
}

END_CPPSDK_NAMESPACE
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
