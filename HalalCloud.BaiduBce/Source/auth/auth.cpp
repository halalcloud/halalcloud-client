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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <algorithm>

#include "bcesdk/auth/auth.h"

BEGIN_CPPSDK_NAMESPACE

std::string DefaultSigner::get_ak(){
    return _credential.ak();
}

std::string DefaultSigner::get_sk(){
    return _credential.sk();
}


std::string DefaultSigner::generate_auth(HttpRequest *request, int expire_seconds) {
    std::stringstream auth_str;
    std::string sign_time = DateUtil::format_iso8601_date(time(NULL));
    auth_str << "bce-auth-v1/" << _credential.ak() << "/" << sign_time << "/" << expire_seconds;
    std::string sign_key = StringUtil::hmac_sha256_hex(_credential.sk(), auth_str.str());

    std::ostringstream canonical_req;
    // method
    canonical_req << method_str(request->method()) << '\n';
    // uri
    canonical_req << StringUtil::url_encode(request->uri(), false) << '\n';
    // query string
    const StringMap &params = request->parameters();
    if (params.size() > 0) {
        StringMap tmp;
        for (StringMap::const_iterator it = params.begin(); it != params.end(); ++it) {
            std::ostringstream p;
            p << StringUtil::url_encode(StringUtil::trim(it->first)) << '='
                << StringUtil::url_encode(StringUtil::trim(it->second));
            tmp[p.str()] = "";
        }
        StringMap::iterator it = tmp.begin();
        canonical_req << it->first;
        for (++it; it != tmp.end(); ++it) {
            canonical_req << '&' << it->first;
        }
    }
    canonical_req << '\n';

    bool use_sts = !_credential.sts_token().empty();
    // header
    canonical_req << "host:" << StringUtil::url_encode(request->host());
    if (use_sts) {
        canonical_req << "\nx-bce-security-token:" << StringUtil::url_encode(_credential.sts_token());
    }
    LOG(DEBUG) << "canonical request: [" << canonical_req.str() << ']';

    std::string signature = StringUtil::hmac_sha256_hex(sign_key, canonical_req.str());
    auth_str << "/host";
    if (use_sts) {
        auth_str << ";x-bce-security-token";
        request->append_header("x-bce-security-token", _credential.sts_token());
    }
    auth_str << "/" << signature;
    return auth_str.str();
}

void DefaultSigner::sign(HttpRequest *request) {
    request->append_header("Authorization", generate_auth(request, _sign_expire_seconds));
}

END_CPPSDK_NAMESPACE
