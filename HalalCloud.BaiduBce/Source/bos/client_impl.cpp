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
#include "bcesdk/bos/client_impl.h"
#include "bcesdk/util/util.h"
#include "bcesdk/http/http_client.h"
#ifdef ENABLE_BNS
#include "naming_service/naming_service.h"
#endif

BEGIN_CPPSDK_NAMESPACE

ClientImpl::ClientImpl(const Credential &credential, const ClientOptions &options)
    : _options(options) {
    DefaultSigner *signer = new DefaultSigner(options);
    signer->set_credential(credential);
    _signer = signer;
    if (_options.user_agent.empty()) {
        _options.user_agent = sdk_package_string();
    }
}

ClientImpl::~ClientImpl()
{
   if (_signer != NULL) {
       delete _signer;
   }
}


std::string ClientImpl::generate_url(BceRequest &request, int expire_seconds) {
    HttpRequest http_request;
    bool is_change_to_cname = false;
#ifdef ENABLE_BNS
    std::string endpoint = NamingService::convert(_options.endpoint);
    if (endpoint.empty()) {
        return "";
    }
    is_change_to_cname = http_request.set_endpoint(endpoint, _options, request.bucket_name());
#else
    is_change_to_cname = http_request.set_endpoint(_options.endpoint, _options, request.bucket_name());
#endif
    http_request.append_header("Host", http_request.host());
    int ret = request.build_http_request(&http_request, is_change_to_cname);
    if (ret != 0) {
        LOG(ERROR) << "generate url failed due to build http request failed, ret:" << ret;
        return "";
    }
    http_request.add_parameter("authorization",
            _signer->generate_auth(&http_request, expire_seconds));
    return http_request.generate_url();
}

int ClientImpl::build_http_request(BceRequest &request, HttpRequest *http_request) {
    bool is_change_to_cname = false;
#ifdef ENABLE_BNS
    std::string endpoint = NamingService::convert(_options.endpoint);
    if (endpoint.empty()) {
        return RET_CLIENT_ERROR;
    }
    is_change_to_cname = http_request->set_endpoint(endpoint, _options, request.bucket_name());
#else
    is_change_to_cname = http_request->set_endpoint(_options.endpoint, _options, request.bucket_name());
#endif
    http_request->set_timeout(_options.timeout);
    http_request->set_connect_timeout_ms(_options.connect_timeout_ms);
    http_request->append_header("Host", http_request->host());
    http_request->append_header("User-Agent", _options.user_agent);
    int ret = request.build_http_request(http_request, is_change_to_cname);
    if (ret != 0) {
        return ret;
    }
    http_request->append_header("x-bce-date", TimeUtil::now_utctime());
    if (_options.auto_append_bns_to_ua && !_options.endpoint.empty()) {
        http_request->append_header("x-bce-sdk-endpoint", _options.endpoint);
    }
    if (!http_request->is_post_object_request()){
        _signer->sign(http_request);
    }
    
    return 0;
}


int ClientImpl::build_http_response(BceResponse *response, HttpResponse* http_response) {
    return response->fill_http_response(http_response);
}

int ClientImpl::send_request(BceRequest &request, BceResponse *response) {
    HttpRequest http_request;
    int ret = build_http_request(request, &http_request);
    if (RET_OK != ret) {
        LOG(ERROR) << "build http request failed, ret:" << ret;
        return ret;
    }
    HttpResponse *http_response = response->mutable_http_response();
    int try_num = 1;
    if (_options.retry > 0) {
        try_num += _options.retry;
    }
    HttpClient client;
    client.set_options(&_options);
    while (try_num-- > 0) {
        HttpRequest http_request;
        ret = build_http_request(request, &http_request);
        build_http_response(response, http_response);
        if (RET_OK != ret) {
            LOG(ERROR) << "build http request failed, ret:" << ret;
            return ret;
        }
        ret = client.execute(http_request, http_response);
        if (ret == 0) {
            response->handle_response(*http_response);
        } else {
            response->set_error(STATUS_RPC_FAIL, "HttpExecuteFailure");
            LOG(WARN) << "http execute error: (" << ret << ')' << stringfy_ret_code(ret);
        }
        if (response->is_ok()) {
            return 0;
        }
        ret = response->status_code();
        if (ret >= 500 && ret != 501) {
            if (try_num > 0) {
                InputStream *istream = http_request.get_input_stream();
                if (istream != NULL) {
                    istream->seek(0);
                }
                http_response->reset();
                LOG(WARN) << "send_request do retry. last ret status:" 
                          << ret << " retry count:" << try_num;
                continue;
            }
        }
        break;
    }
    return ret;
}

int ClientImpl::send_request(int n, BceRequestContext ctx[], int max_parallel) {
    int ret = 0;
    std::vector<HttpRequest> http_requests_buffer(n);
    std::vector<HttpRequestContext> http_ctx(n);
    if (max_parallel <= 0) {
        max_parallel = _options.max_parallel;
        if (max_parallel <= 0) {
            max_parallel = 10;
        }
    }
    max_parallel = n > max_parallel ? max_parallel : n;
    int i = 0;
    for (i = 0; i < n; ++i) {
        http_ctx[i].request = &http_requests_buffer[i];
        http_ctx[i].response = ctx[i].response->mutable_http_response();
        http_ctx[i].retry = _options.retry;
        http_ctx[i].data = &ctx[i];
    }
    HttpReactor reactor;
    reactor.set_options(&_options);
    for (i = 0; i < max_parallel; ++i) {
        ret = build_http_request(*ctx[i].request, http_ctx[i].request);
        build_http_response(ctx[i].response, http_ctx[i].response);
        if (ret != 0) {
            LOG(ERROR) << "build http request failed: " << ret;
            return ret;
        }
        ret = reactor.execute(&http_ctx[i]);
        if (ret != 0) {
            return ret;
        }
    }
    int finish_num = 0;
    while (finish_num < n) {
        HttpRequestContext *hctx = reactor.perform();
        if (hctx == NULL) {
            reactor.wait();
            continue;
        }
        BceRequestContext *bctx = (BceRequestContext *) hctx->data;
        if (hctx->rc == 0) {
            bctx->response->handle_response(*hctx->response);
        } else {
            bctx->response->set_error(STATUS_RPC_FAIL, "HttpExecuteFailure");
        }
        int status = bctx->response->status_code();
        if ((status >= 500 && status != 501) && hctx->retry > 0) {
            hctx->retry--;
            hctx->request->reset();
            hctx->response->reset();
            LOG(WARN) << "send_request do retry. last ret status:" 
                      << status << " retry count:" << hctx->retry;
        } else if (i < n) {
            hctx = &http_ctx[i++];
            bctx = (BceRequestContext *) hctx->data;
            ++finish_num;
        } else {
            ++finish_num;
            continue;
        }
        ret = build_http_request(*bctx->request, hctx->request);
        build_http_response(bctx->response, hctx->response);
        if (ret != 0) {
            LOG(ERROR) << "build http request failed: " << ret;
            return ret;
        }
        ret = reactor.execute(hctx);
        if (ret != 0) {
            return ret;
        }
    }
    return 0;
}

END_CPPSDK_NAMESPACE

