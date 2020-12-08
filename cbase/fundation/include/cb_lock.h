// Copyright (c) 2020 The cframe Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CB_LOCK_H_
#define CB_LOCK_H_

#include "cbase/cb_baseclass.h"
#include <pthread.h>

namespace cb {

/**
 * @brief ���ӿ���
*/
class LockInterface : public NonCopyable {
 public:
    /**
     * @brief ����
     * @return 0-�ɹ� ����-����ʧ�� 
    */
    virtual int Lock() = 0;

    /**
     * @brief ���Լ���
     * @return 0-�ɹ� ����-����ʧ�� 
    */
    virtual int TryLock() = 0;

    /**
     * @brief �ͷ���
     * @return 0-�ɹ� ����-����ʧ��
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
 * @brief ��Χ��
 * ����RAII���ʵ�����������Զ��������ͷ�
*/
class ScopedLock : public NonCopyable {
 public:
    /**
     * @brief ���캯�� ������������ 
     * @param cLock ��ʵ��
    */
    ScopedLock(LockInterface& cLock) : m_lock(cLock) {
        m_lock->Lock();
    }

    /**
     * @brief �������� �����ͷ�������
    */
    ~ScopedLock() {
        m_lock->UnLock();
    }

 private:
    LockInterface& m_lock;
};

}   // namespace cb

#endif  // CB_LOCK_H_
