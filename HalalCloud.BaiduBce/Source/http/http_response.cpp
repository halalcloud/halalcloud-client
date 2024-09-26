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
#include "bcesdk/http/http_response.h"
#include "bcesdk/util/util.h"
#include "bcesdk/common/memory_stream.h"
#include <string.h>
#include <stdlib.h>

BEGIN_CPPSDK_NAMESPACE

int HttpResponse::parse_status_line(const std::string &status_line)
{
    std::vector<std::string> items;
    StringUtil::split(status_line, &items, " ", 3);
    if (items.size() != 3U) {
        LOG(ERROR) << "status_line format error, status_line: '" << status_line << "'";
        return RET_SERVICE_ERROR;
    }

    _http_version = StringUtil::trim(items[0]);
    if (!StringUtil::str2int(StringUtil::trim(items[1]), &_status_code)) {
        LOG(ERROR) << "status_line format error, status_line: '" << status_line << "'";
        return RET_SERVICE_ERROR;
    }
    _reason_phrase = StringUtil::trim(items[2]);
    return 0;
}

int HttpResponse::parse_header_line(const std::string &header_line,
        std::string *key, std::string *value)
{
    std::vector<std::string> items;
    StringUtil::split(header_line, &items, ":", 2);
    if (items.size() != 2U) {
        {
            // for last redirected request, re-get http status header
            if (_status_code >= 300 && _status_code <= 399) {
                *key = "mock-parse-header-as-status";
                *value = "mock";
                LOG(DEBUG) << "last request has been 3xx, and try re-get status, header line: '" << header_line << "'";
                return parse_status_line(header_line);
            }
        }
        LOG(ERROR) << "parse_header_line error, header_line: '" << header_line << "'";
        return RET_SERVICE_ERROR;
    }
    *key = StringUtil::lower(StringUtil::trim(items[0]));
    *value = StringUtil::trim(items[1]);
    return 0;
}

int HttpResponse::write_header(const std::string &line) {
    if (line.empty()) {
        return 0;
    }
    if (_has_recv_status_line) {
        std::string key;
        std::string value;
        int ret = parse_header_line(line, &key, &value);
        if (ret != 0) {
            return ret;
        }
        if (key == "content-length" && _body_stream != NULL) {
            long long content_length = strtoll(value.c_str(), NULL, 10);
            int64_t ret = _body_stream->reserve(content_length);
            if (ret != 0) {
                LOGF(ERROR, "%s", stringfy_ret_code(RET_CLIENT_ERROR));
                return 0;
            }
            total_size = content_length;
        }
        _header[key] = value;
        return 0;
    } else {
        _has_recv_status_line = true;
        return parse_status_line(line);
    }
}

int HttpResponse::write_body(void *ptr, size_t size) {
    if (_status_code >= 400) {
        _error_stream << std::string((char *) ptr, size);
        return (int)size;
    }
    if (_body_stream == NULL) {
        return (int)size;
    }
    return (int)_body_stream->write(reinterpret_cast<char *>(ptr), size);
}

END_CPPSDK_NAMESPACE

