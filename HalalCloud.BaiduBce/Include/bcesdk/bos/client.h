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
#ifndef BAIDU_BOS_CPPSDK_BOS_CLIENT_H
#define BAIDU_BOS_CPPSDK_BOS_CLIENT_H

#include <stdint.h>

#include <string>
#include <map>
#include <functional>

#include "bcesdk/common/common.h"

#ifndef _WIN32
#include "bcesdk/common/file_stream.h"
#else
#include "bcesdk/common/win_file_stream.h"
#endif

#include "bcesdk/bos/client_options.h"

#include "bcesdk/bos/request/abort_multipart_upload_request.h"
#include "bcesdk/bos/request/complete_multipart_upload_request.h"
#include "bcesdk/bos/request/copy_object_request.h"
#include "bcesdk/bos/request/delete_object_request.h"
#include "bcesdk/bos/request/get_object_request.h"
#include "bcesdk/bos/request/head_object_request.h"
#include "bcesdk/bos/request/init_multi_upload_request.h"
#include "bcesdk/bos/request/list_multipart_uploads_request.h"
#include "bcesdk/bos/request/list_objects_request.h"
#include "bcesdk/bos/request/list_parts_request.h"
#include "bcesdk/bos/request/put_object_request.h"
#include "bcesdk/bos/request/append_object_request.h"
#include "bcesdk/bos/request/upload_part_request.h"
#include "bcesdk/bos/request/copy_part_request.h"
#include "bcesdk/bos/request/put_object_acl_request.h"
#include "bcesdk/bos/request/get_object_acl_request.h"
#include "bcesdk/bos/request/delete_object_acl_request.h"
#include "bcesdk/bos/request/post_object_request.h"

#include "bcesdk/bos/response/abort_multipart_upload_response.h"
#include "bcesdk/bos/response/complete_multipart_upload_response.h"
#include "bcesdk/bos/response/copy_object_response.h"
#include "bcesdk/bos/response/delete_object_response.h"
#include "bcesdk/bos/response/get_object_response.h"
#include "bcesdk/bos/response/head_object_response.h"
#include "bcesdk/bos/response/init_multi_upload_response.h"
#include "bcesdk/bos/response/list_multipart_uploads_response.h"
#include "bcesdk/bos/response/list_objects_response.h"
#include "bcesdk/bos/response/list_parts_response.h"
#include "bcesdk/bos/response/put_object_response.h"
#include "bcesdk/bos/response/append_object_response.h"
#include "bcesdk/bos/response/upload_part_response.h"
#include "bcesdk/bos/response/copy_part_response.h"
#include "bcesdk/bos/response/put_object_acl_response.h"
#include "bcesdk/bos/response/get_object_acl_response.h"
#include "bcesdk/bos/response/delete_object_acl_response.h"
#include "bcesdk/bos/response/post_object_response.h"

#include "bcesdk/bos/request/delete_bucket_request.h"
#include "bcesdk/bos/request/get_bucket_acl_request.h"
#include "bcesdk/bos/request/get_bucket_location_request.h"
#include "bcesdk/bos/request/head_bucket_request.h"
#include "bcesdk/bos/request/list_buckets_request.h"
#include "bcesdk/bos/request/put_bucket_acl_request.h"
#include "bcesdk/bos/request/put_bucket_request.h"
#include "bcesdk/bos/request/restore_object_request.h"

#include "bcesdk/bos/response/delete_bucket_response.h"
#include "bcesdk/bos/response/get_bucket_acl_response.h"
#include "bcesdk/bos/response/get_bucket_location_response.h"
#include "bcesdk/bos/response/head_bucket_response.h"
#include "bcesdk/bos/response/list_buckets_response.h"
#include "bcesdk/bos/response/put_bucket_acl_response.h"
#include "bcesdk/bos/response/put_bucket_response.h"
#include "bcesdk/bos/response/restore_object_response.h"
#include "bcesdk/bos/request/put_object_tagging_request.h"
#include "bcesdk/bos/response/put_object_tagging_response.h"
#include "bcesdk/bos/request/get_object_tagging_request.h"
#include "bcesdk/bos/response/get_object_tagging_response.h"


BEGIN_CPPSDK_NAMESPACE

