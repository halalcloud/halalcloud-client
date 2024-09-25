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
#ifndef BAIDU_BOS_CPPSDK_BOS_RESPONSE_PUT_OBJECT_RESPONSE_H
#define BAIDU_BOS_CPPSDK_BOS_RESPONSE_PUT_OBJECT_RESPONSE_H

#include "bcesdk/model/bce_response.h"

BEGIN_CPPSDK_NAMESPACE

class PutObjectResponse : public BceResponse {
public:
    const std::string& etag() const {
        const std::string& etag = attribute("etag");
        if (etag.length() < 2 || etag[0] != '"' || etag[etag.length() - 1] != '"') {
            _etag = etag;
        }
        if (etag.empty()) {
            return etag;
        }
        _etag = etag.substr(1, etag.length() - 2);
        return _etag;
    }

    const std::string& version() const {
        const std::string&  version = attribute("x-bce-version");
        if (version.length() < 2 || version[0] != '"' || version[version.length() - 1] != '"') {
            _version = version;
        }
        if (version.empty()) {
            return version;
        }
        _version = version.substr(1, version.length() - 2);
        return _version;
    }
private:
    mutable std::string _etag;
    mutable std::string _version;
};

END_CPPSDK_NAMESPACE
#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
