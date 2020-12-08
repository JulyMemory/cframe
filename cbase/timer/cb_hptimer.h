// Copyright (c) 2020 The cframe Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CB_HP_TIMER_H_
#define CB_HP_TIMER_H_

#include <netinet/in.h>
#include <time.h>

#include <iostream>

constexpr int BUFFER_SIZE = 64;

class heap_timer;

struct client_data {
  sockaddr_in address;
  int sockfd;
  char buf[BUFFER_SIZE];
  heap_timer *timer;
};

/**
 * @brief 定时器类
 */
class heap_timer {
 public:
  heap_timer(int delay) { expire = time(NULL) + delay; }

 private:
  time_t expire;                    //! 定时器生效的绝对时间
  client_data *user_data;           //! 用户数据
  void (*callback)(client_data *);  //! 回调函数
};

/**
 * @brief 时间堆类
 */
class time_heap {
 public:
  /**
   * @brief 构造函数
   * @param cap 堆容量
   */
  time_heap(int cap) : capacity(cap), size(0) {
    array = new heap_timer *[capacity];
    assert(array != NULL);

    for (int i = 0; i < capacity; ++i) {
      array[i] = NULL;
    }
  }

  /**
   * @brief 构造函数
   * @param init_array 已存在数组
   * @param size 数组大小
   * @param capacity 堆容量
   */
  time_heap(heap_timer **init_array, int size, int capacity)
      : size(size), capacity(capacity) {
    assert(size < capacity);

    array = new heap_timer *[capacity];
    assert(array != NULL);
    for (int i = 0; i < capacity; i++) {
      array[i] = NULL;
    }

    if (size != 0) {
      for (int i = 0; i < size; ++i) {
        array[i] = init_array[i];
      }

      for (int i = (this->size - 1) / 2; i >= 0; --i) {
        //! 对数组中第[(size-1)/2]~0个元素执行下滤操作
        percolate_down(i);
      }
    }
  }

  /**
   * @brief 析构函数
   */
  ~time_heap() {
    for (int i = 0; i < size; ++i) {
      delete array[i];
    }
    delete[] array;
    array = NULL;
  }

  /**
   * @brief 添加定时器 O(logn)
   * @param timer 定时器
   */
  void add_timer(heap_timer *timer) {
    if (timer == NULL) {
      return;
    }

    //! 容量不够，扩容
    if (size >= capacity) {
      resize();
    }

    int hole = size++;
    int parent = 0;

    //! 对从空穴到根节点的路径上的所有节点执行上滤操作
    for (; hole > 0; hole = parent) {
      parent = (hole - 1) / 2;
      if (array[parent]->expire <= timer->expire) {
        break;
      }

      array[hole] = array[parent];
    }

    array[hole] = timer;
  }

  /**
   * @brief 删除定时器  O(1)
   * @param timer 定时器
   */
  void del_timer(heap_timer *timer) {
    if (timer == NULL) {
      return;
    }

    //! 延迟销毁，缺点：容易导致数组膨胀
    timer->callback = NULL;
  }

  /**
   * @brief 获取堆顶定时器
   * @return 定时器
   */
  heap_timer *top() const {
    if (empty()) {
      return;
    }

    return array[0];
  }

  /**
   * @brief 删除堆顶定时器
   */
  void pop_timer() {
    if (empty()) {
      return;
    }

    if (array[0]) {
      delete array[0];
      //! 将原来的堆顶元素替换为堆数组中最后一个元素
      array[0] = array[--size];
      //! 对新的堆顶元素执行下滤操作
      percolate_down(0);
    }
  }

  /**
   * @brief 定时任务 O(1)?
   */
  void tick() {
    if (size == 0) {
      return;
    }

    heap_timer *tmp = NULL;
    time_t cur = time(NULL);

    while (!empty()) {
      tmp = array[0];

      //! 如果堆顶定时器没到期，则退出循环
      if (tmp->expire > cur) {
        break;
      }

      if (array[0]->callback != NULL) {
        array[0]->callback(array[0]->user_data);
      }

      //! 删除堆顶元素，同时生成新的堆顶定时器
      pop_timer();
    }
  }

  /**
   * @brief 判断是否为空
   * @return true为空，false不为空
   */
  bool empty() const { return size == 0; }

 private:
  /**
   * @brief 最小堆下滤操作
   * 确保堆数组中以第hole个节点作为根的子树拥有最小堆性质
   * @param hole 节点位置
   */
  void percolate_down(int hole) {
    heap_timer *tmp = array[hole];
    int child = 0;

    for (; (hole * 2 + 1) <= (size - 1); hole = child) {
      child = hole * 2 + 1;
      if ((child < (size - 1)) &&
          (array[child + 1]->expire < array[child]->expire)) {
        ++child;
      }

      if (array[child]->expire < tmp->expire) {
        array[hole] = array[child];
      } else {
        break;
      }
    }

    array[hole] = tmp;
  }

  /**
   * @brief 扩容操作
   */
  void resize() {
    heap_timer **tmp = new heap_timer *[2 * capacity];
    assert(tmp != NULL);

    for (int i = 0; i < 2 * capacity; ++i) {
      tmp[i] = NULL;
    }

    capacity *= 2;
    for (int i = 0; i < size; ++i) {
      tmp[i] = array[i];
    }

    delete[] array;
    array = tmp;
  }

 private:
  heap_timer **array;  //! 堆数组
  int capacity;        //! 堆数组容量
  int size;            //! 堆当前大小
};

#endif  // CB_HP_TIMER_H_