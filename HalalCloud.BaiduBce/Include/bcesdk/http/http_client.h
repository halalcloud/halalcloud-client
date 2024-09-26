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
#ifndef BAIDU_BOS_CPPSDK_HTTP_HTTP_CLIENT_H
#define BAIDU_BOS_CPPSDK_HTTP_HTTP_CLIENT_H

#include "bcesdk/http/http_request.h"
#include "bcesdk/http/http_response.h"
#include <map>
#include <deque>

BEGIN_CPPSDK_NAMESPACE

struct HttpRequestContext {
    HttpRequestContext() : request(NULL), response(NULL), rc(0), retry(0), timestamp_ms(0),
        data(NULL) {}
    HttpRequest *request;
    HttpResponse *response;
    int rc;
    int retry;
    int64_t timestamp_ms;

    // attach the caller's custom data pointer
    void *data;
};

class HttpClient {
public:
    void *prepare_curl(HttpRequest &request, HttpResponse *response);
    void *prepare_curl(void *curl_handle, HttpRequest &request, HttpResponse *response);
    int execute(HttpRequest &request, HttpResponse *response);
    void set_options(ClientOptions* options);
private:
    static size_t write_stream(void *ptr, size_t size, size_t nmemb, void *stream);
    static size_t read_stream(char *ptr, size_t size, size_t nmemb, void *stream);
    static size_t recv_header_line(void* ptr, size_t size, size_t nmemb, void* user_data);

private:
    ClientOptions* _options;
};

typedef void (*DetachContextCallback)(HttpRequestContext *);
typedef std::map<void *, HttpRequestContext *> HandleContextMap;

class HttpReactor {
public:
    HttpReactor();
    ~HttpReactor();

    int execute(HttpRequestContext *ctx);

    HttpRequestContext *perform();
    void wait();

    void set_detach_context_callback(DetachContextCallback cb) {
        _detach_context_callback = cb;
    }
    void set_options(ClientOptions* options);
private:
    HttpClient _client;
    void *_mcurl;
    HandleContextMap _handle_ctx;
    std::deque<void *> _idle_handles;
    DetachContextCallback _detach_context_callback;
};

END_CPPSDK_NAMESPACE
#endif

