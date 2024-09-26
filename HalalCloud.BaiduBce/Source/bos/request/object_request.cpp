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

#include "bcesdk/bos/model/object_meta_request.h"
#include "bcesdk/util/util.h"

BEGIN_CPPSDK_NAMESPACE

void ObjectMetaRequest::append_basic_meta_to(HttpRequest *request) {
    if (_meta == NULL) {
        return;
    }
    if (_meta->has_content_type()) {
        request->append_header("Content-Type", _meta->content_type());
    }
    if (_meta->has_content_disposition()) {
        request->append_header("Content-Disposition", _meta->content_disposition());
    }
    if (_meta->has_content_encoding()) {
        request->append_header("Content-Encoding", _meta->content_encoding());
    }
    if (_meta->has_cache_control()) {
        request->append_header("Cache-Control", _meta->cache_control());
    }
    if (_meta->has_expires()) {
        request->append_header("Expires", DateUtil::format_rfc822_date(_meta->expires()));
    }
    if (_meta->has_storage_class()) {
        request->append_header("x-bce-storage-class", _meta->storage_class());
    }
}

END_CPPSDK_NAMESPACE

