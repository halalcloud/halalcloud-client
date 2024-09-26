/***************************************************************************
 * 
 * Copyright (c) 2016 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file src/http/http_request.cpp
 * @date 2016/11/30 17:55:34
 * @brief 
 *  
 **/

#include "bcesdk/http/http_request.h"
#include "bcesdk/util/util.h"
#include "bcesdk/common/stream.h"
#include <curl/curl.h>

BEGIN_CPPSDK_NAMESPACE

HttpRequest::~HttpRequest() {
    curl_slist_free_all(_header_list);
}

void HttpRequest::append_header(const char *header_line) {
    _header_list = curl_slist_append(_header_list, header_line);
}

void HttpRequest::append_header(const std::string &key, const std::string &value) {
    std::ostringstream header_str;
    header_str << key;
    if (value.empty()) {
        header_str << ';';
    } else {
        header_str << ':' << value;
    }
    _header_list = curl_slist_append(_header_list, header_str.str().c_str());
}

void HttpRequest::append_header(const std::string &key, long long val) {
    std::ostringstream header_str;
    header_str << key << ':' << val;
    _header_list = curl_slist_append(_header_list, header_str.str().c_str());
}

void HttpRequest::add_parameter(const std::string &key, int64_t value) {
    std::ostringstream out;
    out << value;
    _parameters[key] = out.str();
}

std::string HttpRequest::generate_url() const {
    std::ostringstream url;
    url << _protocol << "://" << _host << StringUtil::url_encode(_uri, false);
    if (_parameters.size() > 0) {
        url << '?';
        for (StringMap::const_iterator it = _parameters.begin(); it != _parameters.end(); ++it) {
            if (it != _parameters.begin()) {
                url << '&';
            }
            url << it->first;
            if (!it->second.empty()) {
                url << '=' << StringUtil::url_encode(it->second);
            }
        }
    }
    return url.str();
}

void HttpRequest::reset() {
    if (_in_stream != NULL) {
        _in_stream->seek(0);
    }
    _in_stream = NULL;
    curl_slist_free_all(_header_list);
    _header_list = NULL;
    _parameters.clear();
    _endpoint.clear();
    _protocol.clear();
    _host.clear();
    _uri.clear();
    _is_post_object_request = false;
}

END_CPPSDK_NAMESPACE

