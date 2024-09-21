/*
 * @Author: four1er 92024373@qq.com
 * @Date: 2024-09-20 23:15:58
 * @LastEditors: four1er 92024373@qq.com
 * @LastEditTime: 2024-09-21 22:22:06
 * @FilePath: /random_code/coroutine/coroutine.h
 */
#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>

#include "coroutine.h"

struct schedule;

#define STACK_SIZE 1024 * 1024
typedef void (*coroutine_func)(struct schedule *, void *ud);
#define DEFAULT_COROUTINE 16

// 先声明协程类型
struct coroutine;

struct schedule {
  char stack[STACK_SIZE];
  ucontext_t main;
  int nco;
  int cap;
  int running;
  struct coroutine **co;
};

// coroutine: 协程。对于协程需要什么？
struct coroutine {
  coroutine_func func;   // 协程函数
  void *ud;              // 协程函数参数
  ucontext_t ctx;        // 协程上下文
  struct schedule *sch;  // 协程所属的调度器
  ptrdiff_t cap;         // 协程栈容量
  ptrdiff_t size;        // 协程栈大小
  int status;            // 协程状态
  char *stack;           // 协程栈
};

struct schedule *coroutine_open(void);
void coroutine_close(struct schedule *);

int coroutine_new(struct schedule *, coroutine_func, void *ud);
void coroutine_resume(struct schedule *, int id);
int coroutine_status(struct schedule *, int id);
int coroutine_running(struct schedule *);
void coroutine_yield(struct schedule *);
