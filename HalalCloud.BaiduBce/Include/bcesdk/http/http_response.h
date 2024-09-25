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
#ifndef BAIDU_BOS_CPPSDK_BOS_RESPONSE_HTTP_RESPONSE_H
#define BAIDU_BOS_CPPSDK_BOS_RESPONSE_HTTP_RESPONSE_H

#include <map>
#include <string>
#include <sstream>
#include <vector>

#include "bcesdk/common/common.h"
#include "bcesdk/common/stream.h"
#include "bcesdk/http/http_service_data.h"

BEGIN_CPPSDK_NAMESPACE

class OutputStream;

class HttpResponse : public HttpServiceData {
public:
    HttpResponse() {
        _body_stream = NULL;
        _has_recv_status_line = false;
    }

    void set_output_stream(OutputStream * data) {
        _body_stream = data;
    }

    OutputStream *get_output_stream() const {
        return _body_stream;
    }

    void set_http_version(const std::string & data) {
        _http_version = data;
    }
    const std::string &http_version() const {
        return _http_version;
    }

    void set_status_code(const int data) {
        _status_code = data;
    }
    int status_code() const {
        return _status_code;
    }

    void set_reason_phrase(const std::string &data) {
        _reason_phrase = data;
    }
    const std::string &reason_phrase() const {
        return _reason_phrase;
    }

    std::string get_error_message() const
    {
        return _error_stream.str();
    }

    const StringMap &header() const {
        return _header;
    }
    const std::string &header(const std::string &key) const {
        StringMap::const_iterator it = _header.find(key);
        if (it != _header.end()) {
            return it->second;
        }
        static const std::string empty_string;
        return empty_string;
    }

    int write_header(const std::string &line);
    int write_body(void *ptr, size_t size);

    void reset() {
        if (_body_stream != NULL) {
            _body_stream->seek(0);
        }
        _error_stream.clear();
        _http_version.clear();
        _status_code = 0;
        _reason_phrase.clear();
        _header.clear();
        _has_recv_status_line = false;
    }
private:
    int parse_status_line(const std::string &status_line);
    int parse_header_line(const std::string &header_line, std::string *key, std::string *value);

    OutputStream *_body_stream;
    std::stringstream _error_stream;
    std::string _http_version;
    int _status_code;
    std::string _reason_phrase;
    StringMap _header;
    bool _has_recv_status_line;
};

END_CPPSDK_NAMESPACE
#endif

