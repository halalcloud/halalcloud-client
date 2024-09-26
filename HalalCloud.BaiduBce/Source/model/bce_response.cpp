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
#include "bcesdk/model/bce_response.h"
#include "bcesdk/util/util.h"
#include <json/json.h>

BEGIN_CPPSDK_NAMESPACE

void BceResponse::handle_response(HttpResponse &response) {
    clear_error();
    _request_id = response.header("x-bce-request-id");
    _debug_id = response.header("x-bce-debug-id");
    if (response.status_code() >= 400) {
        set_error_by_json(response.status_code(), response.get_error_message());
        return;
    }
    do_handle_chain(response);
}


void BceResponse::set_error_by_json(int status_code, const std::string &json) {
    _status_code = status_code;
    if (_error == NULL) {
        _error = new ServiceException();
    }
    if (json.empty()) {
        // may be HEAD and has no body
        _error->set_code("UnknownError");
        return;
    }

    Json::Reader reader(Json::Features::strictMode());
    Json::Value value;
    if (!reader.parse(json, value)) {
        LOG(ERROR) << "malformed json: '" << json << '\'';
        set_error(STATUS_INVALID_RESPONSE, "MalformedJson");
        return;
    }
    try {
        _error->set_code(value["code"].asString());
        _error->set_message(value["message"].asString());
        _error->set_request_id(value["requestId"].asString());
    } catch (...) {
        LOG(ERROR) << "invalid response json content: " << json;
        set_error(STATUS_INVALID_RESPONSE, "MalformedJson");
    }
}

END_CPPSDK_NAMESPACE

