/**
 * Copyright 2018 (c) Baidu, Inc.
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
#ifdef _WIN32

#include <windows.h>
#include <Wincrypt.h>
#include <vector>
#include <assert.h>

#include "bcesdk/common/common.h"
#include "bcesdk/util/crypto_util.h"

BEGIN_CPPSDK_NAMESPACE


class CryptContext {
public:
    CryptContext(LPCWSTR szContainer, LPCWSTR szProvider, DWORD dwProvType, DWORD dwFlags)
        : _hProv(NULL) {
        if (szProvider != NULL && szProvider == std::wstring(MS_ENH_RSA_AES_PROV_W)) {
            if (false == IsWin7OrLater()) {
                szProvider = MS_ENH_RSA_AES_PROV_XP_W;
            }
        }
        if (!CryptAcquireContextW(&_hProv, szContainer, szProvider, dwProvType, dwFlags)) {
            assert(!"Windows version too low ?");
        }
    }

    ~CryptContext() {
        if (_hProv) {
            CryptReleaseContext(_hProv, 0);
        }
    }
    
    operator HCRYPTPROV () {
        return _hProv;
    }
private:
    bool IsWin7OrLater() {
#pragma warning( push )  
#pragma warning(disable : 4996)
        DWORD version = GetVersion(); 
#pragma warning( pop )  
        DWORD major = (DWORD)(LOBYTE(LOWORD(version)));
        DWORD minor = (DWORD)(HIBYTE(LOWORD(version)));
        return (major > 6) || ((major == 6) && (minor >= 1));
    }
    HCRYPTPROV  _hProv;
};

class WinMD5Util :public MD5Util {
public:
    WinMD5Util() : _hHash(NULL) {
        static CryptContext hProv(NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);

        //Specify the Hash Algorithm here
        if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &_hHash)) {
            assert(!"CryptCreateHash failed");
            return;
        }
    }

    ~WinMD5Util() {
        if (_hHash) {
            CryptDestroyHash(_hHash);
        }
    }

    virtual void Update(void *data, size_t len) {
        if (!CryptHashData(_hHash, (const BYTE*)data, (DWORD)len, 0)){
            assert(!"CryptHashData failed");
            return;
        }
    }

    virtual void Final(unsigned char *md) {
        DWORD cbHashSize = 0;
        DWORD dwCount = sizeof(DWORD);

        //Get the final hash size 
        if (!CryptGetHashParam(_hHash, HP_HASHSIZE, (BYTE *)&cbHashSize, &dwCount, 0)) {
            assert(!"CryptGetHashParam failed");
            return;
        }

        DWORD dwFinalHashSize = 32;
        //Now get the computed hash 
        if (!CryptGetHashParam(_hHash, HP_HASHVAL, md, &dwFinalHashSize, 0)) {
            assert(!"CryptGetHashParam failed");
            return;
        }
    }
private:
    HCRYPTHASH _hHash;
};


class WinSHA256Util :public SHA256Util {
public:
    WinSHA256Util() : _hHash(NULL) {
        static CryptContext hProv(NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);

        //Specify the Hash Algorithm here
        if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &_hHash)) {
            assert(!"CryptCreateHash failed");
            return;
        }
    }

    ~WinSHA256Util() {
        if (_hHash) {
            CryptDestroyHash(_hHash);
        }
    }

    virtual void Update(void *data, size_t len) {
        if (!CryptHashData(_hHash, (const BYTE*)data, (DWORD)len, 0)) {
            assert(!"CryptHashData failed");
            return;
        }
    }

    virtual void Final(unsigned char *md) {
        DWORD cbHashSize = 0;
        DWORD dwCount = sizeof(DWORD);

        //Get the final hash size 
        if (!CryptGetHashParam(_hHash, HP_HASHSIZE, (BYTE *)&cbHashSize, &dwCount, 0)) {
            assert(!"CryptGetHashParam failed");
            return;
        }

        DWORD dwFinalHashSize = 256;
        //Now get the computed hash 
        if (!CryptGetHashParam(_hHash, HP_HASHVAL, md, &dwFinalHashSize, 0)) {
            assert(!"CryptGetHashParam failed");
            return;
        }
    }
private:
    HCRYPTHASH _hHash;
};

MD5Util * create_md5_util() {
    return new WinMD5Util();
}

SHA256Util * create_sha256_util() {
    return new WinSHA256Util();
}

void destroy_md5_util(MD5Util * util) {
    delete util;
}

void destroy_sha256_uitl(SHA256Util * util) {
    delete util;
}

std::string hmac_sha256(const std::string &key, const std::string &data) {
    HCRYPTKEY   hKey = NULL;
    HCRYPTHASH  hHmacHash = NULL;
    std::string output;
    static CryptContext hProv(NULL, MS_ENH_RSA_AES_PROV_W, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);

    do {
        typedef struct _keyBlob {
            BLOBHEADER hdr;
            DWORD cbKeySize;
            BYTE rgbKeyData[256];
        } keyBlob;

        BYTE bKeyBuf[1024] = { 0 };
        keyBlob *pBlob = NULL;
        int iBlobLen = 0;
        pBlob = (keyBlob *)bKeyBuf;


        pBlob->hdr.bType = PLAINTEXTKEYBLOB;
        pBlob->hdr.aiKeyAlg = CALG_RC2;
        pBlob->hdr.bVersion = CUR_BLOB_VERSION;
        pBlob->cbKeySize = (DWORD)key.length();
        int keyDataSize = pBlob->cbKeySize;
        memcpy(pBlob->rgbKeyData, key.data(), pBlob->cbKeySize);
        iBlobLen = sizeof(BLOBHEADER) + sizeof(DWORD) + pBlob->cbKeySize;

        if (!CryptImportKey(
            hProv,   // handle of the CSP
            bKeyBuf, // key container name
            iBlobLen, //sizeof(HmacKeyBlob),
            NULL,
            CRYPT_IPSEC_HMAC_KEY,
            &hKey)) {
            assert(!"CryptImportKey failed");
            break;
        }

        if (!CryptCreateHash(
            hProv,                    // handle of the CSP.
            CALG_HMAC,                // HMAC hash algorithm ID
            hKey,                     // key for the hash (see above)
            0,                        // reserved
            &hHmacHash)) {
            assert(!"CryptCreateHash failed");
            break;
        }

        HMAC_INFO   HmacInfo;
        memset(&HmacInfo, 0, sizeof(HmacInfo));
        HmacInfo.HashAlgid = CALG_SHA_256;
        if (!CryptSetHashParam(
            hHmacHash,                // handle of the HMAC hash object
            HP_HMAC_INFO,             // setting an HMAC_INFO object
            (BYTE*)&HmacInfo,         // the HMAC_INFO object
            0)) {
            assert(!"CryptSetHashParam failed");
            break;
        }

        if (!CryptHashData(
            hHmacHash,                // handle of the HMAC hash object
            (BYTE *)data.data(),                    // message to hash
            (DWORD)data.length(),            // number of bytes of data to add
            0)) {
            assert(!"CryptHashData failed");
            break;
        }

        DWORD dwDataLen = 0;
        if (!CryptGetHashParam(
            hHmacHash,                // handle of the HMAC hash object
            HP_HASHVAL,               // query on the hash value
            NULL,                     // filled on second call
            &dwDataLen,               // length, in bytes, of the hash
            0)) {
            break;
        }

        output.resize(dwDataLen);
        if (!CryptGetHashParam(
            hHmacHash,                 // handle of the HMAC hash object
            HP_HASHVAL,                // query on the hash value
            (BYTE *)output.data(),                    // pointer to the HMAC hash value
            &dwDataLen,                // length, in bytes, of the hash
            0)) {
            assert(!"CryptGetHashParam failed");
            break;
        }
    } while (0);
    if (hHmacHash)
        CryptDestroyHash(hHmacHash);
    if (hKey)
        CryptDestroyKey(hKey);
    return output;
}

END_CPPSDK_NAMESPACE

#endif
