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
#include <sstream>
#include "bcesdk/bos/request/complete_multipart_upload_request.h"
#include "bcesdk/util/util.h"
#include <json/json.h>

BEGIN_CPPSDK_NAMESPACE

int CompleteMultipartUploadRequest::build_command_specific(HttpRequest *request)
{
    request->set_method(HTTP_METHOD_POST);
    request->add_parameter("uploadId", _upload_id);
    append_user_meta_to(request);

    Json::Value value;
    std::vector<part_t>::iterator it;
    for (it = _parts.begin(); it != _parts.end(); it++) {
        Json::Value tmp;
        tmp["partNumber"] = Json::Value(it->part_number);
        tmp["eTag"] = Json::Value(it->etag);
        value["parts"].append(tmp);
    }
    Json::FastWriter fast_writer;
    _content = new std::string("");
    if (!value.isNull()) {
        _content->assign(fast_writer.write(value));
    }

    if (!_content->empty()) { // erase the end of \n for mock
        _content->erase(_content->end() - 1, _content->end());
    }

    _content_stream = new MemoryInputStream(_content->data(), _content->size());
    LOGF(DEBUG, "request body:%s", _content->c_str());

    request->set_input_stream(_content_stream);
    return 0;
}

END_CPPSDK_NAMESPACE
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
