// Copyright (c) 2020 The cframe Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CB_LOCK_H_
#define CB_LOCK_H_

#include "cbase/cb_baseclass.h"
#include <pthread.h>

namespace cb {

/**
 * @brief 锁接口类
*/
class LockInterface : public NonCopyable {
 public:
    /**
     * @brief 加锁
     * @return 0-成功 其他-加锁失败 
    */
    virtual int Lock() = 0;

    /**
     * @brief 尝试加锁
     * @return 0-成功 其他-加锁失败 
    */
    virtual int TryLock() = 0;

    /**
     * @brief 释放锁
     * @return 0-成功 其他-加锁失败
    */
    virtual int UnLock() = 0;
};

class MutexLock : public LockInterface {
 public:
    MutexLock() {}
    ~MutexLock() {}

    virtual int Lock();
    virtual int TryLock();
    virtual int UnLock();

 private:
     pthread_mutex_t m_mutex;
};


/**
 * @brief 范围锁
 * 基于RAII风格，实现作用域内自动加锁与释放
*/
class ScopedLock : public NonCopyable {
 public:
    /**
     * @brief 构造函数 附带加锁操作 
     * @param cLock 锁实例
    */
    ScopedLock(LockInterface& cLock) : m_lock(cLock) {
        m_lock->Lock();
    }

    /**
     * @brief 析构函数 附带释放锁操作
    */
    ~ScopedLock() {
        m_lock->UnLock();
    }

 private:
    LockInterface& m_lock;
};

}   // namespace cb

#endif  // CB_LOCK_H_
