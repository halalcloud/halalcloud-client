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
#include <string>
#include <iostream>
#include <cstdlib>
#include <algorithm>

#include "bcesdk/bos/client.h"
#include "bcesdk/bos/client_impl.h"
#include "bcesdk/util/util.h"

BEGIN_CPPSDK_NAMESPACE

void nothing(off_t, size_t) {}
Client::Client(const std::string &access_key_id, const std::string &access_key_secret) {
    ClientOptions option;
    option.endpoint = "bj.bcebos.com";
    _impl = new ClientImpl(Credential(access_key_id, access_key_secret), option);
}

Client::Client(const std::string &access_key_id, const std::string &access_key_secret,
        const ClientOptions &options)
{
    _impl = new ClientImpl(Credential(access_key_id, access_key_secret), options);
}

Client::Client(const Credential &credential, const ClientOptions &options) {
    _impl = new ClientImpl(credential, options);
}

Client::~Client()
{
    if (_impl) {
        delete _impl;
        _impl = NULL;
    }
}

int Client::send_request(BceRequest &request, BceResponse *response) {
    return _impl->send_request(request, response);
}

int Client::send_request(int n, BceRequestContext ctx[], int max_parallel) {
    return _impl->send_request(n, ctx, max_parallel);
}

int Client::put_object(PutObjectRequest &request, PutObjectResponse *result) const {
    request.set_calc_md5_on(_impl->options().calc_md5_on);
    return _impl->send_request(request, result);
}

int Client::get_object(GetObjectRequest &request, GetObjectResponse *result) const {
    std::string object_name = request.object_name();
    // check object name in case: get -> list
    if (object_name.empty() ||
        std::count(object_name.begin(), object_name.end(), '/') == object_name.length()) {
        return RET_ILLEGAL_ARGUMENT;
    }
    return _impl->send_request(request, result);
}

