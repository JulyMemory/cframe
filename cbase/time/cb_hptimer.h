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
 * @brief ��ʱ����
 */
class heap_timer {
 public:
  heap_timer(int delay) { expire = time(NULL) + delay; }

 private:
  time_t expire;                    //! ��ʱ����Ч�ľ���ʱ��
  client_data *user_data;           //! �û�����
  void (*callback)(client_data *);  //! �ص�����
};

/**
 * @brief ʱ�����
 */
class time_heap {
 public:
  /**
   * @brief ���캯��
   * @param cap ������
   */
  time_heap(int cap) : capacity(cap), size(0) {
    array = new heap_timer *[capacity];
    assert(array != NULL);

    for (int i = 0; i < capacity; ++i) {
      array[i] = NULL;
    }
  }

  /**
   * @brief ���캯��
   * @param init_array �Ѵ�������
   * @param size �����С
   * @param capacity ������
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
        //! �������е�[(size-1)/2]~0��Ԫ��ִ�����˲���
        percolate_down(i);
      }
    }
  }

  /**
   * @brief ��������
   */
  ~time_heap() {
    for (int i = 0; i < size; ++i) {
      delete array[i];
    }
    delete[] array;
    array = NULL;
  }

  /**
   * @brief ��Ӷ�ʱ�� O(logn)
   * @param timer ��ʱ��
   */
  void add_timer(heap_timer *timer) {
    if (timer == NULL) {
      return;
    }

    //! ��������������
    if (size >= capacity) {
      resize();
    }

    int hole = size++;
    int parent = 0;

    //! �Դӿ�Ѩ�����ڵ��·���ϵ����нڵ�ִ�����˲���
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
   * @brief ɾ����ʱ��  O(1)
   * @param timer ��ʱ��
   */
  void del_timer(heap_timer *timer) {
    if (timer == NULL) {
      return;
    }

    //! �ӳ����٣�ȱ�㣺���׵�����������
    timer->callback = NULL;
  }

  /**
   * @brief ��ȡ�Ѷ���ʱ��
   * @return ��ʱ��
   */
  heap_timer *top() const {
    if (empty()) {
      return;
    }

    return array[0];
  }

  /**
   * @brief ɾ���Ѷ���ʱ��
   */
  void pop_timer() {
    if (empty()) {
      return;
    }

    if (array[0]) {
      delete array[0];
      //! ��ԭ���ĶѶ�Ԫ���滻Ϊ�����������һ��Ԫ��
      array[0] = array[--size];
      //! ���µĶѶ�Ԫ��ִ�����˲���
      percolate_down(0);
    }
  }

  /**
   * @brief ��ʱ���� O(1)?
   */
  void tick() {
    if (size == 0) {
      return;
    }

    heap_timer *tmp = NULL;
    time_t cur = time(NULL);

    while (!empty()) {
      tmp = array[0];

      //! ����Ѷ���ʱ��û���ڣ����˳�ѭ��
      if (tmp->expire > cur) {
        break;
      }

      if (array[0]->callback != NULL) {
        array[0]->callback(array[0]->user_data);
      }

      //! ɾ���Ѷ�Ԫ�أ�ͬʱ�����µĶѶ���ʱ��
      pop_timer();
    }
  }

  /**
   * @brief �ж��Ƿ�Ϊ��
   * @return trueΪ�գ�false��Ϊ��
   */
  bool empty() const { return size == 0; }

 private:
  /**
   * @brief ��С�����˲���
   * ȷ�����������Ե�hole���ڵ���Ϊ��������ӵ����С������
   * @param hole �ڵ�λ��
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
   * @brief ���ݲ���
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
  heap_timer **array;  //! ������
  int capacity;        //! ����������
  int size;            //! �ѵ�ǰ��С
};

#endif  // CB_HP_TIMER_H_