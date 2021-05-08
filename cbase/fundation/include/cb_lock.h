#ifndef CB_LOCK_H_
#define CB_LOCK_H_

#include <pthread.h>

#include "cbase/cb_baseclass.h"

namespace cb {
namespace fundation {

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
  ScopedLock(LockInterface& cLock) : m_lock(cLock) { m_lock->Lock(); }

  /**
   * @brief 析构函数 附带释放锁操作
   */
  ~ScopedLock() { m_lock->UnLock(); }

 private:
  LockInterface& m_lock;
};

/**
 * @brief 互斥锁
 * 用于多线程同步
 */
class MutexLock : public LockInterface {
 public:
  /**
   * @brief 构造函数
   * @param isRecursive 
   * @param isSharedInProcess 
  */
  MutexLock(bool isRecursive = false, bool isSharedInProcess = false);

  /**
   * @brief 析构函数，释放锁对象
   */
  ~MutexLock();

  /**
   * @brief 加锁
   * @return
   */
  virtual int Lock();

  /**
   * @brief 尝试加锁
   * @return
   */
  virtual int TryLock();

  /**
   * @brief 解锁
   * @return
   */
  virtual int UnLock();

 private:
  pthread_mutex_t m_mutex;
};

/**
 * @brief 自旋锁实现类
 */
class SpinLock : public LockInterface {
 public:
  /**
   * @brief 构造函数
   * @param isSharedInProcess 
  */
  SpinLock(bool isSharedInProcess = false);

  /**
    * @brief 析构函数，释放锁对象
  */
  ~SpinLock();

  /**
   * @brief 加锁
   * @return 
  */
  virtual int Lock();

  /**
   * @brief 尝试加锁
   * @return 
  */
  virtual int TryLock();

  /**
   * @brief 解锁
   * @return 
  */
  virtual int UnLock();

 private:
  pthread_spinlock_t m_sLock;
};

/**
 * @brief 读写锁
*/
class RWLock : LockInterface {

};

}  // namespace fundation

}  // namespace cb

#endif  // CB_LOCK_H_
