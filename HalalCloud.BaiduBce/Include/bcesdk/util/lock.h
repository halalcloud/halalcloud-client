/***************************************************************************
 * 
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
/**
 * @file include/bcesdk/util/lock.h
 * @author lili19(com@baidu.com)
 * @date 2017/08/20 21:38:28
 * @brief 
 *  
 **/
#ifndef BAIDU_BOS_CPPSDK_INCLUDE_BCESDK_UTIL_LOCK_H
#define BAIDU_BOS_CPPSDK_INCLUDE_BCESDK_UTIL_LOCK_H

#include "bcesdk/common/common.h"
#include <pthread.h>
#include <vector>

BEGIN_CPPSDK_NAMESPACE

class TLSLock {
public:
    TLSLock() {
        pthread_mutex_init(&_lock, NULL);
        pthread_key_create(&_rwlock_key, NULL);
    }

    ~TLSLock() {
        pthread_key_delete(_rwlock_key);
        for (size_t i = 0; i < _tls_locks.size(); ++i) {
            pthread_mutex_destroy(_tls_locks[i]);
            delete _tls_locks[i];
        }
        pthread_mutex_destroy(&_lock);
    }

    pthread_mutex_t *r_lock() {
        pthread_mutex_t *tls_lock = (pthread_mutex_t *) pthread_getspecific(_rwlock_key);
        if (tls_lock == NULL) {
            tls_lock = new pthread_mutex_t;
            pthread_mutex_init(tls_lock, NULL);
            pthread_setspecific(_rwlock_key, tls_lock);
            pthread_mutex_lock(&_lock);
            _tls_locks.push_back(tls_lock);
            pthread_mutex_unlock(&_lock);
        }
        pthread_mutex_lock(tls_lock);
        return tls_lock;
    }

    void r_unlock() {
        pthread_mutex_t *tls_lock = (pthread_mutex_t *) pthread_getspecific(_rwlock_key);
        if (tls_lock != NULL) {
            pthread_mutex_unlock(tls_lock);
        }
    }

    void w_lock() {
        pthread_mutex_lock(&_lock);
        for (size_t i = 0; i < _tls_locks.size(); ++i) {
            pthread_mutex_lock(_tls_locks[i]);
        }
    }

    void w_unlock() {
        for (size_t i = 0; i < _tls_locks.size(); ++i) {
            pthread_mutex_unlock(_tls_locks[i]);
        }
        pthread_mutex_unlock(&_lock);
    }

private:
    std::vector<pthread_mutex_t *> _tls_locks;
    pthread_mutex_t _lock;
    pthread_key_t _rwlock_key;

    TLSLock(const TLSLock &) {}
    TLSLock &operator=(const TLSLock &) { return *this; }
};

class TLSLockReadGuard {
public:
    TLSLockReadGuard(TLSLock &lock) {
        _lock = lock.r_lock();
    }
    void unlock() {
        pthread_mutex_unlock(_lock);
        _lock = NULL;
    }
    ~TLSLockReadGuard() {
        if (_lock != NULL) {
            pthread_mutex_unlock(_lock);
        }
    }
private:
    pthread_mutex_t *_lock;
};

class TLSLockWriteGuard {
public:
    TLSLockWriteGuard(TLSLock &lock) {
        _lock = &lock;
        _lock->w_lock();
    }
    ~TLSLockWriteGuard() {
        _lock->w_unlock();
    }
private:
    TLSLock *_lock;
};

END_CPPSDK_NAMESPACE

#endif

