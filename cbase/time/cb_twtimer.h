// Copyright (c) 2020 The cframe Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CB_TW_TIMER_H_
#define CB_TW_TIMER_H_

#include <netinet/in.h>
#include <time.h>

constexpr int BUFFER_SIZE = 64;

class tw_timer;

struct client_data {
  sockaddr_in address;
  int sockfd;
  char buf[BUFFER_SIZE];
  tw_timer *timer;
};

/**
 * @brief 定时器类
 */
class tw_timer {
 public:
  /**
   * @brief 构造函数
   * @param rot 触发时机（时间轮转动圈数）
   * @param ts 标识槽
   */
  tw_timer(int rot, int ts)
      : next(NULL), prev(NULL), rotation(rot), time_slot(ts) {}
  /**
   * @brief 析构函数
   */
  ~tw_timer() {}

 private:
  int rotation;  //! 记录定时器在时间轮转动多少圈后生效
  int time_slot;  //! 记录定时器属于时间轮上的哪个槽（对应的链表）

  void (*callback)(client_data *);  //! 定时器回调函数
  client_data *user_data;           //! 客户数据

  tw_timer *next;  //! 指向下一个定时器
  tw_timer *prev;  //! 指向前一个定时器
};

/**
 * @brief 时间轮容器类
 */
class time_wheel {
 public:
  /**
   * @brief 构造函数
   */
  time_wheel() : cur_slot(0) {
    for (int i = 0; i < N; ++i) {
      slots[i] = NULL;
    }
  }

  /**
   * @brief 析构函数
   */
  ~time_wheel() {
    for (int i = 0; i < N; i++) {
      tw_timer *tmp = slots[i];
      while (tmp) {
        slots[i] = tmp->next;
        delete tmp;
        tmp = slots[i];
      }
    }
  }

  /**
   * @brief 添加定时器 O(1)
   * @param timeout 定时间隔
   * @return 新创建的定时器
   */
  tw_timer *add_timer(int timeout) {
    if (timeout < 0) {
      return NULL;
    }

    /* 根据待插入定时器的超时值计算它将在转动多少个滴答（ticks）后被触发。
     * 如果待插入的超时值小于时间轮的槽隔间SI，则将ticks向上折合为1，否则ticks向下
     * 折合为timeout/SI。
     */
    int ticks = 0;
    if (timeout < SI) {
      ticks = 1;
    } else {
      ticks = timeout / SI;
    }

    //! 计算待插入的定时器在时间轮转动多少圈后被触发
    int rotation = ticks / N;
    //! 计算待插入的定时器应该被插入哪个槽中
    int ts = (cur_slot + (ticks % N)) % N;
    tw_timer *timer = new tw_timer(rotation, ts);

    if (slots[ts] == NULL) {
      //! todo: print log
      slots[ts] = timer;
    } else {
      timer->next = slots[ts];
      slots[ts]->prev = timer;
      slots[ts] = timer;
    }

    return timer;
  }

  /**
   * @brief 删除定时器 O(1)
   * @param timer 定时器
   */
  void del_timer(tw_timer *timer) {
    if (timer == NULL) {
      return;
    }

    int ts = timer->time_slot;
    /*
     * slots[ts]是槽的头结点。如果目标定时器就是该头结点，则重置第ts个头结点。
     */
    if (timer == slots[ts]) {
      slots[ts] = slots[ts]->next;

      if (slots[ts] != NULL) {
        slots[ts]->prev = NULL;
      }
    } else {
      timer->prev->next = timer->next;
      if (timer->next != NULL) {
        timer->next->prev = timer->prev;
      }
    }

    delete timer;
  }

  /**
   * @brief 时间轮定时任务 O(n)
   * 当SI时间到达，调用该函数，时间轮向前滚动一个槽的间隔
   */
  void tick() {
    tw_timer *tmp = slots[cur_slot];  //! 取得时间轮上当前槽的头结点
    //! todo: print log

    while (tmp) {
      if (tmp->rotation > 0) {
        //! 如果定时器的rotation值大于0，则它在这一轮不起作用
        tmp->rotation--;
        tmp = tmp->next;
      } else {
        //! 定时器已经到期，执行定时任务，然后删除该定时器
        tmp->callback(tmp->user_data);
        if (tmp == slots[cur_slot]) {
          slot[cur_slot] = tmp->next;
          delete tmp;

          if (slots[cur_slot] != NULL) {
            slots[cur_slot]->prev = NULL;
          }
          tmp = slots[cur_slot];
        } else {
          tmp->prev->next = tmp->next;
          if (tmp->prev != NULL) {
            tmp->next->prev = tmp->prev;
          }
          tw_timer *next = tmp->next;
          delete tmp;
          tmp = next;
        }
      }
    }
    //! 更新时间轮当前槽，以及时间轮的转动
    cur_slot = ++cur_slot % N;
  }

 private:
  static const int N = 60;  //! 时间轮上槽的数目
  static const int SI = 1;  //! 每1s转动一次，即槽间隔为1s
  tw_timer *slots[N];  //! 时间轮的槽，其中每个元素指向一个定时器链表，链表无序
  int cur_slot;        //! 指向时间轮当前槽
};

#endif  // CB_TW_TIMER_H_