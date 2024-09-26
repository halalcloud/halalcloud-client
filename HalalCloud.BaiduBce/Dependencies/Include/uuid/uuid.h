#pragma once

#ifdef _WIN32
#include <Windows.h>
inline void uuid_generate(uuid_t & out) {
    CoCreateGuid(&out);
}

inline void uuid_unparse(const uuid_t & guid, void * output) {
    char guid_string[40];
    sprintf_s(
        guid_string, 
        "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", //size == 36
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1], guid.Data4[2],
        guid.Data4[3], guid.Data4[4], guid.Data4[5],
        guid.Data4[6], guid.Data4[7]);
    memcpy(output, guid_string, 36);
}

#endif
