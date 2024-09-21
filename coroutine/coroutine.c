/**
 * @file coroutine.c
 * @author {four1er} ({920242373@qq.com})
 * @brief
 * @version 0.1
 * @date 2024-09-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "coroutine.h"

#include <assert.h>
#include <stdint.h>
#include <sys/types.h>
#include <ucontext.h>

// coroutine_open: 创建一个协程。
/**
 * @brief 创建一个协程调度器, 默认协程数16
 *
 * @return struct schedule*
 */
struct schedule *coroutine_open(void) {
  struct schedule *S = malloc(sizeof(*S));
  S->nco = 0;                  // 初始化时，协程数量为0
  S->running = -1;             // 初始化时，没有协程在运行
  S->cap = DEFAULT_COROUTINE;  // 初始化时，协程调度器的容量为默认值
  S->co =
      malloc(sizeof(struct coroutine) * S->cap);  // 分配内存空间，用于存储协程
  memset(S->co, 0, sizeof(struct coroutine) * S->cap);
  return S;
}

/**
 * @brief delete coroutine
 *
 * @param [in] co coroutine
 */
void _delete_co(struct coroutine *co) {
  free(co->stack);
  free(co);
}

/**
 * @brief 关闭协程调度器，释放内存。注意需要关闭每个协程对象。
 *
 * @param S 协程调度器， 指针类型
 */
void coroutine_close(struct schedule *S) {
  int i;
  for (i = 0; i < S->cap; i++) {
    struct coroutine *co = S->co[i];
    if (co) {
      _delete_co(co);
    }
  }
  free(S->co);
  S->co = NULL;
  free(S);
}

/**
 * @brief create a new coroutine struct.
 *
 * @param [in] S
 * @param [in] func
 * @param [in] ud
 * @return struct coroutine*
 */
struct coroutine *_co_new(struct schedule *S, coroutine_func func, void *ud) {
  struct coroutine *co = malloc(sizeof(*co));
  co->func = func;
  co->ud = ud;
  co->sch = S;
  co->cap = 0;
  co->size = 0;
  co->status = COROUTINE_READY;
  co->stack = NULL;
  return co;
}

/**
 * @brief create a new coroutine in schedule S, and return its id
 *
 * @param [in] S scheduler
 * @param [in] func corotine function
 * @param [in] ud user define parameter
 * @return int
 */
int coroutine_new(struct schedule *S, coroutine_func func, void *ud) {
  struct coroutine *co = _co_new(S, func, ud);
  if (S->nco >= S->cap) {
    // if the coroutine num in S is larger than the capacity of S, then we need
    // to expand the cap. .
    int id = S->cap;
    S->co = realloc(S->co, S->cap * 2 * sizeof(struct coroutine *));
    memset(S->co + S->cap, 0, sizeof(struct coroutine *) * S->cap);
    S->cap *= 2;
    S->co[S->cap] = co;
    S->nco = S->nco + 1;
    return id;
  } else {
    // if the coroutine num in S is less than the capacity of S, then we can
    // directly use the next available slot.
    int i = 0;
    for (int i = 0; i < S->cap; i++) {
      int id = (i + S->nco) % S->cap;
      if (S->co[id] == NULL) {
        S->co[id] = co;
        S->nco = S->nco + 1;
        return id;
      }
    }
  }
  assert(0);
  return -1;
}

static void mainfunc(uint32_t low32, uint32_t high32) {
  uintptr_t ptr = (uintptr_t)low32 | ((uintptr_t)high32 << 32);
  struct schedule *S = (struct schedule *)ptr;
  int id = S->running;
  struct coroutine *co = S->co[id];
  co->func(S, co->ud);
  _delete_co(co);
  S->co[id] = NULL;
  S->nco = S->nco - 1;
  S->running = -1;
}

/**
 * @brief resume coroutine with the co_id
 *
 * @param [in] S
 * @param [in] id
 */
void coroutine_resume(struct schedule *S, int id) {
  assert(S->running == -1);
  assert(id >= 0 && id < S->cap);
  struct coroutine *co = S->co[id];
  if (co == NULL) return;
  if (co->status == COROUTINE_READY) {
    // get current context, and save it into co->ctx。
    getcontext(&co->ctx);
    co->ctx.uc_stack.ss_sp = S->stack;
    co->ctx.uc_stack.ss_size = STACK_SIZE;
    co->ctx.uc_link = &S->main;
    S->running = id;
    co->status = COROUTINE_RUNNING;
    uintptr_t ptr = (uintptr_t)S;
    makecontext(&co->ctx, (void (*)(void))mainfunc, 2, (uint32_t)ptr,
                (uint32_t)(ptr >> 32));
    swapcontext(&S->main, &co->ctx);
  } else if (co->status == COROUTINE_SUSPEND) {
    memcpy(S->stack + STACK_SIZE - co->size, co->stack, co->size);
    S->running = id;
    co->status = COROUTINE_RUNNING;
    swapcontext(&S->main, &co->ctx);
  } else {
    assert(0);
  }
  return;
}

/**
 * @brief get coroutine status
 *
 * @param [in] S
 * @param [in] id
 * @return int
 */
int coroutine_status(struct schedule *S, int id) {
  assert(id >= 0 && id < S->cap);
  if (S->co[id] == NULL) {
    return COROUTINE_DEAD;
  }
  return S->co[id]->status;
}

int coroutine_running(struct schedule *S) { return S->running; }

/**
 * @brief save current coroutine stack
 *
 * @param [in] C coroutine
 * @param [in] top current stack top
 */
static void _save_stack(struct coroutine *C, char *top) {
  char dummy = 0;
  assert(top - &dummy <= STACK_SIZE);
  if (C->cap < top - &dummy) {
    free(C->stack);
    C->cap = top - &dummy;
    C->stack = malloc(C->cap);
  } else {
    C->size = top - &dummy;
    memcpy(C->stack, &dummy, C->size);
  }
}

/**
 * @brief yield current coroutine
 *
 * @param [in] S
 */
void coroutine_yield(struct schedule *S) {
  int id = S->running;
  assert(id >= 0);
  struct coroutine *C = S->co[id];
  assert((char *)&C > S->stack);
  _save_stack(C, S->stack + STACK_SIZE);
  C->status = COROUTINE_SUSPEND;
  S->running = -1;
  swapcontext(&C->ctx, &S->main);
}