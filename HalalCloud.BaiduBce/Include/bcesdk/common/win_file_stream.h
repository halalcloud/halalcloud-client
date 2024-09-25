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
#ifndef BAIDU_BOS_CPPSDK_BOS_COMMON_WIN_FILE_STREAM_H
#define BAIDU_BOS_CPPSDK_BOS_COMMON_WIN_FILE_STREAM_H

#include <windows.h>
#include <errno.h>     /* for errno */
#include <stdio.h>     /* for standard C file IO */
#include <assert.h>

#include "bcesdk/common/common.h"
#include "bcesdk/common/stream.h"

typedef  long ssize_t;
BEGIN_CPPSDK_NAMESPACE

struct FileMapGran {
    FileMapGran() {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        sys_gran = sysInfo.dwAllocationGranularity;
    }
    DWORD sys_gran;
};

class FileInputStream : public InputStream {
public:
    FileInputStream(const std::string &file_name, int64_t offset = 0, int64_t size = -1) {
        _last_error = 0;
        _size = 0;
        HANDLE fd = ::CreateFileA(file_name.c_str(), GENERIC_READ, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (fd == INVALID_HANDLE_VALUE) {
            _last_error = GetLastError();
            assert(!"CreateFileA failed");
            return;
        }
        init_by_fd(fd, offset, size);
    }

    FileInputStream(fd_t fd, int64_t offset = 0, int64_t size = -1, bool own_fd = false) {
        _last_error = 0;
        _size = 0;
        HANDLE new_fd = INVALID_HANDLE_VALUE;
        if (false == own_fd) {
            HANDLE current_process(::GetCurrentProcess());
            if (FALSE == ::DuplicateHandle(current_process, (HANDLE)fd, current_process,
                &new_fd, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
                _last_error = GetLastError();
                assert(!"DuplicateHandle failed");
                return;
            }
        }
        else {
            new_fd = (HANDLE)fd;
        }

        init_by_fd(new_fd, offset, size);
    }

    virtual ~FileInputStream() {
        if (_fm != INVALID_HANDLE_VALUE && _fm != NULL) {
            ::CloseHandle(_fm);
        }
        if (_fd != INVALID_HANDLE_VALUE && _fd != NULL) {
            CloseHandle(_fd);
        }
    }

    virtual int last_error() const { return _last_error; }

    virtual int64_t read(int64_t size, std::string *data) {
        char *buffer = new char[(unsigned int)size];
        int64_t data_len = this->read(buffer, size);
        if (data_len > 0) {
            data->assign(buffer, (unsigned int)data_len);
        }
        delete[]buffer;
        return data_len;
    }

    virtual int64_t read(char *buffer, int64_t size) {
        if (_fm == INVALID_HANDLE_VALUE) {
            return 0;
        }

        static FileMapGran file_map_gran;
        int64_t remain = _size - (_offset - _start_pos);
        if (size > remain) {
            size = remain;
        }
        if (size == 0) {
            return 0;
        }

        int64_t file_map_offset = _offset % file_map_gran.sys_gran;
        int64_t file_map_start = (_offset / file_map_gran.sys_gran) * file_map_gran.sys_gran;
        char *raw_buffer = (char*)MapViewOfFile(_fm, FILE_MAP_READ, (DWORD)(file_map_start >> 32),
            (DWORD)file_map_start, int(file_map_offset + size));
        if (raw_buffer != NULL) {
            memcpy(buffer, raw_buffer + file_map_offset, (int)size);
        }
        else {
            _last_error = GetLastError();
            assert(!"MapViewOfFile error");
            return 0;
        }
        _offset += size;
        ::UnmapViewOfFile(raw_buffer);

        return size;
    }

    virtual int64_t get_size() const {
        return _size;
    }

    virtual int64_t seek(int64_t pos) {
        _offset = _start_pos + pos;
        return 0;
    }

    virtual int64_t tell() const {
        return _offset - _start_pos;
    }

    fd_t fd() {
        return _fd;
    }
    void set_start_pos(off_t pos) {
        _start_pos = pos;
    }
    void set_size(int64_t size) {
        _size = size;
    }
private:
    int init_by_fd(HANDLE fd, int64_t offset, int64_t size) {
        _fd = fd;
        _start_pos = offset;
        _offset = offset;
        if (size < 0) {
            LARGE_INTEGER file_size;
            if (FALSE == ::GetFileSizeEx(_fd, &file_size)) {
                _last_error = GetLastError();
                assert(!"GetFileSizeEx error");
                return -1;
            }
            _size = file_size.QuadPart - offset;
        } else {
            _size = size;
        }
        _fm = ::CreateFileMapping(_fd, NULL, PAGE_READONLY, 0, 0, NULL);
        if (_fm == INVALID_HANDLE_VALUE) {
            assert(!"invalid file mapping");
            return -1;
        }
        return 0;
    }

private:
    HANDLE _fd;
    HANDLE _fm;
    int64_t _size;
    int64_t _start_pos;
    int64_t _offset;
    int _last_error;
};

class FileOutputStream : public OutputStream {
public:
    FileOutputStream(const std::string &file_name, int64_t offset = 0) {
        _last_error = 0;
//        ::DeleteFileA(file_name.c_str());
        _fd = ::CreateFileA(file_name.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (_fd == INVALID_HANDLE_VALUE) {
            _last_error = GetLastError();
            assert(!"CreateFileA failed");
            return;
        }
        _start_pos = offset;
        _offset = offset;
    }

    FileOutputStream(fd_t fd, int64_t offset = 0, bool own_fd = false) {
        _last_error = 0;
        HANDLE new_fd;
        if (false == own_fd) {
            HANDLE current_process(::GetCurrentProcess());
            if (FALSE == ::DuplicateHandle(current_process, (HANDLE)fd, current_process,
                &new_fd, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
                assert(!"DuplicateHandle failed");
                _last_error = GetLastError();
                return;
            }
        } else {
            new_fd = (HANDLE)fd;
        }
        _fd = new_fd;
        _start_pos = offset;
        _offset = offset;

    }

    virtual ~FileOutputStream() {
        if (_fd != INVALID_HANDLE_VALUE) {
            CloseHandle(_fd);
        }
    }

    int last_error() const { return _last_error; }

    virtual int64_t write(const std::string &data) {
        return this->write(data.c_str(), data.size());
    }

    virtual int64_t write(const char *buffer, int64_t size) {
        static FileMapGran file_map_gran;
        int64_t file_map_offset = _offset % file_map_gran.sys_gran;
        int64_t file_map_start = (_offset / file_map_gran.sys_gran) * file_map_gran.sys_gran;

        uint64_t total_size = _offset + size;
        HANDLE hMap = ::CreateFileMapping(_fd, NULL, PAGE_READWRITE, (DWORD)(total_size >> 32),
            (DWORD)(total_size), NULL);
        if (hMap == INVALID_HANDLE_VALUE || hMap == NULL) {
            assert(!"CreateFileMapping failed");
        }

        char *raw_buffer = (char*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, (DWORD)(file_map_start >> 32),
            (DWORD)file_map_start, int(file_map_offset + size));
        if (raw_buffer != NULL) {
            memcpy(raw_buffer + file_map_offset, buffer, (int)size);
        } else {
            _last_error = GetLastError();
            assert(!"MapViewOfFile error");
            return 0;
        }
        ::UnmapViewOfFile(raw_buffer);
        CloseHandle(hMap);
        _offset += size;
        return size;
    }

    virtual int64_t reserve(int64_t size) {
        (void)size;
        return 0;
    }

    virtual int64_t seek(int64_t pos) {
        _offset = _start_pos + pos;
        return 0;
    }

    virtual int64_t tell() const {
        return _offset - _start_pos;
    }

    fd_t fd() {
        return _fd;
    }

private:
    HANDLE _fd;
    int64_t _start_pos;
    int64_t _offset;
    int _last_error;
};

END_CPPSDK_NAMESPACE
#endif

