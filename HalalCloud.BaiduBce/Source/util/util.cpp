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
#include <stdio.h>  /* for c-style IO */
#ifndef _WIN32
#include <syslog.h> /* for syslog */
#include <unistd.h>
#include <sys/time.h>
#else
#include "bcesdk/util/windows_port.h"
#pragma warning(disable : 4996)
#endif
#include <stdarg.h> /* for ISO C variable arguments*/
#include <ctype.h>  /* for upper/lower */
#include <limits.h> /* for UCHAR_MAX */
#include <stdarg.h> /* for ISO C variable arguments*/


#include <algorithm>/* for std algorithms */
#include <sstream>  /* string stream */



#include "bcesdk/util/util.h"

#ifndef VERSION
#include "config.h"
#endif

BEGIN_CPPSDK_NAMESPACE

LogLevel LogUtil::_s_log_level = SDK_LOG_DEBUG;
FILE *LogUtil::_s_log_stream = stderr;
std::string LogUtil::_s_log_filename;
int64_t LogUtil::_s_log_rotate_size = 1024 * 1024 * 1024;

const char *sdk_package_string() {
    return "baidu/bos/cppsdk@" VERSION;
}

void sdk_set_log_level(LogLevel level) {
    LogUtil::_s_log_level = level;
}
void sdk_set_log_stream(FILE *fp) {
    LogUtil::_s_log_stream = fp;
#ifndef _WIN32   //windows not support
    int fd = fileno(fp);
    if (fd < 0) {
        return;
    }
    std::ostringstream out;
    out << "/proc/self/fd/" << fd;
    char filename[1024];
    int n = readlink(out.str().c_str(), filename, sizeof(filename));
    if (n < 0) {
        return;
    }
    filename[n] = 0;
    LogUtil::_s_log_filename = filename;
#endif
}

void sdk_set_log_filename(const std::string &filename) {
    FILE *fp = fopen(filename.c_str(), "a");
    if (fp == NULL) {
        return;
    }
    LogUtil::_s_log_stream = fp;
    LogUtil::_s_log_filename = filename;
}

void sdk_set_log_rotate_size(int size) {
    LogUtil::_s_log_rotate_size = size;
}

const char * LogUtil::level_to_string(int level)
{
    switch (level) {
        case SDK_LOG_FATAL:
            return "FATAL";
        case SDK_LOG_ERROR:
            return "ERROR";
        case SDK_LOG_WARN:
            return "WARN";
        case SDK_LOG_INFO:
            return "INFO";
        case SDK_LOG_DEBUG:
            return "DEBUG";
        default:
            return "DEBUG";
    }
}

int LogUtil::string_to_level(std::string level) { 
    level = StringUtil::lower(level);
    if (level == "fatal") {
        return SDK_LOG_FATAL;
    } else if (level == "error") {
        return SDK_LOG_ERROR;
    } else if (level == "warn") {
        return SDK_LOG_WARN;
    } else if (level == "info") {
        return SDK_LOG_INFO;
    } else if (level == "debug") {
        return SDK_LOG_DEBUG;
    }
    return -1;
}

void LogUtil::check_and_rotate() {
    if (!_s_log_filename.empty()) {
        if (ftell(_s_log_stream) >= _s_log_rotate_size) {
            std::ostringstream out;
            out << _s_log_filename << '.' << TimeUtil::format("%Y%m%d%H%M%S", time(NULL));
            if (rename(_s_log_filename.c_str(), out.str().c_str()) != 0) {
                freopen(_s_log_filename.c_str(), "a", _s_log_stream);
            }
        }
    }
}

void LogUtil::logging(int level, const char * fmt, ...) {
    std::ostringstream final_fmt;
    int64_t ts = TimeUtil::now_ms();
    final_fmt << '[' << level_to_string(level) << ']' << ' '
        << TimeUtil::format("%Y%m%d %H:%M:%S.", ts / 1000) << (ts % 1000) << ' '
        << fmt << std::endl;
    va_list ap;
    va_start(ap, fmt);
    vfprintf(_s_log_stream, final_fmt.str().c_str(), ap);
    va_end(ap);

    fflush(LogUtil::_s_log_stream);
    check_and_rotate();
}

// TimeUtil definition
const char * TimeUtil::UTC_FORMAT= "%Y-%m-%dT%H:%M:%SZ";
const int TimeUtil::UTC_FORMAT_LENGTH= 20;
const char * TimeUtil::GMT_FORMAT= "%a, %d %b %Y %H:%M:%S GMT";

int64_t TimeUtil::now_ms()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return (int64_t)now.tv_sec * 1000 + (int64_t)now.tv_usec / 1000;
}

