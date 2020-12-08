// Copyright (c) 2020 The cframe Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CB_BASECLASS_H_
#define CB_BASECLASS_H_

namespace cb {

/**
 * @brief 不可复制类
*/
class NonCopyable {
 protected:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

    NonCopyable() = default;
    ~NonCopyable() = default;
};

}   // namespace cb

#endif  // CB_BASECLASS_H_
