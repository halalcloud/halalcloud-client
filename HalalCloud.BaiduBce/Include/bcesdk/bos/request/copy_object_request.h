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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_COPY_OBJECT_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_COPY_OBJECT_REQUEST_H

#include "bcesdk/bos/model/object_meta_request.h"
#include "bcesdk/bos/model/copy_request.h"

BEGIN_CPPSDK_NAMESPACE

class CopyObjectRequest : public ObjectMetaRequest, public CopyRequest {
public:
    CopyObjectRequest(const std::string &bucket_name, const std::string &object_name,
            const std::string &source_bucket_name, const std::string &source_object_name)
            : ObjectMetaRequest(bucket_name, object_name),
            CopyRequest(source_bucket_name, source_object_name) {
    }

    virtual ~CopyObjectRequest() {}

    void set_directive(const std::string &directive) {
        _directive = directive;
    }

    int build_command_specific(HttpRequest *request) {
        request->set_method(HTTP_METHOD_PUT);
        if (!_directive.empty()) {
            request->append_header("x-bce-metadata-directive", _directive);
        }
        CopyRequest::fill_http_request(request);
        append_all_meta_to(request);
        return 0;
    }
private:
    std::string _directive;
};

END_CPPSDK_NAMESPACE
#endif

