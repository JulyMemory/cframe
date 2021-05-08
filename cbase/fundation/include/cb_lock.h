#ifndef CB_LOCK_H_
#define CB_LOCK_H_

#include <pthread.h>

#include "cbase/cb_baseclass.h"

namespace cb {
namespace fundation {

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
  ScopedLock(LockInterface& cLock) : m_lock(cLock) { m_lock->Lock(); }

  /**
   * @brief �������� �����ͷ�������
   */
  ~ScopedLock() { m_lock->UnLock(); }

 private:
  LockInterface& m_lock;
};

/**
 * @brief ������
 * ���ڶ��߳�ͬ��
 */
class MutexLock : public LockInterface {
 public:
  /**
   * @brief ���캯��
   * @param isRecursive 
   * @param isSharedInProcess 
  */
  MutexLock(bool isRecursive = false, bool isSharedInProcess = false);

  /**
   * @brief �����������ͷ�������
   */
  ~MutexLock();

  /**
   * @brief ����
   * @return
   */
  virtual int Lock();

  /**
   * @brief ���Լ���
   * @return
   */
  virtual int TryLock();

  /**
   * @brief ����
   * @return
   */
  virtual int UnLock();

 private:
  pthread_mutex_t m_mutex;
};

/**
 * @brief ������ʵ����
 */
class SpinLock : public LockInterface {
 public:
  /**
   * @brief ���캯��
   * @param isSharedInProcess 
  */
  SpinLock(bool isSharedInProcess = false);

  /**
    * @brief �����������ͷ�������
  */
  ~SpinLock();

  /**
   * @brief ����
   * @return 
  */
  virtual int Lock();

  /**
   * @brief ���Լ���
   * @return 
  */
  virtual int TryLock();

  /**
   * @brief ����
   * @return 
  */
  virtual int UnLock();

 private:
  pthread_spinlock_t m_sLock;
};

/**
 * @brief ��д��
*/
class RWLock : LockInterface {

};

}  // namespace fundation

}  // namespace cb

#endif  // CB_LOCK_H_
