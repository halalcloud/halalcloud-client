/***************************************************************************
 *
 * Copyright (c) 2016 Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file src/bos/model/object_meta_data.h
 * @author lili19(com@baidu.com)
 * @date 2016/11/06 20:50:25
 * @brief
 *
 **/
#ifndef BAIDU_BOS_CPPSDK_SRC_BOS_MODEL_OBJECT_META_DATA_H
#define BAIDU_BOS_CPPSDK_SRC_BOS_MODEL_OBJECT_META_DATA_H

#include "bcesdk/common/common.h"
#include <string>
#include <map>
#include <time.h>
#include <stdlib.h>

BEGIN_CPPSDK_NAMESPACE

struct RestoreStatus {
	bool ongoing;
	std::string expiry_date;
};

struct TagInfo {
    TagInfo() {}
    TagInfo(const std::string& k, 
        const std::string& v) : key(k), value(v) {
    } 
	std::string key;
	std::string value;
};

class ObjectMetaData {
public:
    template<typename T>
    static inline const T &get(T* const member) {
        if (member == NULL) {
            static const T default_val;
            return default_val;
        }
        return *member;
    }
    template<typename T>
    static inline void set(T *&member, const T &val) {
        if (member == NULL) {
            member = new T();
        }
        *member = val;
    }
    template<typename T>
    static inline void release(T *&member) {
        if (member != NULL) {
            delete member;
            member = NULL;
        }
    }
    template<typename T>
    static inline void copy(T *&member, const T *val) {
        release(member);
        if (val != NULL) {
            member = new T(*val);
        } else {
            member = NULL;
        }
    }
    template<typename T>
    static inline void move(T *&member, T *&val) {
        release(member);
        member = val;
        val = NULL;
    }
public:
    ObjectMetaData() : _content_length(-1), _instance_length(-1), _last_modified(0),
        _content_md5(NULL), _content_sha256(NULL), _content_range(NULL), _etag(NULL),
        _content_type(NULL), _content_disposition(NULL), _content_encoding(NULL),
        _cache_control(NULL), _expires(0), _storage_class(NULL), _next_append_offset(0), _object_type(NULL), _restore_rep(NULL){
        _restore.ongoing = false;
        _restore.expiry_date = "";
    }
    ObjectMetaData(const ObjectMetaData &meta) : _content_length(-1), _instance_length(-1), _last_modified(0),
        _content_md5(NULL), _content_sha256(NULL), _content_range(NULL), _etag(NULL),
        _content_type(NULL), _content_disposition(NULL), _content_encoding(NULL),
        _cache_control(NULL), _expires(0), _storage_class(NULL), _next_append_offset(0), _object_type(NULL), _restore_rep(NULL) {
        _restore.ongoing = false;
        _restore.expiry_date = "";
        copy_from(meta);
    }
    ~ObjectMetaData() {
        clear();
    }
    RestoreStatus restore_status() const {
        return _restore;
    }
    // get only
    long long content_length() const {
        return _content_length;
    }
    const std::string &content_md5() const {
        return get(_content_md5);
    }
    const std::string &bce_content_sha256() const {
        return get(_content_sha256);
    }
    const std::string &etag() const {
        return get(_etag);
    }
    inline bool has_etag() const {
        return _etag != NULL;
    }
    long long instance_length() const {
        return _instance_length;
    }
    time_t last_modified() const {
        return _last_modified;
    }
    const std::string &content_range() const {
        return get(_content_range);
    }
    inline bool has_content_range() const {
        return _content_range != NULL;
    }
    // can set
    void set_content_type(const std::string &content_type) {
        set(_content_type, content_type);
    }
    const std::string &content_type() const {
        return get(_content_type);
    }
    inline bool has_content_type() const {
        return _content_type != NULL;
    }

    void set_content_disposition(const std::string &disposition) {
        set(_content_disposition, disposition);
    }
    const std::string &content_disposition() const {
        return get(_content_disposition);
    }
    inline bool has_content_disposition() const {
        return _content_disposition != NULL;
    }

    void set_content_encoding(const std::string &encoding) {
        set(_content_encoding, encoding);
    }
    const std::string &content_encoding() const {
        return get(_content_encoding);
    }
    inline bool has_content_encoding() const {
        return _content_encoding != NULL;
    }

