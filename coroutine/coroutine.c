#include "coroutine.h"

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
 * @brief 关闭协程调度器，释放内存。注意需要关闭每个协程对象。
 *
 * @param S 协程调度器， 指针类型
 */
void coroutine_close(struct schedule *S) {
  int i = 0;
  for (int i = 0; i < S->cap; i++) {
    struct *co = S->co[i];
    if (co) {
      delete_co(co);
    }
  }
  return;
}

int coroutine_new(struct schedule *, coroutine_func, void *ud);
void coroutine_resume(struct schedule *, int id);
int coroutine_status(struct schedule *, int id);
int coroutine_running(struct schedule *);
void coroutine_yield(struct schedule *);