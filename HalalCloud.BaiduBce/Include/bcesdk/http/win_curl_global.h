/**
 * Copyright 2018 (c) Baidu, Inc.
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

#ifndef BAIDU_BOS_CPPSDK_HTTP_WIN_CURL_GLOBAL_H
#define BAIDU_BOS_CPPSDK_HTTP_WIN_CURL_GLOBAL_H

#ifdef _WIN32

#include <windows.h>
#include <vector>
#include "bcesdk/common/common.h"
#include "bcesdk/util/util.h"
#include <curl/curl.h>

BEGIN_CPPSDK_NAMESPACE
class CurlGlobal {
private:
    DWORD * get_global_share_key() {
        static DWORD _s_share_key;
        return &_s_share_key;
    }
public:
    CurlGlobal(long flags) {
        int rc = curl_global_init(flags);
        if (rc != CURLE_OK) {
            LOGF(FATAL, "curl_global_init failed with flags: %d, code: %d", flags, rc);
            exit(rc);
        }
        InitializeCriticalSection(&_mutex);
        *get_global_share_key() = TlsAlloc();
    }
    ~CurlGlobal() {
        curl_global_cleanup();
        for (size_t i = 0; i < _share_list.size(); ++i) {
            curl_share_cleanup(_share_list[i]);
        }
        DeleteCriticalSection(&_mutex);
    }
    CURLSH *thread_local_share() {
        CURLSH *cs = (CURLSH *)TlsGetValue(*get_global_share_key());
        if (cs == NULL) {
            cs = curl_share_init();
            if (cs == NULL) {
                LOG(ERROR) << "curl init share failed!";
                return NULL;
            }
            curl_share_setopt(cs, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
            curl_share_setopt(cs, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
            TlsSetValue(*get_global_share_key(), (void *)cs);

            EnterCriticalSection(&_mutex);
            _share_list.push_back(cs);
            LeaveCriticalSection(&_mutex);
        }
        return cs;
    }
private:
    CRITICAL_SECTION  _mutex;
    std::vector<CURLSH *> _share_list;
};

END_CPPSDK_NAMESPACE

#endif
#endif
