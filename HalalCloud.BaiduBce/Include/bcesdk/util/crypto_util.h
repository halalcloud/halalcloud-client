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
#ifndef BAIDU_BOS_CPPSDK_BOS_UTIL_CRYPTO_UTIL_H
#define BAIDU_BOS_CPPSDK_BOS_UTIL_CRYPTO_UTIL_H

#include "bcesdk/common/common.h"
#include <string>

BEGIN_CPPSDK_NAMESPACE
class MD5Util {
public:
    virtual ~MD5Util() {}
    virtual void Update(void *data, size_t len) = 0;
    virtual void Final(unsigned char *md) = 0;

};

class SHA256Util {
public:
    virtual ~SHA256Util() {}
    virtual void Update(void *data, size_t len) = 0;
    virtual void Final(unsigned char *md) = 0;
};

MD5Util * create_md5_util();
SHA256Util * create_sha256_util();
void destroy_md5_util(MD5Util * util);
void destroy_sha256_uitl(SHA256Util * util);
std::string hmac_sha256(const std::string &key, const std::string &data);

END_CPPSDK_NAMESPACE
#endif
