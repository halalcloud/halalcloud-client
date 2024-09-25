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
#ifndef BAIDU_BOS_CPPSDK_BOS_COMMON_FILE_STREAM_H
#define BAIDU_BOS_CPPSDK_BOS_COMMON_FILE_STREAM_H

#include <errno.h>     /* for errno */
#include <stdio.h>     /* for standard C file IO */
#include <sys/stat.h>  /* for low level file IO */
#include <unistd.h>    /* for low level file IO */
#include <fcntl.h>

#include "bcesdk/common/common.h"
#include "bcesdk/common/stream.h"

BEGIN_CPPSDK_NAMESPACE

class FileInputStream : public InputStream {
public:
    FileInputStream(const std::string &filename, off_t offset = 0, ssize_t size = -1) {
        _last_error = 0;
        int fd = open(filename.c_str(), O_RDONLY);
        if (fd < 0) {
            _last_error = errno;
            return;
        }
        init_by_fd(fd, offset, size);
        _own_fd = true;
    }

    FileInputStream(fd_t fd, off_t offset = 0, ssize_t size = -1, bool own_fd = false) {
        _last_error = 0;
        init_by_fd(fd, offset, size);
        _own_fd = own_fd;
    }

    virtual ~FileInputStream() {
        if (_fd >= 0 && _own_fd) {
            close(_fd);
            _fd = -1;
        }
    }

    virtual int last_error() const { return _last_error; }

    virtual int64_t read(int64_t size, std::string *data) {
        char *buffer = new char[size];
        int64_t data_len = read(buffer, size);
        if (data_len > 0) {
            data->assign(buffer, data_len);
        }
        delete []buffer;
        return data_len;
    }

    virtual int64_t read(char *buffer, int64_t size) {
        int64_t remain = _size - (_offset - _start_pos);
        if (size > remain) {
            size = remain;
        }
        ssize_t n = pread(_fd, buffer, size, _offset);
        if (n < 0) {
            _last_error = errno;
            return -1;
        }
        _offset += n;
        return n;
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

    int init_by_fd(int fd, off_t offset, ssize_t size) {
        _fd = fd;
        _start_pos = offset;
        _offset = offset;
        if (size < 0) {
            struct stat st;
            int ret = fstat(_fd, &st);
            if (ret != 0) {
                _last_error = errno;
                return -1;
            }
            _size = st.st_size - offset;
        } else {
            _size = size;
        }
        return 0;
    }

private:
    int _fd;
    int64_t _size;
    off_t _start_pos;
    off_t _offset;
    int _last_error;
    bool _own_fd;
};

class FileOutputStream : public OutputStream {
public:
    FileOutputStream(const std::string &file_name, off_t offset = 0) {
        _last_error = 0;
        _fd = open(file_name.c_str(), O_WRONLY | O_CREAT, 0666);
        if (_fd < 0) {
            _last_error = errno;
            return;
        }
        _start_pos = offset;
        _offset = offset;
        _own_fd = true;
    }

    FileOutputStream(fd_t fd, off_t offset = 0, bool own_fd = false) {
        _last_error = 0;
        _fd = fd;
        _start_pos = offset;
        _offset = offset;
        _own_fd = own_fd;
    }

    virtual ~FileOutputStream() {
        if (_fd >= 0 && _own_fd) {
            close(_fd);
            _fd = -1;
        }
    }

    int last_error() const { return _last_error; }

    virtual int64_t write(const std::string &data) {
        return write(data.c_str(), data.size());
    }

    virtual int64_t write(const char *buffer, int64_t size) {
        ssize_t n = pwrite(_fd, buffer, size, _offset);
        if (n < 0) {
            _last_error = errno;
            return -1;
        }
        _offset += n;
        return n;
    }

    virtual int64_t reserve(int64_t size) {
        (void) size;
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
    int _fd;
    off_t _start_pos;
    off_t _offset;
    int _last_error;
    bool _own_fd;
};

END_CPPSDK_NAMESPACE
#endif

