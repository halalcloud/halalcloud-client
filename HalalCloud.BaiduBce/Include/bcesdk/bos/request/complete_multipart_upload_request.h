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
#ifndef BAIDU_BOS_CPPSDK_BOS_REQUEST_COMPLETE_MULTIPART_UPLOAD_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_REQUEST_COMPLETE_MULTIPART_UPLOAD_REQUEST_H

#include <vector>

#include "bcesdk/bos/model/object_meta_request.h"
#include "bcesdk/common/memory_stream.h"

BEGIN_CPPSDK_NAMESPACE

struct part_t {
    int part_number;
    std::string etag;
};

class CompleteMultipartUploadRequest : public ObjectMetaRequest {
public:
    CompleteMultipartUploadRequest(const std::string &bucket_name, const std::string &object_name,
            const std::string &upload_id) : ObjectMetaRequest(bucket_name, object_name),
            _upload_id(upload_id) {
        _content_stream = NULL;
        _content = NULL;
    }

    virtual ~CompleteMultipartUploadRequest()
    {
        if (_content_stream) {
            delete _content_stream;
            _content_stream = NULL;
        }

        if (_content) {
            delete _content;
            _content = NULL;
        }
    }

    const std::string &upload_id() const {
        return _upload_id;
    }

    int add_part(int number, const std::string &etag)
    {
        if (number < 1 || number > 10000) {
            return RET_ILLEGAL_ARGUMENT;
        }
        part_t part;
        part.part_number = number;
        part.etag = etag;
        _parts.push_back(part);
        return RET_OK;
    }

    int build_command_specific(HttpRequest *request);

private:
    std::string _upload_id;
    std::vector<part_t> _parts;
    std::string        *_content;
    InputStream        *_content_stream;
};

END_CPPSDK_NAMESPACE
#endif

