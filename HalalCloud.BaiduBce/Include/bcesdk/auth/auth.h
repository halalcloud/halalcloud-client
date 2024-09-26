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
#ifndef BAIDU_BOS_CPPSDK_BOS_AUTH_AUTH_H
#define BAIDU_BOS_CPPSDK_BOS_AUTH_AUTH_H

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "bcesdk/common/common.h"
#include "bcesdk/util/util.h"
#include "bcesdk/http/http_request.h"
#include "bcesdk/bos/client_options.h"

BEGIN_CPPSDK_NAMESPACE

class Signer {
public:
    virtual ~Signer() {};
    virtual std::string generate_auth(HttpRequest *request, int expire_seconds) = 0;
    virtual void sign(HttpRequest *request) = 0;
    virtual std::string get_ak() = 0;
    virtual std::string get_sk() = 0;
};

class DefaultSigner : public Signer {
public:
    DefaultSigner(const ClientOptions &options) {
         _sign_expire_seconds = options.sign_expire_seconds;
    }

    virtual std::string get_ak();

    virtual std::string get_sk();
    
    void set_credential(const Credential &credential) {
        _credential = credential;
    }
    virtual std::string generate_auth(HttpRequest *request, int expire_seconds);
    virtual void sign(HttpRequest *request);

private:
    int _sign_expire_seconds;
    Credential _credential;
};

END_CPPSDK_NAMESPACE
#endif

