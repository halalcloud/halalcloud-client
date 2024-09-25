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
#ifndef BAIDU_BOS_CPPSDK_BOS_CLIENT_IMPL_H
#define BAIDU_BOS_CPPSDK_BOS_CLIENT_IMPL_H

#include "bcesdk/bos/client.h"
#include "bcesdk/auth/auth.h"

BEGIN_CPPSDK_NAMESPACE

class ClientImpl {
public:
    ClientImpl(const Credential &credential, const ClientOptions &options);
    virtual ~ClientImpl();

    const ClientOptions &options() const { return _options; }

    const std::string get_access_key(){
        return _signer->get_ak();
    }

    const std::string get_secret_key(){
        return _signer->get_sk();
    }
    std::string generate_url(BceRequest &request, int expire_seconds);
    int build_http_request(BceRequest &request, HttpRequest *http_request);
    int build_http_response(BceResponse *response, HttpResponse* http_response);
    int send_request(BceRequest &request, BceResponse *response);
    int send_request(int n, BceRequestContext ctx[], int max_parallel = 0);
private:
    ClientOptions _options;
    Signer *_signer;
};

END_CPPSDK_NAMESPACE
#endif

