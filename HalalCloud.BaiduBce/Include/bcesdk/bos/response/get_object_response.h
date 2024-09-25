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
#ifndef BAIDU_BOS_CPPSDK_BOS_RESPONSE_GET_OBJECT_RESPONSE_H
#define BAIDU_BOS_CPPSDK_BOS_RESPONSE_GET_OBJECT_RESPONSE_H

#include <string>
#include <sstream>
#include <map>

#include "bcesdk/model/bce_response.h"
#include "bcesdk/bos/model/object_meta_data.h"

BEGIN_CPPSDK_NAMESPACE

class GetObjectResponse : public BceResponse {
public:
    GetObjectResponse(std::string *buffer);

    GetObjectResponse(OutputStream *object_stream, bool is_owner = false);

    virtual ~GetObjectResponse();

    const ObjectMetaData &meta() const { return _meta; }
    ObjectMetaData &meta() { return _meta; }

    virtual void do_handle_chain(HttpResponse &response) {
        _meta.set_by_header(response.header());
    }

    OutputStream *out() { return _out; };
private:
    OutputStream *_out;
    ObjectMetaData _meta;
    bool          _owner_of_out;
};

END_CPPSDK_NAMESPACE
#endif

