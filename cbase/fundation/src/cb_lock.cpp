#include "cbase/cb_lock.h"

namespace cb {
namespace fundation {

MutexLock::MutexLock(bool isRecursive, bool isSharedInProcess) {
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);

  if (isRecursive) {
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  }

  if (isSharedInProcess) {
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
  }

  pthread_mutex_init(&m_mutex, &attr);
  pthread_mutexattr_destroy(&attr);
}

MutexLock::~MutexLock() { pthread_mutex_destroy(&m_mutex); }

int MutexLock::Lock() { return pthread_mutex_lock(&m_mutex); }

int MutexLock::TryLock() { return pthread_mutex_trylock(&m_mutex); }

int MutexLock::UnLock() { return pthread_mutex_unlock(&m_mutext); }

SpinLock::SpinLock(bool isSharedInProcess) {
  if (isSharedInProcess) {
    pthread_spin_init(m_slock, PTHREAD_PROCESS_SHARED);
  } else {
    pthread_spin_init(m_slock, PTHREAD_PROCESS_PRIVATE);
  }
}

SpinLock::~SpinLock() { pthread_spin_destroy(&m_sLock); }

int SpinLock::Lock() { return pthread_spin_lock(&m_sLock); }

int SpinLock::UnLock() { return pthread_spin_unlock(&m_sLock); }

int SpinLock::TryLock() { return pthread_spin_trylock(&m_sLock); }

}  // namespace fundation
}  // namespace cb