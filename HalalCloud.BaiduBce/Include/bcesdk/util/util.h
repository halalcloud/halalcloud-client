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
#ifndef BAIDU_BOS_CPPSDK_BOS_UTIL_UTIL_H
#define BAIDU_BOS_CPPSDK_BOS_UTIL_UTIL_H

#include <stdint.h>
#include <time.h>

#include <stdlib.h>

#include <string>
#include <vector>
#include <sstream>

#include "bcesdk/common/common.h"
#include "bcesdk/util/crypto_util.h"

#ifndef _WIN32
#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100  + __GNUC_PATCHLEVEL__)
#endif
#endif

#ifndef LOGF
#define LOGF(level, fmt, ...) bcesdk_ns::LogUtil::should_log(bcesdk_ns::SDK_LOG_##level) ? \
    bcesdk_ns::LogUtil::logging(bcesdk_ns::SDK_LOG_##level, "%s:%d " fmt, __FILE__, __LINE__, ##__VA_ARGS__) : (void) 0
#endif

#ifndef LOG
#define LOG(level) if (bcesdk_ns::LogUtil::should_log(bcesdk_ns::SDK_LOG_##level)) \
    bcesdk_ns::LogStream(bcesdk_ns::SDK_LOG_##level) << __FILE__ << ':' << __LINE__ << ' '
#endif


BEGIN_CPPSDK_NAMESPACE

class LogUtil {
public:
    static const char *level_to_string(int level);
    static int string_to_level(std::string level);
    static inline bool should_log(int level) {
        return level <= _s_log_level;
    }
    static void logging(int level, const char * fmt, ...);

    static void check_and_rotate();

public:
    static LogLevel _s_log_level;
    static FILE *_s_log_stream;
    static std::string _s_log_filename;
    static int64_t _s_log_rotate_size;
};

class LogStream {
public:
    LogStream(int level) : _level(level) {};
    ~LogStream() {
        LogUtil::logging(_level, "%s", _stream.str().c_str());
    }
    template<typename T>
    inline LogStream &operator <<(const T &t) {
        _stream << t;
        return *this;
    }
private:
    std::ostringstream _stream;
    int _level;
};

class TimeUtil {
public:
    static int64_t now_ms();
    static int64_t now_us();
    static time_t now();
    static struct tm now_tm();
    static std::string format(const std::string &fmt, time_t ts);
    static std::string now_utctime();
    static std::string now_gmttime();
    static std::string timestamp_to_utctime(time_t);
    static std::string timestamp_to_gmttime(time_t);
    static int64_t utctime_to_timestamp(const std::string &);
    static int64_t gmttime_to_timestamp(const std::string &);

    static const char * UTC_FORMAT;
    static const char * GMT_FORMAT;
    static const int UTC_FORMAT_LENGTH;
};

class StringUtil {
public:
    template<typename T>
    static std::string to_string(const T &n) {
        std::ostringstream oss;
        oss << n;
        return oss.str();
    }

    static std::string trim(const std::string &s, const char *c = EMPTY_CHARS) {
        size_t b = s.find_first_not_of(c);
        if (b == std::string::npos) {
            return "";
        }
        size_t e = s.find_last_not_of(c);
        if (e == std::string::npos) {
            return s.substr(b);
        }
        return s.substr(b, e - b + 1);
    }

    static std::string ltrim(const std::string &s, const char *c = EMPTY_CHARS) {
        size_t b = s.find_first_not_of(c);
        if (b == std::string::npos) {
            return "";
        }
        return s.substr(b);
    }
    static std::string rtrim(const std::string &s, const char *c = EMPTY_CHARS) {
        size_t e = s.find_last_not_of(c);
        if (e == std::string::npos) {
            return s;
        }
        return s.substr(0, e + 1);
    }

    template<typename T>
    static void split(const std::string &s, T *result, const char *delimiter, int max = 0) {
        size_t last = 0;
        result->clear();
        for (int i = 0; !max || i < (max - 1); ++i) {
            size_t pos = s.find(delimiter, last);
            if (pos == std::string::npos) {
                break;
            }
            result->push_back(s.substr(last, pos - last));
            last = pos + 1;
        }
        result->push_back(s.substr(last));
    }

    static std::string lower(const std::string &s) {
        std::string result = s;
        for (size_t i = 0; i < s.size(); ++i) {
            result[i] = tolower(result[i]);
        }
        return result;
    }
    static std::string upper(const std::string &s) {
        std::string result = s;
        for (size_t i = 0; i < s.size(); ++i) {
            result[i] = toupper(result[i]);
        }
        return result;
    }

    static int hexchar2int(char c) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        } else if (c >= 'A' && c <= 'Z') {
            return c - 'A' + 10;
        } else if (c >= 'a' && c <= 'z') {
            return c - 'a' + 10;
        }
        return -1;
    }

    static std::string hex(const char *data, int size) {
        std::string result(size * 2, '0');
        for (int i = 0; i < size; ++i) {
            int c = (data[i] >> 4) & 0xf;
            if (c < 10) {
                result[i * 2] = c + '0';
            } else {
                result[i * 2] = (c - 10) + 'a';
            }
            c = data[i] & 0xf;
            if (c < 10) {
                result[i * 2 + 1] = c + '0';
            } else {
                result[i * 2 + 1] = (c - 10) + 'a';
            }
        }
        return result;
    }
    static std::string hex(const std::string &data) {
        return hex(data.data(), (int) data.size());
    }

    static std::string base64_encode(const std::string &src);
    static std::string base64_decode(const std::string &src);
    static std::string url_encode(const std::string &src, bool encode_slash = true);
    static std::string url_decode(const std::string &src);


    static std::string hmac_sha256_hex(const std::string &key, const std::string &data) {
        return hex(hmac_sha256(key, data));
    }

    static bool str2int(const std::string &n, int *result, int base = 10) {
        char *end = NULL;
        *result = strtol(n.c_str(), &end, base);
        return n.c_str() + n.length() == end;
    }
    static bool str2uint64(const std::string &n, unsigned long long *result, int base = 10) {
        char *end = NULL;
        *result = strtoull(n.c_str(), &end, base);
        return n.c_str() + n.length() == end;
    }
    static bool byteunit2double(const std::string &n, double *result);
    static bool byteunit2int(const std::string &n, int64_t *result);

private:
    static inline unsigned char base64_decode_char(const unsigned char);
    const static char * EMPTY_CHARS;
    const static char * BASE64_CHARS;
};

class DateUtil {
public:
    static time_t parse_rfc822_date(const std::string &date);
    static time_t parse_iso8601_date(const std::string &date);
    static std::string format_rfc822_date(time_t timestamp);
    static std::string format_iso8601_date(time_t timestamp);
};

END_CPPSDK_NAMESPACE
#endif