typedef std::function<void(off_t, size_t)> progress_cb_t;
class ClientImpl;

struct BceRequestContext {
    BceRequestContext() : request(NULL), response(NULL), is_own(false) {}
    ~BceRequestContext() {
        if (is_own) {
            if (request != NULL) {
                delete request;
            }
            if (response != NULL) {
                delete response;
            }
        }
    }
    BceRequest *request;
    BceResponse *response;
    bool is_own;
};

class Client {
public:
    Client(const std::string &access_key_id, const std::string &access_key_secret);
    Client(const std::string &access_key_id, const std::string &access_key_secret,
            const ClientOptions &options);
    Client(const Credential &credential, const ClientOptions &options);
    ~Client();

    int send_request(BceRequest &request, BceResponse *response);
    int send_request(int n, BceRequestContext ctx[], int max_parallel = 0);

    // Object api
    int head_object(HeadObjectRequest &request, HeadObjectResponse *result) const;
    int put_object(PutObjectRequest &request, PutObjectResponse *result) const;
    int get_object(GetObjectRequest &request, GetObjectResponse *result) const;
    int delete_object(DeleteObjectRequest &request, DeleteObjectResponse *result) const;
    int copy_object(CopyObjectRequest &request, CopyObjectResponse *result) const;
    int append_object(AppendObjectRequest &request, AppendObjectResponse *result) const;
    int restore_object(RestoreObjectRequest &request, RestoreObjectResponse *result) const;
    int put_object_acl(PutObjectAclRequest &request, PutObjectAclResponse *result) const;
    int get_object_acl(GetObjectAclRequest &request, GetObjectAclResponse *result) const;
    int delete_object_acl(DeleteObjectAclRequest &request, DeleteObjectAclResponse *result) const;
    int put_object_tagging(PutObjectTaggingRequest &request, PutObjectTaggingResponse *result) const;
    int get_object_tagging(GetObjectTaggingRequest &request, GetObjectTaggingResponse *result) const;
    int post_object(PostObjectRequest &request, PostObjectResponse *result) const;
    /**
     *
     * @return non zero if request/response has not been sent/parsed correctly
     */
    int init_multipart_upload(InitMultiUploadRequest &request,
            InitMultiUploadResponse *result) const;
    int upload_part(UploadPartRequest &request, UploadPartResponse *result) const;
    int copy_part(CopyPartRequest &request, CopyPartResponse *result);
    /**
     *
     * @return non zero if request/response has not been sent/parsed correctly
     */
    int abort_multipart_upload(AbortMultipartUploadRequest &request,
            AbortMultipartUploadResponse *result) const;
    int complete_multipart_upload(CompleteMultipartUploadRequest &request,
            CompleteMultipartUploadResponse *result) const;
    int list_parts(ListPartsRequest &request, ListPartsResponse *result) const;
    int list_multipart_uploads(ListMultipartUploadsRequest &request,
            ListMultipartUploadsResponse *result) const;
    int list_objects(ListObjectsRequest &request, ListObjectsResponse *result) const;

    // Bucket api
    int get_bucket_location(GetBucketLocationRequest &request, GetBucketLocationResponse *result) const;
    int delete_bucket(DeleteBucketRequest &request, DeleteBucketResponse *result) const;
    int get_bucket_acl(GetBucketAclRequest &request, GetBucketAclResponse *result) const;
    int head_bucket(HeadBucketRequest &request, HeadBucketResponse *result) const;
    int list_buckets(ListBucketsRequest &request, ListBucketsResponse *result) const;
    int put_bucket_acl(PutBucketAclRequest &request, PutBucketAclResponse *result) const;
    int put_bucket(PutBucketRequest &request, PutBucketResponse *result) const;

    // utility method for conveniently usage
    int put_object(const std::string &bucket_name, const std::string &object_name,
            const std::string &data) const;
    int get_object(const std::string &bucket_name, const std::string &object_name,
            std::string *data) const;
    int delete_object(const std::string &bucket_name, const std::string &object_name) const;

    int copy_object(const std::string &src_bucket, const std::string &src_object,
        const std::string &dst_bucket, const std::string &dst_object,
        const std::string &storage_class, ObjectMetaData *meta = NULL);

