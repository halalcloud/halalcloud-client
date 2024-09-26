/***************************************************************************
 * 
 * Copyright (c) 2016 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file src/bos/model/object_meta_data.cpp
 * @author lili19(com@baidu.com)
 * @date 2016/11/18 17:14:22
 * @brief 
 *  
 **/
#include "bcesdk/bos/model/object_meta_data.h"
#include "bcesdk/util/util.h"

BEGIN_CPPSDK_NAMESPACE

static inline void set_by_map(std::string *&target, const StringMap &m, const char *key) {
    StringMap::const_iterator it = m.find(key);
    if (it == m.end()) {
        return;
    }
    ObjectMetaData::set(target, it->second);
}

static inline void set_by_map(long long &target, const StringMap &m, const char *key) {
    StringMap::const_iterator it = m.find(key);
    if (it == m.end()) {
        return;
    }
    target = strtoll(it->second.c_str(), NULL, 10);
}

static inline void set_rfc822_by_map(time_t &target, const StringMap &m, const char *key) {
    StringMap::const_iterator it = m.find(key);
    if (it == m.end()) {
        return;
    }
    target = DateUtil::parse_rfc822_date(it->second);
}


void ObjectMetaData::set_user_meta(const std::string &key, int val) {
    _user_meta[key] = StringUtil::to_string(val);
}

void ObjectMetaData::set_by_header(const StringMap &header) {
    set_by_map(_content_length, header, "content-length");
    set_rfc822_by_map(_last_modified, header, "last-modified");
    set_by_map(_content_md5, header, "content-md5");
    set_by_map(_content_sha256, header, "x-bce-content-sha256");
    set_by_map(_content_range, header, "content-range");
    set_by_map(_restore_rep, header, "x-bce-restore");

    // parse restore status
    if (_restore_rep != NULL) {
        _restore.ongoing = _restore_rep->find("true") != std::string::npos ? true:false;  
        size_t pos_e  = _restore_rep->find_last_of('"');
        size_t pos_s  = _restore_rep->find_last_of('"', pos_e - 1);
        _restore.expiry_date = _restore_rep->substr(pos_s+1, pos_e-pos_s-1);
    }
    
    if (has_content_range()) {
        size_t pos = _content_range->find('/');
        if (pos != std::string::npos) {
            _instance_length = strtoll(_content_range->substr(pos + 1).c_str(), NULL, 10);
        }
    }
    set_by_map(_etag, header, "etag");
    if (has_etag() && _etag->length() >= 2 && *_etag->begin() == '"' && *_etag->rbegin() == '"') {
        *_etag = _etag->substr(1, _etag->length() - 2);
    }
    set_by_map(_content_type, header, "content-type");
    set_by_map(_content_disposition, header, "content-disposition");
    set_by_map(_content_encoding, header, "content-encoding");
    set_by_map(_cache_control, header, "cache-control");
    set_rfc822_by_map(_expires, header, "expires");
    set_by_map(_storage_class, header, "x-bce-storage-class");
    set_by_map(_object_type, header, "x-bce-object-type");

    StringMap::const_iterator it = header.find("x-bce-next-append-offset");
    if (it != header.end()) {
        if (!StringUtil::str2uint64(it->second, &_next_append_offset)) {
            _next_append_offset = 0;
        }
    }

    it = header.upper_bound("x-bce-meta-");
    if (it == header.end()) {
        return;
    }
    static const std::string bce_prefix("x-bce-meta-");
    while (it != header.end() && it->first.compare(0, bce_prefix.size(), bce_prefix) == 0) {
        set_user_meta(it->first.substr(bce_prefix.size()), it->second);
        ++it;
    }

    
}

END_CPPSDK_NAMESPACE

