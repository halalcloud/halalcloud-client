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
#ifndef BAIDU_BOS_CPPSDK_BOS_MODEL_SERVICE_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_MODEL_SERVICE_REQUEST_H

#include <iostream>
#include "bcesdk/common/common.h"
#include "bcesdk/http/http_request.h"
#include "bcesdk/http/http_response.h"

BEGIN_CPPSDK_NAMESPACE

class ServiceRequest {
public:
    ServiceRequest() : _transfer_progress_cb(NULL), _transfer_progress_data(NULL) {
    }
    virtual ~ServiceRequest() {}

    TransferProgressCb get_transfer_process_cb() {
        return _transfer_progress_cb;
    }
    void* get_transfer_process_data() {
        return _transfer_progress_data;
    }

    void set_progress(TransferProgress progress) {
        _transfer_progress_cb = progress.transfer_progress_cb;
        _transfer_progress_data = progress.user_data;
    }

    int fill_http_request(HttpRequest *request) {
        if (_transfer_progress_cb) {
            request->trans_progress_cb = _transfer_progress_cb;
            request->trans_progress_data = _transfer_progress_data;
        }
        return 0;
    }

    int fill_http_response(HttpResponse* http_response) {
        if (_transfer_progress_cb) {
            http_response->trans_progress_cb = _transfer_progress_cb;
            http_response->trans_progress_data = _transfer_progress_data;
            http_response->transferred_size = 0;
            http_response->total_size = 0;
        }
        return 0;
    }

private:
    TransferProgressCb _transfer_progress_cb;
    void * _transfer_progress_data;
};

END_CPPSDK_NAMESPACE
#endif