    std::string generate_url(const std::string &bucket, const std::string &object,
            int expire_seconds = 1800);

    int download_file(
            const std::string &bucket_name,
            const std::string &object_name,
            FileOutputStream &fos,
            int64_t start = -1,
            int64_t length = -1,
            ObjectMetaData *object_meta_data = NULL,
            std::string *etag = NULL,
            int64_t *content_length = NULL,
            std::string *range = NULL);
    int download_file(
            const std::string &bucket_name,
            const std::string &object_name,
            const std::string &file_name,
            int64_t start,
            int64_t length,
            ObjectMetaData *object_meta_data = NULL,
            std::string *etag = NULL,
            int64_t *content_length = NULL,
            std::string *range = NULL);
    int download_file(
            const std::string &bucket_name,
            const std::string &object_name,
            fd_t fd,
            int64_t start,
            int64_t length,
            ObjectMetaData *object_meta_data = NULL,
            std::string *etag = NULL,
            int64_t *content_length = NULL,
            std::string *range = NULL);
    int upload_file(
            const std::string &bucket_name,
            const std::string &object_name,
            FileInputStream &file,
            ObjectMetaData *meta = NULL);
    int upload_file(
            const std::string &bucket_name,
            const std::string &object_name,
            const std::string &file_name,
            ObjectMetaData *meta = NULL);
    int upload_file(
            const std::string &bucket_name,
            const std::string &object_name,
            fd_t fd,
            ObjectMetaData *meta = NULL);
    int upload_super_file(
            const std::string &bucket_name,
            const std::string &object_name,
            fd_t fd,
            ObjectMetaData *meta = NULL);
    int upload_super_file(
            const std::string &bucket_name,
            const std::string &object_name,
            const std::string &file_name,
            ObjectMetaData *meta = NULL);

    int parallel_download(const std::string &bucket, const std::string &object,
        fd_t fd, int64_t start = 0, int64_t size = -1, ObjectMetaData *meta = NULL, 
        progress_cb_t success_cb = nullptr, 
        progress_cb_t err_cb = nullptr);
    int parallel_download(const std::string &bucket, const std::string &object,
        FileOutputStream &file, int64_t start = 0, int64_t size = -1, ObjectMetaData *meta = NULL,
        progress_cb_t success_cb = nullptr,
        progress_cb_t err_cb = nullptr);

    int parallel_upload(const std::string &bucket, const std::string &object,
            FileInputStream &file, ObjectMetaData *meta = NULL);

    int parallel_copy(const std::string &src_bucket, const std::string &src_object,
            const std::string &dst_bucket, const std::string &dst_object) {
        return parallel_copy(src_bucket, src_object, dst_bucket, dst_object, "");
    }
    int parallel_copy(const std::string &src_bucket, const std::string &src_object,
            const std::string &dst_bucket, const std::string &dst_object,
            const std::string &storage_class);
private:
    ClientImpl *_impl;
};

typedef Client BosClient;

class MultipleUploadHelper {
public:
    static inline int64_t calc_part_size(int64_t size, int64_t blksize = 10485760) {
        if (blksize <= 0) {
            blksize = 10485760;
        }
        int n = int((size + blksize - 1) / blksize);
        if (n > 10000) {
            blksize = (size + 9999) / 10000;
        }
        blksize = ((blksize + 0xfffff) >> 20) << 20;
        return blksize;
    }
public:
    MultipleUploadHelper(BosClient *client) : _complete_request(NULL) {
        _client = client;
    }
    ~MultipleUploadHelper();

    int init();

    int init(const std::string &bucket, const std::string &object, ObjectMetaData *meta = NULL);

    void add_part(int part_number, const std::string &etag) {
        _complete_request->add_part(part_number, etag);
    }

    const std::string &upload_id() const {
        if (_complete_request == NULL) {
            const static std::string empty;
            return empty;
        }
        return _complete_request->upload_id();
    }

    InitMultiUploadRequest &init_request() {
        return _init_request;
    }

    int complete();
private:
    BosClient *_client;
    InitMultiUploadRequest _init_request;
    CompleteMultipartUploadRequest *_complete_request;
};

END_CPPSDK_NAMESPACE
#endif

