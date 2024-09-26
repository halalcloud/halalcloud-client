/*
 * PROJECT:   Halal Cloud Client
 * FILE:      HalalCloud.BaiduBce.cpp
 * PURPOSE:   Implementation for the HalalCloud.BaiduBce
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "HalalCloud.BaiduBce.h"

#include "bcesdk/bos/client.h"

extern "C" int HccBcePutObject(
    const char* FilePath,
    const char* Endpoint,
    const char* AccessKey,
    const char* SecretKey,
    const char* Token,
    const char* Bucket,
    const char* Key)
{
    baidu::bos::cppsdk::Credential Credential(
        AccessKey,
        SecretKey,
        Token);
    baidu::bos::cppsdk::ClientOptions Options;
    Options.endpoint = Endpoint;
    Options.max_parallel = 5;
    baidu::bos::cppsdk::Client Client(Credential, Options);
    baidu::bos::cppsdk::FileInputStream FileStream(FilePath);
    baidu::bos::cppsdk::PutObjectRequest Request(Bucket, Key, &FileStream);
    baidu::bos::cppsdk::PutObjectResponse Response;
    Client.put_object(Request, &Response);
    return Response.status_code();
}
