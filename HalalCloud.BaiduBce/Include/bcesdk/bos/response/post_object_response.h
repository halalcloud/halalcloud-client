/**
 * Copyright 2024 (c) Baidu, Inc.
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
#ifndef BAIDU_BOS_CPPSDK_BOS_RESPONSE_POST_OBJECT_RESPONSE_H
#define BAIDU_BOS_CPPSDK_BOS_RESPONSE_POST_OBJECT_RESPONSE_H

#include "bcesdk/model/bce_response.h"

BEGIN_CPPSDK_NAMESPACE

class PostObjectResponse : public BceResponse {
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

    const std::string& content_md5() const {
        const std::string& content_md5 = attribute("Content-MD5");
        if (content_md5.length() < 2 || content_md5[0] != '"' || content_md5[content_md5.length() - 1] != '"') {
            _content_md5 = content_md5;
        }
        if (content_md5.empty()) {
            return content_md5;
        }
        _content_md5 = content_md5.substr(1, content_md5.length() - 2);
        return _content_md5;
    }


private:
    mutable std::string _content_md5;
    mutable std::string _etag;
};

END_CPPSDK_NAMESPACE
#endif

