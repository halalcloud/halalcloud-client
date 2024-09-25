/*
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
#ifndef BAIDU_BOS_CPPSDK_BOS_COMMON_STREAM_H
#define BAIDU_BOS_CPPSDK_BOS_COMMON_STREAM_H

#include <stdint.h>

#include <string>

#include "bcesdk/common/common.h"

BEGIN_CPPSDK_NAMESPACE

class Stream {
public:
    virtual ~Stream() {}

    virtual int last_error() { return 0; }
    virtual std::string last_error_msg() { return ""; }

    virtual int64_t seek(int64_t pos) = 0;
    virtual int64_t tell() const = 0;
};

class InputStream : public Stream {
public:
    virtual ~InputStream() {}

    virtual int64_t read(char *buffer, int64_t size) = 0;
    virtual int64_t get_size() const = 0;
};

class OutputStream : public Stream {
public:
    virtual ~OutputStream() {}

    virtual int64_t write(const char *buffer, int64_t size) = 0;
    virtual int64_t reserve(int64_t size) = 0;
};

END_CPPSDK_NAMESPACE

#endif