    void set_cache_control(const std::string &cache_control) {
        set(_cache_control, cache_control);
    }
    const std::string &cache_control() const {
        return get(_cache_control);
    }

    const std::string &object_type() const {
        return get(_object_type);
    }
    inline bool has_cache_control() const {
        return _cache_control != NULL;
    }

    void set_expires(time_t expires) {
        _expires = expires;
    }
    time_t expires() const {
        return _expires;
    }
    inline bool has_expires() const {
        return _expires > 0;
    }

    void set_storage_class(const std::string &storage_class) {
        set(_storage_class, storage_class);
    }
    const std::string &storage_class() const {
        const static std::string default_sc = "STANDARD";
        if (_storage_class == NULL) {
            return default_sc;
        }
        return *_storage_class;
    }
    inline bool has_storage_class() const {
        return _storage_class != NULL;
    }
    inline bool has_object_type() const {
        return _object_type != NULL;
    }

    void set_user_meta(const std::string &key, const std::string &val) {
        _user_meta[key] = val;
    }
    void set_user_meta(const std::string &key, int val);

    const std::string &user_meta(const std::string& name) const {
        StringMap::const_iterator it = _user_meta.find(name);
        if (it == _user_meta.end()) {
            static const std::string empty_str;
            return empty_str;
        }
        return it->second;
    }
    const StringMap &user_meta() const {
        return _user_meta;
    }
    StringMap *mutable_user_meta() {
        return &_user_meta;
    }

    unsigned long long next_append_offset() const {
        return _next_append_offset;
    }

//utils
    void set_by_header(const StringMap &header);

    void clear() {
        _content_length = -1;
        _instance_length = -1;
        _last_modified = 0;
        _expires = 0;
        _next_append_offset = 0;
        release(_content_md5);
        release(_content_sha256);
        release(_etag);
        release(_content_range);
        release(_content_type);
        release(_content_disposition);
        release(_content_encoding);
        release(_cache_control);
        release(_storage_class);
        release(_object_type);
    }

    void copy_from(const ObjectMetaData &meta) {
        _content_length = meta._content_length;
        _instance_length = meta._instance_length;
        _last_modified = meta._last_modified;
        _expires = meta._expires;
        _next_append_offset = meta._next_append_offset;
        copy(_content_md5, meta._content_md5);
        copy(_content_sha256, meta._content_sha256);
        copy(_etag, meta._etag);
        copy(_content_range, meta._content_range);
        copy(_content_type, meta._content_type);
        copy(_content_disposition, meta._content_disposition);
        copy(_content_encoding, meta._content_encoding);
        copy(_cache_control, meta._cache_control);
        copy(_storage_class, meta._storage_class);
        copy(_object_type, meta._object_type);
        _user_meta = meta._user_meta;
    }

    void move_from(ObjectMetaData &meta) {
        _content_length = meta._content_length;
        _instance_length = meta._instance_length;
        _last_modified = meta._last_modified;
        _expires = meta._expires;
        _next_append_offset = meta._next_append_offset;
        move(_content_md5, meta._content_md5);
        move(_content_sha256, meta._content_sha256);
        move(_etag, meta._etag);
        move(_content_range, meta._content_range);
        move(_content_type, meta._content_type);
        move(_content_disposition, meta._content_disposition);
        move(_content_encoding, meta._content_encoding);
        move(_cache_control, meta._cache_control);
        move(_storage_class, meta._storage_class);
        move(_object_type, meta._object_type);
        _user_meta.swap(meta._user_meta);
        meta._user_meta.clear();
    }

    ObjectMetaData &operator =(const ObjectMetaData &meta) {
        copy_from(meta);
        return *this;
    }

private:
    long long _content_length;
    long long _instance_length;
    time_t _last_modified;
    std::string *_content_md5;
    std::string *_content_sha256;
    std::string *_content_range;
    std::string *_etag;

    std::string *_content_type;
    std::string *_content_disposition;
    std::string *_content_encoding;
    std::string *_cache_control;
    time_t _expires;
    std::string *_storage_class;
    StringMap _user_meta;

    unsigned long long _next_append_offset;
    std::string *_object_type;
    std::string *_restore_rep;
    RestoreStatus _restore;
};

END_CPPSDK_NAMESPACE

#endif

