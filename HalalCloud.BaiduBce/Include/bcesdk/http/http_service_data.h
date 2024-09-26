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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_HTTP_SERVICE_DATA_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_HTTP_SERVICE_DATA_H

#include <stdint.h>
#include <string>
#include "bcesdk/common/common.h"

BEGIN_CPPSDK_NAMESPACE

struct HttpServiceData {
    HttpServiceData() :
            trans_progress_cb(NULL),
            trans_progress_data(NULL),
            transferred_size(0),
            total_size(0) {}

    void reset() {
        transferred_size = 0;
        total_size = 0;
    }

    TransferProgressCb trans_progress_cb;
    void * trans_progress_data;
    int64_t transferred_size;
    int64_t total_size;
};

END_CPPSDK_NAMESPACE
#endif
