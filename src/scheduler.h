#ifndef SCHEDULER_H // Header Guard: 防止重複引入導致編譯錯誤
#define SCHEDULER_H
#include <stdlib.h>

#define STACK_SIZE 1024 * 8 // 8KB Stack size

TCB *create_task(int id, void (*func)(void *));
void scheduler_cleanup();
void schedule(void);
void start_scheduling(void);
void enqueue(TCB *task);
TCB *dequeue();
#endif // SCHEDULER_H