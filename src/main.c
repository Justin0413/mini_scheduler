#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include "scheduler.h"

extern TCB *current_task;

void task_function(void *arg)
{
    int id = *(int *)arg;
    for (int i = 0; i < 2; i++)
    {
        printf("Task %d: 執行中 (Round %d)\n", id, i);
        printf("Hello world\n");
        schedule(); // 自願讓出 CPU
    }
    printf("Task %d: 執行完畢\n", id);
    current_task->state = TERMINATED;
    schedule();
}

int main()
{

    enqueue(create_task(1, task_function));
    enqueue(create_task(2, task_function));
    // enqueue(create_task(3, task_function));

    printf("啟動排程器...\n");
    start_scheduling();

    printf("所有任務結束，回到 Main。\n");
    return 0;
}