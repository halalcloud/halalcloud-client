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
#ifndef BAIDU_BOS_CPPSDK_BOS_MODEL_OBJECT_META_REQUEST_H
#define BAIDU_BOS_CPPSDK_BOS_MODEL_OBJECT_META_REQUEST_H

#include "bcesdk/bos/model/object_request.h"
#include "bcesdk/bos/model/object_meta_data.h"

BEGIN_CPPSDK_NAMESPACE

class ObjectMetaRequest : public ObjectRequest {
public:
    ObjectMetaRequest() : _meta(NULL), _is_meta_own(false) {}
    ObjectMetaRequest(const std::string &bucket_name, const std::string &object_name)
        : ObjectRequest(bucket_name, object_name), _meta(NULL), _is_meta_own(false) {
    }
    virtual ~ObjectMetaRequest() {
        if (_meta != NULL && _is_meta_own) {
            delete _meta;
        }
    }

    bool has_meta() {
        return _meta != NULL;
    }

    void set_meta(ObjectMetaData *meta, bool is_own) {
        if (_is_meta_own && _meta != NULL) {
            delete _meta;
        }
        _meta = meta;
        _is_meta_own = is_own;
    }
    void set_meta(ObjectMetaData *meta) {
        set_meta(meta, false);
    }
    void set_allocated_meta(ObjectMetaData *meta) {
        set_meta(meta, true);
    }
    ObjectMetaData *mutable_meta() {
        if (_meta == NULL) {
            _meta = new ObjectMetaData();
            _is_meta_own = true;
        }
        return _meta;
    }

    const ObjectMetaData &meta() const {
        if (_meta != NULL) {
            return *_meta;
        }
        static const ObjectMetaData empty_meta;
        return empty_meta;
    }

    void append_basic_meta_to(HttpRequest *request);

    void append_user_meta_to(HttpRequest *request) {
        if (_meta == NULL) {
            return;
        }
        const StringMap &user_meta = _meta->user_meta();
        for (StringMap::const_iterator it = user_meta.begin(); it != user_meta.end(); ++it) {
            request->append_header("x-bce-meta-" + it->first, it->second);
        }
    }

    void append_all_meta_to(HttpRequest *request) {
        append_basic_meta_to(request);
        append_user_meta_to(request);
    }

    int build_command_specific(HttpRequest *request) {
        append_all_meta_to(request);
        return 0;
    }

private:
    ObjectMetaData *_meta;
    bool _is_meta_own;
};

END_CPPSDK_NAMESPACE
#endif

