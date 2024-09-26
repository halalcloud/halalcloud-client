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

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include "bcesdk/common/common.h"
#include "bcesdk/util/crypto_util.h"

BEGIN_CPPSDK_NAMESPACE
class OpenSSLMD5Util :public MD5Util {
public:
    OpenSSLMD5Util() {
        MD5_Init(&_md5_ctx);
    }

    virtual void Update(void *data, size_t len) {
        MD5_Update(&_md5_ctx, data, len);
    }

    virtual void Final(unsigned char *md) {
        MD5_Final(md, &_md5_ctx);
    }
private:
    MD5_CTX _md5_ctx;
};

class OpenSSLSHA256Util :public SHA256Util {
public:
    OpenSSLSHA256Util() {
        SHA256_Init(&_sha256_ctx);
    }

    virtual void Update(void *data, size_t len) {
        SHA256_Update(&_sha256_ctx, data, len);
    }

    virtual void Final(unsigned char *md) {
        SHA256_Final(md, &_sha256_ctx);
    }
private:
    SHA256_CTX _sha256_ctx;
};

MD5Util * create_md5_util() {
    return new OpenSSLMD5Util();
}

SHA256Util * create_sha256_util() {
    return new OpenSSLSHA256Util();
}

void destroy_md5_util(MD5Util * util) {
    delete util;
}

void destroy_sha256_uitl(SHA256Util * util) {
    delete util;
}

std::string hmac_sha256(const std::string &key, const std::string &data) {
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int md_len = 0;
    HMAC(EVP_sha256(), (const unsigned char *)key.data(), key.size(),
        (const unsigned char *)data.data(), data.size(), md, &md_len);
    return std::string((char *)md, md_len);
}

END_CPPSDK_NAMESPACE
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
