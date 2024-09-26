/*
 * PROJECT:   Halal Cloud Client
 * FILE:      HalalCloud.BaiduBce.h
 * PURPOSE:   Definition for the HalalCloud.BaiduBce
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef HALALCLOUD_BAIDUBCE
#define HALALCLOUD_BAIDUBCE

extern "C" int HccBcePutObject(
    const char* FilePath,
    const char* Endpoint,
    const char* AccessKey,
    const char* SecretKey,
    const char* Token,
    const char* Bucket,
    const char* Key);

#endif // !HALALCLOUD_BAIDUBCE
