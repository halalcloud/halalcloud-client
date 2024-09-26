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
#include "bcesdk/bos/response/get_object_response.h"

BEGIN_CPPSDK_NAMESPACE

GetObjectResponse::GetObjectResponse(std::string *buffer)
{
    _owner_of_out = true;
    _out = new StringOutputStream(buffer);
    set_output_stream(_out);
}

GetObjectResponse::GetObjectResponse(OutputStream *object_stream, bool is_owner)
{
    _owner_of_out = is_owner;
    _out = object_stream;
    set_output_stream(_out);
}

GetObjectResponse::~GetObjectResponse()
{
    if (_owner_of_out && _out) {
        delete _out;
        _out = NULL;
    }
}

END_CPPSDK_NAMESPACE