int64_t TimeUtil::now_us()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000000 + now.tv_usec;
}

time_t TimeUtil::now()
{
    return time(NULL);
}

struct tm TimeUtil::now_tm()
{
    time_t now_time = now();
    struct tm tm_struct;
    gmtime_r(&now_time, &tm_struct);
    return tm_struct;
}

std::string TimeUtil::now_utctime()
{
    return timestamp_to_utctime(now());
}

std::string TimeUtil::now_gmttime()
{
    return timestamp_to_gmttime(now());
}

std::string TimeUtil::format(const std::string &fmt, time_t ts) {
    struct tm t;
    localtime_r(&ts, &t);
    char buf[128];
    size_t size = strftime(buf, 128, fmt.c_str(), &t); 
    return std::string(buf, size);
}

std::string TimeUtil::timestamp_to_utctime(time_t ts)
{
    struct tm tm_result;
    gmtime_r(&ts, &tm_result);

    char buf[128];
    size_t size = strftime(buf, 128, UTC_FORMAT, &tm_result);

    std::string result(buf, size);
    return result;
}

std::string TimeUtil::timestamp_to_gmttime(time_t ts)
{
    struct tm tm_result;
    gmtime_r(&ts, &tm_result);

    char buf[128];
    size_t size = strftime(buf, 128, GMT_FORMAT, &tm_result);

    std::string result(buf, size);
    return result;
}

int64_t TimeUtil::utctime_to_timestamp(const std::string & utc)
{
    struct tm tm_result;
    if (strptime(utc.c_str(), UTC_FORMAT, &tm_result)
            != utc.c_str() + UTC_FORMAT_LENGTH) {
        return -1;
    }

    return static_cast<int64_t>(mktime(&tm_result));
}

int64_t TimeUtil::gmttime_to_timestamp(const std::string & gmt)
{
    struct tm tm_result;
    if (strptime(gmt.c_str(), GMT_FORMAT, &tm_result) == NULL) {
        return -1;
    }

    return static_cast<int64_t>(mktime(&tm_result));
}

//StringUtil definition
const char * StringUtil::EMPTY_CHARS = " \n\t\r\v\f";
const char * StringUtil::BASE64_CHARS = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

std::string StringUtil::base64_encode(const std::string &src)
{
    std::stringstream ss;
    ss.clear();

    size_t length = src.size();
    if (src.empty()) {
        return src;
    }

    unsigned char parts[4];
    for (size_t i = 0; i < length; i += 3) {
        parts[0] = (src[i] & 0xfc) >> 2;
        parts[1] = ((src[i] & 0x03) << 4) | (((length > (i + 1) ? src[i + 1] : 0x00) & 0xf0) >> 4);
        parts[2] = length > (i + 1) ? (((src[i + 1] & 0x0f) << 2) |
                (((length > (i + 2) ? src[i + 2] : 0x00) & 0xc0) >> 6)) : 0x40;
        parts[3] = length > (i + 2) ? (src[i + 2] & 0x3f) : 0x40;

        for (int j = 0; j < 4; ++j) {
            ss << BASE64_CHARS[parts[j]];
        }
    }
    return ss.str();
}

unsigned char StringUtil::base64_decode_char(const unsigned char c)
{
    unsigned char result = 0;
    if (isupper(c)) {
        result = static_cast<unsigned char>(c - 'A');
    } else if (islower(c)) {
        result = static_cast<unsigned char>(c - 'a' + 26);
    } else if (isdigit(c)) {
        result = static_cast<unsigned char>(c - '0' + 52);
    } else if ('+' == c) {
        result = 62;
    } else if ('/' == c) {
        result = 63;
    } else if ('=' == c) {
        result = 64;
    } else {
        result = UCHAR_MAX;
    }
    return result;
}

std::string StringUtil::base64_decode(const std::string &src)
{
    std::stringstream ss;
    ss.clear();
    size_t length = src.size();
    if (src.empty()) {
        return src;
    }

    unsigned char parts[4];
    for (size_t i = 0; i < length; i += 4) {
        parts[0] = base64_decode_char(src[i]);
        parts[1] = length > (i + 1) ? base64_decode_char(src[i + 1]) : 64;
        parts[2] = length > (i + 2) ? base64_decode_char(src[i + 2]) : 64;
        parts[3] = length > (i + 3) ? base64_decode_char(src[i + 3]) : 64;

        ss << static_cast<unsigned char>(((parts[0] << 2) & 0xfc) | ((parts[1] >> 4) & 0x03));
        if (64 == parts[2]) {
            break;
        }
        ss << static_cast<unsigned char>(((parts[1] << 4) & 0xf0) | ((parts[2] >> 2) & 0x0f));
        if (64 == parts[3]) {
            break;
        }
        ss << static_cast<unsigned char>(((parts[2] << 6) & 0xc0) | (parts[3] & 0x3f));
    }
    return ss.str();
}

