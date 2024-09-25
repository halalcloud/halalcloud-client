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
#ifndef BAIDU_BOS_CPPSDK_MODEL_BCE_RESPONSE_H
#define BAIDU_BOS_CPPSDK_MODEL_BCE_RESPONSE_H

#include <map>
#include <vector>
#include <string>

#include "bcesdk/common/common.h"
#include "bcesdk/common/memory_stream.h"
#include "bcesdk/http/http_response.h"
#include "bcesdk/bos/model/service_request.h"

namespace Json {
    class Value;
}

BEGIN_CPPSDK_NAMESPACE

class ServiceException {
public:
    ServiceException() {}
    void set_message(const std::string &message) {
        _message = message;
    }
    const std::string &message() const {
        return _message;
    }

    void set_code(const std::string &error_code) {
        _code = error_code;
    }
    const std::string &code() const {
        return _code;
    }

    void set_request_id(const std::string &request_id) {
        _request_id = request_id;
    }
    const std::string &request_id() const {
        return _request_id;
    }

private:
    std::string _code;
    std::string _request_id;
    std::string _message;
};

class BceResponse : public ServiceRequest {
public:
    BceResponse() : _status_code(STATUS_OK), _error(NULL) {}
    virtual ~BceResponse() {
        clear_error();
    }

    bool is_fail() const { return _error != NULL; }
    bool is_ok() const { return _error == NULL; }

    const ServiceException &error() const {
        static const ServiceException no_error;
        if (_error != NULL) {
            return *_error;
        }
        return no_error;
    }

    const std::string &request_id() const {
        return _request_id;
    }
    const std::string &debug_id() const {
        return _debug_id;
    }

    const std::string &attribute(const std::string &name) const {
        return _http_response.header(name);
    }

    // 0 for success, 400~599 for http failure, 1000~ for local error
    int status_code() const {
        return _status_code;
    }

    HttpResponse *mutable_http_response() {
        return &_http_response;
    }

#ifdef GCC_VERSION
#if GCC_VERSION >= 40500
    __attribute__ ((deprecated("use status_code() instead")))
#else
    __attribute__ ((deprecated))
#endif
#endif
    const HttpResponse &http_response() const {
        return _http_response;
    }

    void set_output_stream(OutputStream *output_stream) {
        _http_response.set_output_stream(output_stream);
    }

    void set_error_by_json(int status_code, const std::string &json);

    void set_error(int status_code, const std::string &code, const std::string &msg = "") {
        if (_error == NULL) {
            _error = new ServiceException();
        }
        _error->set_code(code);
        _error->set_message(msg);
        _error->set_request_id(_request_id);
        _status_code = status_code;
    }

    void clear_error() {
        if (_error != NULL) {
            delete _error;
            _error = NULL;
        }
        // clear _status_code for retry request.
        _status_code = STATUS_OK;
    }

    void handle_response(HttpResponse &response);

    virtual void do_handle_chain(HttpResponse &response) {
        (void) response;
    }

private:
    std::string _request_id;
    std::string _debug_id;
    int _status_code;
    ServiceException *_error;
    HttpResponse _http_response;
};

END_CPPSDK_NAMESPACE
#endif

