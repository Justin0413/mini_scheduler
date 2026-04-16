#include <stdio.h>
#include <stdlib.h>
#include "task.h"

// 宣告在 scheduler.c 裡的函數
void enqueue(TCB *task);
void schedule();
extern TCB *current_task;

void task_function(void *arg)
{
    int id = *(int *)arg;
    for (int i = 0; i < 3; i++)
    {
        printf("Task %d: 執行中 (Round %d)\n", id, i);
        schedule(); // 自願讓出 CPU
    }
    printf("Task %d: 執行完畢\n", id);
    current_task->state = TERMINATED;
    schedule();
}

TCB *create_task(int id, void (*func)())
{
    TCB *task = malloc(sizeof(TCB));
    task->id = id;
    task->state = READY;
    task->stack = malloc(8192);
    getcontext(&task->context);
    task->context.uc_stack.ss_sp = task->stack;
    task->context.uc_stack.ss_size = 8192;
    task->context.uc_link = NULL;
    makecontext(&task->context, func, 1, &task->id);
    return task;
}

int main()
{
    enqueue(create_task(1, (void *)task_function));
    enqueue(create_task(2, (void *)task_function));
    enqueue(create_task(3, (void *)task_function));

    printf("啟動排程器...\n");
    schedule();

    printf("所有任務結束，回到 Main。\n");
    return 0;
}