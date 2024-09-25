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
#ifndef BAIDU_BOS_CPPSDK_BOS_COMMON_COMMON_H
#define BAIDU_BOS_CPPSDK_BOS_COMMON_COMMON_H

#include <string>
#include <map>

#define BEGIN_CPPSDK_NAMESPACE \
namespace baidu { \
namespace bos { \
namespace cppsdk {

#define END_CPPSDK_NAMESPACE }}}
#define bcesdk_ns baidu::bos::cppsdk

BEGIN_CPPSDK_NAMESPACE

typedef std::map<std::string, std::string> StringMap;

enum LogLevel {
    SDK_LOG_FATAL = 0,
    SDK_LOG_ERROR = (1 << 1) - 1,
    SDK_LOG_WARN  = (1 << 2) - 1,
    SDK_LOG_INFO  = (1 << 3) - 1,
    SDK_LOG_DEBUG = (1 << 4) - 1
};

const char *sdk_package_string();

void sdk_set_log_level(LogLevel level);

// if using stream, sdk will convert stream to filename by readlink
void sdk_set_log_stream(FILE *fp);
void sdk_set_log_filename(const std::string &filename);

void sdk_set_log_rotate_size(int size);

// 0 for success, 400~599 for http failure, 1000~ for local error
enum RPCStatus {
    STATUS_OK = 0,
    STATUS_INTERNAL_ERROR = 500,
    STATUS_INVALID_RESPONSE = 1000,
    STATUS_RPC_FAIL = 1001,
};

// Cppsdk return code facilies
enum ret_code_t {
    RET_OK = 0,
    RET_SERVICE_ERROR = 1000,
    RET_CLIENT_ERROR,
    RET_KEY_NOT_EXIST,
    RET_INIT_CURL_FAIL,
    RET_AUTH_GEN_FAIL,
    RET_AUTH_SIGN_FAIL,
    RET_AUTH_WRONG_PREFIX,
    RET_ILLEGAL_ARGUMENT,
    RET_LOAD_MULTIPART_FAIL,
    RET_FILE_INVALID,
    RET_ILLEGAL_OPERATION,
    RET_PARALLEL_COPY_EXIT,
};
const char * stringfy_ret_code(int code);

class Credential {
public:
    Credential() {}
    Credential(const std::string &ak, const std::string &sk) : _ak(ak), _sk(sk) {}
    Credential(const std::string &ak, const std::string &sk, const std::string &sts_token)
        : _ak(ak), _sk(sk), _sts_token(sts_token) {}

    const std::string &ak() const { return _ak; }
    void set_ak(const std::string &ak) {
        _ak = ak;
    }

    const std::string &sk() const { return _sk; }
    void set_sk(const std::string &sk) {
        _sk = sk;
    }

    const std::string &sts_token() const { return _sts_token; }
    void set_sts_token(const std::string &sts_token) {
        _sts_token = sts_token;
    }

private:
    std::string _ak;
    std::string _sk;
    std::string _sts_token;
};

#ifdef _WIN32
typedef void * fd_t;    //created by CreateFile on Windows
#else
typedef int fd_t;       //created by open on Linux
#endif


typedef void (*TransferProgressCb)(int64_t increment, int64_t transfered, int64_t total, void* data);
struct TransferProgress {
    TransferProgress() : transfer_progress_cb(NULL), user_data(NULL) {}

    TransferProgressCb transfer_progress_cb;
    void * user_data;
};

// traffic-limit range (bit/s)
static const int64_t g_traffic_limit_max = 838860800;  // 100MB/s
static const int64_t g_traffic_limit_min = 819200;     // 100KB/s

static const std::string g_official_doamin_end = "bcebos.com";
static const std::string g_service_bucket_mock = "Bos_Service_Bucket_Mock";

END_CPPSDK_NAMESPACE
#endif

