/**
 * Copyright 2014 (c) Baidu, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 */
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_HTTP_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_HTTP_REQUEST_H

#include <stdint.h>

#include <string>

#include "bcesdk/common/common.h"
#include "bcesdk/http/http_service_data.h"
#include "bcesdk/bos/client_options.h"
#include "bcesdk/util/util.h"

struct curl_slist;

BEGIN_CPPSDK_NAMESPACE

enum HttpMethod {
    HTTP_METHOD_INVALID,
    HTTP_METHOD_PUT,
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_DELETE
};

static inline const char *method_str(HttpMethod method) {
    switch (method) {
    case HTTP_METHOD_PUT:
        return "PUT";
    case HTTP_METHOD_GET:
        return "GET";
    case HTTP_METHOD_POST:
        return "POST";
    case HTTP_METHOD_HEAD:
        return "HEAD";
    case HTTP_METHOD_DELETE:
        return "DELETE";
    default:
        return "UNKNOWN";
    }
}

class InputStream;

class HttpRequest : public HttpServiceData {
public:
    HttpRequest() : _header_list(NULL) {
        _method = HTTP_METHOD_INVALID;
        _in_stream = NULL;
        _timeout = 30;
    }
    ~HttpRequest();

    void set_input_stream(InputStream *in_stream) {
        _in_stream = in_stream;
    }

    InputStream *get_input_stream() const {
        return _in_stream;
    }

    void set_method(HttpMethod method) {
        _method = method;
    }

    HttpMethod method() const {
        return _method;
    }

    void append_header(const char *header_line);
    void append_header(const std::string &key, const std::string &val);
    void append_header(const std::string &key, long long val);

    void add_parameter(const std::string &key, const std::string &value) {
        _parameters[key] = value;
    }
    void add_parameter(const std::string &flag) {
        _parameters[flag] = "";
    }
    void add_parameter(const std::string &key, int64_t value);

    const StringMap &parameters() const {
        return _parameters;
    }

    const struct curl_slist *header_list() const {
        return _header_list;
    }

    int get_timeout() const {
        return _timeout;
    }

    int get_connect_timeout_ms() const {
        return _connect_timeout_ms;
    }

    void set_timeout(int timeout) {
        _timeout = timeout;
    }

    void set_connect_timeout_ms(int timeout_ms) {
        _connect_timeout_ms = timeout_ms;
    }

    bool set_endpoint(const std::string &endpoint, const ClientOptions &client_options, 
                                const std::string &bucket_name = "") {
        size_t pos = endpoint.find("://");
        if (pos == std::string::npos) {
            // "region.bcebos.com" default is http, or use bns or vip
            _protocol = "http";
            _host = endpoint;
        } else {
            // bns:// or http:// or https://
            _protocol = endpoint.substr(0, pos);
            _host = endpoint.substr(pos + 3);
        }
        pos = _host.find('/');
        if (pos != std::string::npos) {
            _host = _host.substr(0, pos);
        }

        // change endpoint to bucket style
        bool is_change_to_cname = false;
        if (!client_options.cname_enabled && client_options.auto_cname_enabled
                    && !bucket_name.empty()) {
            std::vector<std::string> result;
            StringUtil::split(_host, &result, ".");     
            int part_num = result.size();
            bool end_with_official = false;
            if (_host.rfind(g_official_doamin_end) == _host.length() - g_official_doamin_end.length()) {
                end_with_official = true;
            }
            bool start_with_bucket = false;
            if (_host.find(bucket_name) == 0) {
                start_with_bucket = true;
            }
            bool is_service_cmd = false;
            if (bucket_name == g_service_bucket_mock) {
                is_service_cmd = true;
            }
            if (part_num == 3) {
                if (end_with_official) {
                    // "region.bcebos.com"
                    if (!is_service_cmd) {
                        _host = bucket_name + '.' + _host;
                        is_change_to_cname = true;
                    }
                }
            } else if (part_num == 4) {
                if (is_service_cmd) {
                    if (end_with_official) {
                        _host = result[1] + "." + result[2] + "." + result[3];
                    }
                } else {
                    if (start_with_bucket && end_with_official) {
                        // "bucket.region.bcebos.com"
                        is_change_to_cname = true;
                    }
                }
            } else {
                // others && diy domain need set cname_enabled = true
                is_change_to_cname = false;
            }
        }

        if (!is_change_to_cname && client_options.cname_enabled) {
            is_change_to_cname = true;
        }
        return is_change_to_cname;
    }

    const std::string &host() const {
        return _host;
    }

    void set_uri(const std::string &uri) {
        _uri = uri;
    }
    const std::string &uri() const {
        return _uri;
    }

    std::string generate_url() const;

    std::string to_string() {
        return std::string(method_str(_method)) + " " + generate_url();
    }
    
    void set_is_post_object_request(bool is_post_object) {
        _is_post_object_request = is_post_object;
    }

    bool is_post_object_request() const {
        return _is_post_object_request;
    }

    // reset to state before build that can rebuild again
    void reset();

private:
    InputStream *_in_stream;
    std::string _endpoint;
    std::string _protocol;
    std::string _host;
    std::string _uri;
    struct curl_slist *_header_list;
    StringMap _parameters;
    HttpMethod _method;
    // seconds
    int _timeout;
    // milli seconds
    int _connect_timeout_ms;
    bool _is_post_object_request = false;
};

END_CPPSDK_NAMESPACE
#endif
