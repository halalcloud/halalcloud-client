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

#include "bcesdk/bos/model/upload_request.h"
#include "bcesdk/util/util.h"
#include "bcesdk/util/crypto_util.h"

BEGIN_CPPSDK_NAMESPACE
int UploadRequest::calculate_md5(std::string *digest) {
    InputStream *stream = _stream;
    if (stream == nullptr) {
        return 0;
    }
    const int size_per_block = 1024 * 64;
    std::vector<char> block;
    block.resize(size_per_block);
    int64_t saved_pos = stream->tell();
    MD5Util * md5_util = create_md5_util();
    int64_t ret = 0;
    while (true) {
        ret = stream->read(block.data(), size_per_block);
        if (ret < 0) {
            LOGF(ERROR, "read from stream failed: (%d)%s", stream->last_error(),
                    stream->last_error_msg().c_str());
            return RET_CLIENT_ERROR;
        }
        if (ret == 0) {
            break;
        }

        md5_util->Update(block.data(), (size_t)ret);
    }
    ret = stream->seek(saved_pos);
    unsigned char md[16];
    md5_util->Final(md);
    destroy_md5_util(md5_util);
    if (ret < 0) {
        LOGF(ERROR, "seek stream to %ld failed: (%d)%s", saved_pos, stream->last_error(),
                    stream->last_error_msg().c_str());
        return RET_CLIENT_ERROR;
    }
    digest->assign((const char *) md, sizeof(md));
    return 0;
}

int UploadRequest::calculate_sha256(std::string *digest) {
    InputStream *stream = _stream;
    if (stream == nullptr) {
        return 0;
    }
    const int size_per_block = 1024 * 64;
    std::vector<char> block;
    block.resize(size_per_block);
    int64_t saved_pos = stream->tell();

    SHA256Util * sha256_util = create_sha256_util();
    int64_t ret = 0;
    while (true) {
        ret = stream->read(block.data(), size_per_block);
        if (ret < 0) {
            LOGF(ERROR, "read from stream failed: (%d)%s", stream->last_error(),
                    stream->last_error_msg().c_str());
            return RET_CLIENT_ERROR;
        }
        if (ret == 0) {
            break;
        }
        sha256_util->Update(block.data(), (size_t)ret);
    }

    ret = stream->seek(saved_pos);

    unsigned char hash[32];
    sha256_util->Final(hash);
    destroy_sha256_uitl(sha256_util);

    if (ret < 0) {
        LOGF(ERROR, "seek stream to %ld failed: (%d)%s", saved_pos, stream->last_error(),
                    stream->last_error_msg().c_str());
        return RET_CLIENT_ERROR;
    }
    digest->assign(StringUtil::hex((const char *) hash, sizeof(hash)));
    return 0;
}

int UploadRequest::fill_http_request(HttpRequest *request) {
    if (_calc_md5_on) {
        std::string md5_sum;
        int ret = calculate_md5(&md5_sum);
        if (ret != 0) {
            LOGF(ERROR, "upload request calculate_md5 fail");
            return ret;
        }
        request->append_header("Content-MD5", StringUtil::base64_encode(md5_sum));
    }
    if (_traffic_limit) {
        request->append_header("x-bce-traffic-limit", _traffic_limit);
    }
    request->set_input_stream(_stream);
    return ServiceRequest::fill_http_request(request);
}

END_CPPSDK_NAMESPACE

