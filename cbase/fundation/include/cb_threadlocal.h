#ifndef CB_THREADLOCAL_H_
#define CB_THREADLOCAL_H_

#include <boost/thread.hpp>

namespace cb {
namespace fundation {

template <typename T>
class ThreadLocal {
 public:
  ThreadLocal() { m_ctsp.reset(nullptr); }
  ~ThreadLocal() {}

 public:
  T* Get() { 
    T* ptsp = m_ctsp.get(); 
    if (ptsp == nullptr) {
      ptsp = new T();
      m_ctsp.reset(ptsp);
    }

    return ptsp;
  }
  T* Release() { return m_ctsp.release(); }
  T* Ptr() { return m_ctsp.get(); }
  void Reset(T* new_value = 0) { m_ctsp.reset(new_value); }

  T* operator->() { return Get(); }
  T& operator*() { return *Get(); }

 private:
  boost::thread_specific_ptr<T> m_ctsp;
};

}  // namespace fundation
}  // namespace cb

#endif