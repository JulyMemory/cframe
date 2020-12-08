// Copyright (c) 2020 The cframe Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cbase/cb_lock.h"

MutexLock::MutexLock() {
    pthread_mutex_init(&m_mutex, NULL);
}

MutexLock::~MutexLock() {
    pthread_mutex_destroy(&m_mutex);
}

int MutexLock::Lock() {
    return pthread_mutex_lock(&m_mutex);
}

int MutexLock::TryLock() {
    return pthread_mutex_trylock(&m_mutex);
}

int MutexLock::UnLock() {
    return pthread_mutex_unlock(&m_mutext);
}