#ifndef CB_SINGLETON_H_
#define CB_SINGLETON_H_

#include <assert.h>

#include "cbase/cb_baseclass.h"
#include "cbase/cb_lock.h"

namespace cb {
namespace fundation {

/**
 * @brief 单例模式
 * @tparam T
 */
template <typename T>
class Singleton : public NonCopyable {
 public:
  /**
   * @brief 构造函数
   */
  Singleton() : m_pInstance(NULL) {}

  /**
   * @brief 析构函数
   */
  ~Singleton() {
    if (m_pInstance != NULL) {
      delete m_pInstance;
      m_pInstance = NULL;
    }
  }

  T* Get() {
    if (m_pInstance == NULL) {
      ScopedLock cScopeLock(m_lock);
      if (m_pInstance == NULL) {
        m_pInstance = new T();
        assert(m_pInstance != NULL);
      }
    }

    return m_pInstance;
  }

  T* operator->() { return Get(); }

  T& operator*() { return *Get(); }

 private:
  MutexLock m_lock;  // SpinLock?
  volatile T* m_pInstance;
};

}  // namespace fundation
}  // namespace cb

#endif  // CB_SINGLETON_H_