int Client::delete_object(DeleteObjectRequest &request, DeleteObjectResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::head_object(HeadObjectRequest &request, HeadObjectResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::restore_object(RestoreObjectRequest&request, RestoreObjectResponse*result) const {
    return _impl->send_request(request, result);
}

int Client::copy_object(CopyObjectRequest &request, CopyObjectResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::append_object(AppendObjectRequest &request, AppendObjectResponse *result) const {
    request.set_calc_md5_on(_impl->options().calc_md5_on);
    return _impl->send_request(request, result);
}

int Client::get_object_acl(GetObjectAclRequest &request, GetObjectAclResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::put_object_acl(PutObjectAclRequest &request, PutObjectAclResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::delete_object_acl(DeleteObjectAclRequest &request, DeleteObjectAclResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::put_object_tagging(PutObjectTaggingRequest &request, PutObjectTaggingResponse *result) const {
    return _impl->send_request(request, result);    
}

int Client::get_object_tagging(GetObjectTaggingRequest &request, GetObjectTaggingResponse *result) const {
    return _impl->send_request(request, result);    
}

int Client::init_multipart_upload(InitMultiUploadRequest &request,
        InitMultiUploadResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::upload_part(UploadPartRequest &request, UploadPartResponse *result) const {
    request.set_calc_md5_on(_impl->options().calc_md5_on);
    return _impl->send_request(request, result);
}

int Client::copy_part(CopyPartRequest &request, CopyPartResponse *result) {
    return _impl->send_request(request, result);
}

int Client::abort_multipart_upload(AbortMultipartUploadRequest &request,
        AbortMultipartUploadResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::complete_multipart_upload(CompleteMultipartUploadRequest &request,
        CompleteMultipartUploadResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::list_parts(ListPartsRequest &request, ListPartsResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::list_multipart_uploads(ListMultipartUploadsRequest &request,
        ListMultipartUploadsResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::list_objects(ListObjectsRequest &request, ListObjectsResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::delete_bucket(DeleteBucketRequest &request, DeleteBucketResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::get_bucket_acl(GetBucketAclRequest &request, GetBucketAclResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::get_bucket_location(GetBucketLocationRequest &request,
        GetBucketLocationResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::head_bucket(HeadBucketRequest &request, HeadBucketResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::list_buckets(ListBucketsRequest &request, ListBucketsResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::put_bucket_acl(PutBucketAclRequest &request, PutBucketAclResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::put_bucket(PutBucketRequest &request, PutBucketResponse *result) const {
    return _impl->send_request(request, result);
}

int Client::post_object(PostObjectRequest &request, PostObjectResponse* result) const {
    request.set_access_key(_impl->get_access_key());
    request.set_secret_access_key(_impl->get_secret_key());
    int post_object_result = _impl->send_request(request, result);
    return post_object_result;
}

int Client::put_object(const std::string &bucket_name, const std::string &object_name,
        const std::string &data) const {
    PutObjectRequest request(bucket_name, object_name, data);
    request.set_calc_md5_on(_impl->options().calc_md5_on);
    PutObjectResponse result;
    return _impl->send_request(request, &result);
}

int Client::get_object(const std::string &bucket_name, const std::string &object_name,
        std::string *data) const {
    GetObjectRequest request(bucket_name, object_name);
    GetObjectResponse result(data);
    return _impl->send_request(request, &result);
}

int Client::delete_object(const std::string &bucket_name, const std::string &object_name) const {
    DeleteObjectRequest request(bucket_name, object_name);
    DeleteObjectResponse result;
    return _impl->send_request(request, &result);
}



int Client::copy_object(const std::string &src_bucket, const std::string &src_object,
        const std::string &dst_bucket, const std::string &dst_object,
        const std::string &storage_class, ObjectMetaData *meta) {
    CopyObjectRequest request(dst_bucket, dst_object, src_bucket, src_object);
    if (meta != NULL) {
        request.set_directive("replace");
        request.set_meta(meta);
    } else {
        meta = request.mutable_meta();
    }
    if (!storage_class.empty()) {
        meta->set_storage_class(storage_class);
    }
    CopyObjectResponse response;
    copy_object(request, &response);
    if (response.is_fail()) {
        LOG(WARN) << "copy object(/" << src_bucket << '/' << src_object << "->/" << dst_bucket << '/'
            << dst_object << ") failed, error:(" << response.status_code() << ")"
            << response.error().message();
        if (response.status_code() != 404) {
            return RET_SERVICE_ERROR;
        }
        return RET_KEY_NOT_EXIST;
    }
    return 0;
}

std::string Client::generate_url(const std::string &bucket, const std::string &object,
        int expire_seconds) {
    GetObjectRequest req(bucket, object);
    return _impl->generate_url(req, expire_seconds);
}

int Client::download_file(
        const std::string &bucket_name,
        const std::string &object_name,
        FileOutputStream &file,
        int64_t start,
        int64_t length,
        ObjectMetaData *object_meta_data,
        std::string *etag,
        int64_t *content_length,
        std::string *range)
{
    int ret = RET_OK;
    GetObjectRequest request(bucket_name, object_name);
    GetObjectResponse response(&file);

    if (start != -1 && length != -1) {
        request.set_range(start, start + length - 1);
    } else {
        return RET_ILLEGAL_ARGUMENT;
    }

    ret = _impl->send_request(request, &response);
    if (ret != 0 || response.is_fail()) {
        LOGF(WARN, "download file error client code:%d, server message:%s", ret,
                response.error().message().c_str());
        return ret;
    }
    if (etag != NULL) {
        *etag = response.meta().etag();
    }
    if (content_length != NULL) {
        *content_length = response.meta().content_length();
    }
    if (range != NULL) {
        *range = response.meta().content_range();
    }
    if (object_meta_data != NULL) {
        object_meta_data->move_from(response.meta());
    }
    return RET_OK;
}

int Client::download_file(
        const std::string &bucket_name,
        const std::string &object_name,
        const std::string &file_name,
        int64_t start,
        int64_t length,
        ObjectMetaData *object_meta_data,
        std::string *etag,
        int64_t *content_length,
        std::string *range)
{
    FileOutputStream file(file_name, start);
    if (file.last_error() != 0) {
        LOGF(WARN, "open output file %s failed: %d", file_name.c_str(), file.last_error());
        return file.last_error();
    }
    return download_file(bucket_name, object_name, file, start, length,
            object_meta_data, etag, content_length, range);
}

int Client::download_file(
        const std::string &bucket_name,
        const std::string &object_name,
        fd_t fd,
        int64_t start,
        int64_t length,
        ObjectMetaData *object_meta_data,
        std::string *etag,
        int64_t *content_length,
        std::string *range)
{
    FileOutputStream file(fd, start);
    if (file.last_error() != 0) {
        LOGF(WARN, "open output fd %d failed: %d", fd, file.last_error());
        return file.last_error();
    }
    return download_file(bucket_name, object_name, file, start, length,
            object_meta_data, etag, content_length, range);
}

int Client::upload_file(
        const std::string &bucket_name,
        const std::string &object_name,
        FileInputStream &file,
        ObjectMetaData *meta) {
    PutObjectRequest request(bucket_name, object_name, &file);
    request.set_calc_md5_on(_impl->options().calc_md5_on);
    request.set_meta(meta);
    PutObjectResponse response;
    int ret = _impl->send_request(request, &response);
    if (ret != RET_OK) {
        LOG(WARN) << "upload file failed due to client side error, ret:" << ret;
        return ret;
    }
    if (response.is_fail()) {
        LOG(WARN) << "upload file failed due to server response error, code:"
            << response.error().code() << " msg:" << response.error().message();
        return RET_SERVICE_ERROR;
    }
    return RET_OK;
}

int Client::upload_file(
        const std::string &bucket_name,
        const std::string &object_name,
        const std::string &file_name,
        ObjectMetaData *meta)
{
    FileInputStream file(file_name);
    if (file.last_error() != 0) {
        LOGF(WARN, "open input file %s failed: %d", file_name.c_str(), file.last_error());
        return file.last_error();
    }
    return upload_file(bucket_name, object_name, file, meta);
}

int Client::upload_file(const std::string &bucket_name, const std::string &object_name, fd_t fd,
        ObjectMetaData *meta) {
    FileInputStream file(fd);
    if (file.last_error() != 0) {
        LOGF(WARN, "open input file fd:%d failed: %d", fd, file.last_error());
        return file.last_error();
    }
    return upload_file(bucket_name, object_name, file, meta);
}

int Client::upload_super_file(
        const std::string &bucket_name,
        const std::string &object_name,
        fd_t fd,
        ObjectMetaData *meta) {
    FileInputStream file(fd);
    if (file.last_error() != 0) {
        LOGF(WARN, "Multiupload : open fd %d failed: %d", fd, file.last_error());
        return RET_CLIENT_ERROR;
    }
    return parallel_upload(bucket_name, object_name, file, meta);
}

int Client::upload_super_file(
        const std::string &bucket_name,
        const std::string &object_name,
        const std::string &file_name,
        ObjectMetaData *meta)
{
    FileInputStream file(file_name);
    if (file.last_error() != 0) {
        LOGF(WARN, "Multiupload : open file %s failed: %d", file_name.c_str(), file.last_error());
        return RET_CLIENT_ERROR;
    }
    return parallel_upload(bucket_name, object_name, file, meta);
}

int Client::parallel_download(const std::string &bucket, const std::string &object,
    fd_t fd, int64_t start, int64_t size, ObjectMetaData *meta,
    progress_cb_t success_cb, progress_cb_t fail_cb) {
    FileOutputStream file(fd, start);
    if (file.last_error() != 0) {
        LOGF(WARN, "Multiupload : open fd %d failed: %d", fd, file.last_error());
        return RET_CLIENT_ERROR;
    }
    return parallel_download(bucket, object, file, start, size,
                meta, success_cb, fail_cb);
}

int Client::parallel_download(const std::string &bucket, const std::string &object,
        FileOutputStream &file, int64_t start, int64_t size, ObjectMetaData *meta,
        progress_cb_t success_cb, progress_cb_t fail_cb) {
    LOG(DEBUG) << "parallel_download, fd: " << file.fd() <<", start: " << start << ", size: " << size;
    if (size < 0) {
        HeadObjectRequest request(bucket, object);
        HeadObjectResponse response;
        _impl->send_request(request, &response);
        if (response.is_fail()) {
            LOG(WARN) << "get size of bos:/" << bucket << '/' << object << " failed, error:("
                << response.status_code() << ")" << response.error().message();
            return RET_SERVICE_ERROR;
        }
        size = response.meta().content_length();
    }
    if (size < 10 * 1024 * 1024) {
        return download_file(bucket, object, file, start, size, meta);
    }

    long long blksize = _impl->options().multi_part_size;
    int n = int((size + blksize - 1) / blksize);
    long long off = start;
    std::vector<BceRequestContext> ctx(n);
    for (int i = 0; i < n; ++i) {
        long long len = blksize;
        if (i == n - 1) {
            len = size - (off - start);
        }
        GetObjectRequest *req = new GetObjectRequest(bucket, object);
        req->set_range(off, off + len - 1);
        ctx[i].request = req;
        ctx[i].response = new GetObjectResponse(new FileOutputStream(file.fd(), off), true);
        ctx[i].is_own = true;
        off += len;
    }
    int ret = _impl->send_request((int)ctx.size(), &ctx.front());
    if (ret != 0) {
        return ret;
    }
    for (size_t i = 0; i < ctx.size(); ++i) {
        if (ctx[i].response->is_fail()) {
            GetObjectRequest *req = (GetObjectRequest *) ctx[i].request;
            LOG(ERROR) << "parallel download bos:/" << bucket << '/' << object << ":["
                << req->range_start() << ',' << req->range_end() << "] failed, error:("
                << ctx[i].response->status_code() << ")" << ctx[i].response->error().message();
            // info which range failed
            if (fail_cb != nullptr) {
                fail_cb(req->range_start(),
                        req->range_end()-req->range_start());
            }
            ret = RET_SERVICE_ERROR;
        } else if (success_cb != nullptr){
            // parse range
            GetObjectResponse *res = (GetObjectResponse *) ctx[i].response;
            std::string range = res->meta().content_range();
            off_t start = 0;
            size_t size  = 0;
            std::string buf;
            for (size_t i = 0; i < range.size(); ++i) {
                if (range[i] != '-' && range[i] != '/'
                            &&range[i] >= '0' && range[i] <= '9') {
                    buf.push_back(range[i]);
                } else if (range[i] == '-') {
                    start = atoll(buf.c_str());
                    buf.clear();
                } else if (range[i] == '/') {
                    size = atoll(buf.c_str()) - start + 1;
                    break;
                }
            }
            // call call_back to update progress
            success_cb(start, size);
        }
    }
    if (ret == 0 && ctx.size() > 0 && meta != NULL) {
        meta->copy_from(((GetObjectResponse *) ctx[0].response)->meta());
    }
    return ret;
}

int Client::parallel_upload(const std::string &bucket, const std::string &object,
        FileInputStream &file, ObjectMetaData *meta) {
    MultipleUploadHelper helper(this);
    int ret = helper.init(bucket, object, meta);
    if (ret != 0) {
        return ret;
    }
    const std::string &upload_id = helper.upload_id();
    int64_t size = file.get_size();
    int64_t part_size = MultipleUploadHelper::calc_part_size(size, _impl->options().multi_part_size);
    int part_num = int((size + part_size - 1) / part_size);
    LOG(INFO) << "upload " << upload_id << ", size:" << size << " part_size:" << part_size
        << " part_num:" << part_num;

    std::vector<BceRequestContext> ctx(part_num);
    int64_t off = 0;
    for (int i = 0; off < size; ++i) {
        if (off + part_size > size) {
            part_size = size - off;
        }
        FileInputStream *f = new FileInputStream(file.fd(), off, part_size);
        UploadPartRequest *req = new UploadPartRequest(bucket, object, i + 1, upload_id);
        req->set_calc_md5_on(_impl->options().calc_md5_on);
        req->set_allocated_stream(f);
        ctx[i].request = req;
        ctx[i].response = new UploadPartResponse();
        ctx[i].is_own = true;

        LOG(DEBUG) << "upload " << upload_id << ", add new part: " << i + 1 << " (" << off << ','
                << part_size << ')';
        off += part_size;
    }
    ret = send_request((int) ctx.size(), &ctx.front());
    if (ret != 0) {
        return ret;
    }
    for (size_t i = 0; i < ctx.size(); ++i) {
        UploadPartResponse *res = (UploadPartResponse *) ctx[i].response;
        if (res->is_fail()) {
            LOG(WARN) << "upload " << upload_id << ", part " << i + 1 << " failed, error:("
                << res->status_code() << ")" << res->error().message();
            ret = RET_SERVICE_ERROR;
            continue;
        }
        helper.add_part((int)i + 1, res->etag());
    }
    if (ret != 0) {
        return ret;
    }
    ret = helper.complete();
    if (ret != 0) {
        return ret;
    }
    return 0;
}

int Client::parallel_copy(const std::string &src_bucket, const std::string &src_object,
        const std::string &dst_bucket, const std::string &dst_object,
        const std::string &storage_class) {
    HeadObjectRequest request(src_bucket, src_object);
    HeadObjectResponse response;
    int ret = _impl->send_request(request, &response);
    if (ret != 0 || response.is_fail()) {
        LOG(WARN) << "get meta of bos:/" << src_bucket << '/' << src_object << " failed, error: "
            << response.error().message();
        return RET_SERVICE_ERROR;
    }
    ObjectMetaData &meta = response.meta();
    if (!storage_class.empty()) {
        meta.set_storage_class(storage_class);
    }
    int64_t size = meta.content_length();
    int64_t part_size = MultipleUploadHelper::calc_part_size(size, _impl->options().multi_part_size);
    if (size < part_size) {
        return copy_object(src_bucket, src_object, dst_bucket, dst_object, storage_class);
    }
    int part_num = int((size + part_size - 1) / part_size);

    MultipleUploadHelper helper(this);
    ret = helper.init(dst_bucket, dst_object, &meta);
    if (ret != 0) {
        return ret;
    }
    const std::string &upload_id = helper.upload_id();
    LOG(INFO) << "upload " << upload_id << ", size:" << size << " part_size:" << part_size
        << " part_num:" << part_num;
    std::vector<BceRequestContext> ctx(part_num);
    int64_t off = 0;
    for (int i = 0; off < size; ++i) {
        if (off + part_size > size) {
            part_size = size - off;
        }
        CopyPartRequest *req = new CopyPartRequest();
        req->set_bucket_name(dst_bucket);
        req->set_object_name(dst_object);
        req->set_upload_id(upload_id);
        req->set_part_number(i + 1);
        req->set_source_bucket_name(src_bucket);
        req->set_source_object_name(src_object);
        req->set_range(off, off + part_size - 1);
        req->set_if_match(meta.etag());

        ctx[i].request = req;
        ctx[i].response = new CopyPartResponse();
        ctx[i].is_own = true;

        LOG(DEBUG) << "upload " << upload_id << ", add new part: " << i + 1 << " (" << off << ','
                << part_size << ')';
        off += part_size;
    }
    ret = _impl->send_request((int) ctx.size(), &ctx.front());
    if (ret != 0) {
        return ret;
    }
    for (size_t i = 0; i < ctx.size(); ++i) {
        CopyPartResponse *res = (CopyPartResponse *) ctx[i].response;
        if (res->is_fail()) {
            LOG(WARN) << "copy " << upload_id << ", part " << i + 1 << " failed, error:("
                << res->status_code() << ")" << res->error().message();
            ret = RET_SERVICE_ERROR;
            continue;
        }
        helper.add_part((int)i + 1, res->etag());
    }
    if (ret != 0) {
        return ret;
    }
    ret = helper.complete();
    if (ret != 0) {
        return ret;
    }
    return 0;
}

MultipleUploadHelper::~MultipleUploadHelper() {
    if (_complete_request != NULL) {
        AbortMultipartUploadRequest abort_req(_complete_request->bucket_name(),
                _complete_request->object_name(), _complete_request->upload_id());
        AbortMultipartUploadResponse abort_res;
        _client->abort_multipart_upload(abort_req, &abort_res);
        delete _complete_request;
    }
}

int MultipleUploadHelper::init(const std::string &bucket, const std::string &object,
        ObjectMetaData *meta) {
    _init_request.set_bucket_name(bucket);
    _init_request.set_object_name(object);
    if (meta != NULL) {
        _init_request.set_meta(meta);
    }
    return init();
}

int MultipleUploadHelper::init() {
    InitMultiUploadResponse res;
    _client->init_multipart_upload(_init_request, &res);
    if (res.is_fail()) {
        LOG(WARN) << "init multi upload server error:(" << res.status_code() << ")"
            << res.error().message();
        return RET_SERVICE_ERROR;
    }
    _complete_request = new CompleteMultipartUploadRequest(_init_request.bucket_name(),
            _init_request.object_name(), res.upload_id());
    if (_init_request.has_meta()) {
        _complete_request->set_meta(_init_request.mutable_meta());
    }
    return 0;
}

int MultipleUploadHelper::complete() {
    if (_complete_request == NULL) {
        return RET_CLIENT_ERROR;
    }
    CompleteMultipartUploadResponse res;
    _client->complete_multipart_upload(*_complete_request, &res);
    const std::string &upload_id = _complete_request->upload_id();
    if (res.is_fail()) {
        LOG(WARN) << "complete multi upload " << upload_id << " failed, error:("
            << res.status_code() << ")" << res.error().message();
        return RET_SERVICE_ERROR;
    }
    delete _complete_request;
    _complete_request = NULL;
    return 0;
}

END_CPPSDK_NAMESPACE

