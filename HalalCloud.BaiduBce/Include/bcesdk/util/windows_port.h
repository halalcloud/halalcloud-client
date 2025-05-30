﻿/**
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
#ifndef BAIDU_BOS_CPPSDK_BOS_UTIL_WINDOWS_PORT_H
#define BAIDU_BOS_CPPSDK_BOS_UTIL_WINDOWS_PORT_H

#ifdef _WIN32
#include <windows.h>
#include <time.h>

namespace baidu {
namespace bos {
namespace cppsdk {

int gettimeofday(struct timeval *tp, void *tzp);
void gmtime_r(const time_t *time, struct tm * tm_struct);
void localtime_r(const time_t *time, struct tm * tm_struct);
char * strptime(const char *buf, const char *fmt, struct tm *tm);

}}}
#define timegm _mkgmtime

#endif

#endif
