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
#ifndef BAIDU_BOS_CPPSDK_BOS_CLIENT_OPTIONS_H
#define BAIDU_BOS_CPPSDK_BOS_CLIENT_OPTIONS_H

#include <string>

#include "bcesdk/common/common.h"

BEGIN_CPPSDK_NAMESPACE

struct ClientOptions {
    enum auth_version_t {
        BOS_COMPATIBLE_AUTH,
        BCE_AUTH
    };

    ClientOptions() : endpoint("http://bj.bcebos.com"), sign_expire_seconds(1200),
            timeout(120), connect_timeout_ms(10 * 1000/*10s*/), retry(2), max_parallel(10), multi_part_size(10485760),
            calc_md5_on(false), cname_enabled(false), auto_cname_enabled(true), auto_append_bns_to_ua(false), 
            is_verify_ssl(false), follow_redirects_enabled_cnts(0) {}

    std::string endpoint;

    std::string user_agent;

    int sign_expire_seconds;

    // http request timeout in seconds
    int32_t timeout;
    // http connect timeout in milli seconds
    int32_t connect_timeout_ms;
    int retry;
    int max_parallel;

    int multi_part_size;
    //this is a switch to decide whether to calculate_md5 or not
    bool calc_md5_on;
    // enable self defined bucket domain
    bool cname_enabled;
    bool auto_cname_enabled;
    bool auto_append_bns_to_ua;

    //default not to verify
    bool is_verify_ssl;
    std::string ca_path;
    std::string ca_file;

    // allow redirects cnts for 3xx
    int32_t follow_redirects_enabled_cnts;
};

END_CPPSDK_NAMESPACE
#endif

