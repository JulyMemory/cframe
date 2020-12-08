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
 * @brief ��ʱ����
 */
class tw_timer {
 public:
  /**
   * @brief ���캯��
   * @param rot ����ʱ����ʱ����ת��Ȧ����
   * @param ts ��ʶ��
   */
  tw_timer(int rot, int ts)
      : next(NULL), prev(NULL), rotation(rot), time_slot(ts) {}
  /**
   * @brief ��������
   */
  ~tw_timer() {}

 private:
  int rotation;  //! ��¼��ʱ����ʱ����ת������Ȧ����Ч
  int time_slot;  //! ��¼��ʱ������ʱ�����ϵ��ĸ��ۣ���Ӧ������

  void (*callback)(client_data *);  //! ��ʱ���ص�����
  client_data *user_data;           //! �ͻ�����

  tw_timer *next;  //! ָ����һ����ʱ��
  tw_timer *prev;  //! ָ��ǰһ����ʱ��
};

/**
 * @brief ʱ����������
 */
class time_wheel {
 public:
  /**
   * @brief ���캯��
   */
  time_wheel() : cur_slot(0) {
    for (int i = 0; i < N; ++i) {
      slots[i] = NULL;
    }
  }

  /**
   * @brief ��������
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
   * @brief ��Ӷ�ʱ�� O(1)
   * @param timeout ��ʱ���
   * @return �´����Ķ�ʱ��
   */
  tw_timer *add_timer(int timeout) {
    if (timeout < 0) {
      return NULL;
    }

    /* ���ݴ����붨ʱ���ĳ�ʱֵ����������ת�����ٸ��δ�ticks���󱻴�����
     * ���������ĳ�ʱֵС��ʱ���ֵĲ۸���SI����ticks�����ۺ�Ϊ1������ticks����
     * �ۺ�Ϊtimeout/SI��
     */
    int ticks = 0;
    if (timeout < SI) {
      ticks = 1;
    } else {
      ticks = timeout / SI;
    }

    //! ���������Ķ�ʱ����ʱ����ת������Ȧ�󱻴���
    int rotation = ticks / N;
    //! ���������Ķ�ʱ��Ӧ�ñ������ĸ�����
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
   * @brief ɾ����ʱ�� O(1)
   * @param timer ��ʱ��
   */
  void del_timer(tw_timer *timer) {
    if (timer == NULL) {
      return;
    }

    int ts = timer->time_slot;
    /*
     * slots[ts]�ǲ۵�ͷ��㡣���Ŀ�궨ʱ�����Ǹ�ͷ��㣬�����õ�ts��ͷ��㡣
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
   * @brief ʱ���ֶ�ʱ���� O(n)
   * ��SIʱ�䵽����øú�����ʱ������ǰ����һ���۵ļ��
   */
  void tick() {
    tw_timer *tmp = slots[cur_slot];  //! ȡ��ʱ�����ϵ�ǰ�۵�ͷ���
    //! todo: print log

    while (tmp) {
      if (tmp->rotation > 0) {
        //! �����ʱ����rotationֵ����0����������һ�ֲ�������
        tmp->rotation--;
        tmp = tmp->next;
      } else {
        //! ��ʱ���Ѿ����ڣ�ִ�ж�ʱ����Ȼ��ɾ���ö�ʱ��
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
    //! ����ʱ���ֵ�ǰ�ۣ��Լ�ʱ���ֵ�ת��
    cur_slot = ++cur_slot % N;
  }

 private:
  static const int N = 60;  //! ʱ�����ϲ۵���Ŀ
  static const int SI = 1;  //! ÿ1sת��һ�Σ����ۼ��Ϊ1s
  tw_timer *slots[N];  //! ʱ���ֵĲۣ�����ÿ��Ԫ��ָ��һ����ʱ��������������
  int cur_slot;        //! ָ��ʱ���ֵ�ǰ��
};

#endif  // CB_TW_TIMER_H_