std::string StringUtil::url_encode(const std::string &src, bool encode_slash) {
    std::ostringstream ss;
    for (size_t i = 0; i < src.size(); ++i) {
        char c = src[i];
        if ((c >= -1 && c <= 255 && isalnum(c)) || c == '_' || c == '-' || c == '~' || c == '.'
                || (c == '/' && !encode_slash)) {
            ss << c;
        } else {
            ss << '%';
            int tmp = (c >> 4) & 0xf;
            if (tmp >= 10) {
                tmp = tmp - 10 + 'A';
            } else {
                tmp = tmp + '0';
            }
            ss << (char) tmp;
            tmp = c & 0xf;
            if (tmp >= 10) {
                tmp = tmp - 10 + 'A';
            } else {
                tmp = tmp + '0';
            }
            ss << (char) tmp;
        }
    }
    return ss.str();
}

std::string StringUtil::url_decode(const std::string &src) {
    std::ostringstream ss;
    for (size_t i = 0; i < src.size();) {
        if (src[i] != '%') {
            ss << src[i++];
        } else {
            ss << (char) ((hexchar2int(src[i + 1]) << 4) | hexchar2int(src[i + 2]));
            i += 3;
        }
    }
    return ss.str();
}


bool StringUtil::byteunit2double(const std::string &n, double *result) {
    size_t pos = 0;
    for (; pos < n.size(); ++pos) {
        if (n[pos] != '.' && (n[pos] < '0' || n[pos] > '9')) {
            break;
        }
    }
    char *end = NULL;
    *result = strtod(n.c_str(), &end);
    if (&n[pos] != end) {
        return false;
    }
    std::string unit;
    while (pos < n.size()) {
        unit.append(1, toupper(n[pos++]));
    }
    if (unit.empty() || unit == "B") {
        return true;
    } else if (unit == "K" || unit == "KB") {
        *result *= 1024;
        return true;
    } else if (unit == "M" || unit == "MB") {
        *result *= 1024 * 1024;
        return true;
    }
    return false;
}

bool StringUtil::byteunit2int(const std::string &n, int64_t *result) {
    double tmp;
    if (byteunit2double(n, &tmp)) {
        *result = (int64_t)tmp;
        return true;
    }
    return false;
}


// Definition for return code
const char * stringfy_ret_code(int code)
{
    switch (code) {
        case RET_OK:
            return "OK";
        case RET_SERVICE_ERROR:
            return "BOS service occurs error";
        case RET_CLIENT_ERROR:
            return "BOS client occurs error";
        case RET_INIT_CURL_FAIL:
            return "Init CURL Net handle failed";
        case RET_AUTH_GEN_FAIL:
            return "Generate auth string failed";
        case RET_AUTH_SIGN_FAIL:
            return "signature for auth string failed";
        case RET_AUTH_WRONG_PREFIX:
            return "bce auth with wrong prefix";
        case RET_ILLEGAL_ARGUMENT:
            return "argument is illegal";
        case RET_LOAD_MULTIPART_FAIL:
            return "load multipart cache fail";
        case RET_FILE_INVALID:
            return "file pointer is invalid";
        case RET_ILLEGAL_OPERATION:
            return "operation is illegal";
        default:
            return "Unknown Error";
    }
}

time_t DateUtil::parse_rfc822_date(const std::string &date) {
    struct tm t;
    strptime(date.c_str(), "%a, %d %b %Y %H:%M:%S %Z", &t);
    return timegm(&t);
}

time_t DateUtil::parse_iso8601_date(const std::string &date) {
    struct tm t;
    strptime(date.c_str(), "%Y-%m-%dT%H:%M:%SZ", &t);
    return timegm(&t);
}

std::string DateUtil::format_rfc822_date(time_t timestamp) {
    struct tm t;
    char buf[128];
    gmtime_r(&timestamp, &t);
#ifdef _WIN32
    size_t size = strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &t);
#else
    size_t size = strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &t);
#endif
    return std::string(buf, size);
}

std::string DateUtil::format_iso8601_date(time_t timestamp) {
    struct tm t;
    char buf[128];
    gmtime_r(&timestamp, &t);
    size_t size = strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &t);
    return std::string(buf, size);
}

END_CPPSDK_NAMESPACE
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
