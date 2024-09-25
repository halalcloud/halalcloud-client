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
#ifndef BAIDU_BOS_CPPSDK_BOS_COMMON_MEMORY_STREAM_H
#define BAIDU_BOS_CPPSDK_BOS_COMMON_MEMORY_STREAM_H

#include <string.h>

#include "bcesdk/common/common.h"
#include "bcesdk/common/stream.h"

BEGIN_CPPSDK_NAMESPACE

class MemoryInputStream : public InputStream {
public:
    MemoryInputStream(const void *buffer, int64_t size) :
            _as_long_as_stream_lives(static_cast<const char*>(buffer), size), 
            _buffer(_as_long_as_stream_lives.data()),
            _size(size), _pos(0) {
    }

    virtual ~MemoryInputStream() {}

    virtual int64_t read(int64_t size, std::string *data) {
        int64_t n = size > (_size - _pos) ? (_size - _pos) : size;
        data->assign((const char *) _buffer + _pos, (size_t)n);
        _pos += n;
        return n;
    }

    virtual int64_t read(char *buffer, int64_t size) {
        int64_t n = size > (_size - _pos) ? (_size - _pos) : size;
        memcpy(buffer, (const char *) _buffer + _pos, (size_t)n);
        _pos += n;
        return n;
    }

    virtual int64_t get_size() const {
        return _size;
    }

    virtual int64_t seek(int64_t pos) {
        if (pos < 0 || pos > _size) {
            return -1;
        }
        _pos = pos;
        return pos;
    }

    virtual int64_t tell() const {
        return _pos;
    }

private:
    const std::string _as_long_as_stream_lives;
    const void *_buffer;
    int64_t _size;
    int64_t _pos;
};

class MemoryOutputStream : public OutputStream {
public:
    MemoryOutputStream(void *buffer, int64_t size) : _buffer(buffer), _size(size), _pos(0) {}

    virtual ~MemoryOutputStream() {}

    virtual int64_t write(const std::string &data) {
        return write(data.data(), data.size());
    }

    virtual int64_t write(const char *buffer, int64_t size) {
        int64_t n = size > (_size - _pos) ? (_size - _pos) : size;
        memcpy((char *) _buffer + _pos, buffer, (size_t)n);
        _pos += n;
        return n;
    }

    virtual int64_t reserve(int64_t size) {
        if (size <= _size) {
            return 0;
        }
        return -1;
    }
    virtual int64_t seek(int64_t pos) {
        if (pos < 0 || pos > _size) {
            return -1;
        }
        _pos = pos;
        return pos;
    }

    virtual int64_t tell() const {
        return _pos;
    }

private:
    void *  _buffer;
    int64_t _size;
    int64_t _pos;
};

class StringOutputStream : public OutputStream {
public:
    StringOutputStream(std::string *buffer) : _buffer(buffer), _pos(0) {}

    virtual int64_t write(const std::string &data) {
        return write(data.data(), data.size());
    }

    virtual int64_t write(const char *buffer, int64_t size) {
        if (_pos >= (int64_t) _buffer->length()) {
            _buffer->append(buffer, (size_t)size);
        } else {
            int64_t n = _buffer->length() - _pos;
            if (size > n) {
                _buffer->replace((size_t)_pos, (size_t)n, buffer);
                _buffer->append(buffer + n, (size_t)(size - n));
            } else {
                _buffer->replace((size_t)_pos, (size_t)size, buffer);
            }
        }
        _pos += size;
        return size;
    }

    virtual int64_t reserve(int64_t size) {
        _buffer->reserve((size_t)size);
        return 0;
    }

    virtual int64_t seek(int64_t pos) {
        if (pos < 0) {
            return -1;
        }
        if (pos > (int64_t) _buffer->length()) {
            _buffer->resize((size_t)pos);
        }
        _pos = pos;
        return pos;
    }

    virtual int64_t tell() const {
        return _pos;
    }

private:
    const std::string _as_long_as_stream_lives;
    std::string *_buffer;
    int64_t _pos;
};

END_CPPSDK_NAMESPACE
#endif

