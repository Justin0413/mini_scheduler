#include <stdio.h>
#include <stdlib.h>
#include "task.h"

TCB *head = NULL; // Ready Queue 的頭
TCB *current_task = NULL;
ucontext_t ctx_main;

// 加入任務到 Queue 尾端
void enqueue(TCB *task)
{
    if (!head)
    {
        head = task;
    }
    else
    {
        TCB *temp = head;
        while (temp->next)
            temp = temp->next;
        temp->next = task;
    }
    task->next = NULL;
}

// 從 Queue 頭部取出任務
TCB *dequeue()
{
    if (!head)
        return NULL;
    TCB *task = head;
    head = head->next;
    return task;
}

void schedule()
{
    TCB *next_task = dequeue();
    if (next_task)
    {
        TCB *prev_task = current_task;
        current_task = next_task;

        // 如果目前有任務在跑，把它塞回 Queue 尾巴以便下次輪轉
        if (prev_task && prev_task->state != TERMINATED)
        {
            enqueue(prev_task);
        }

        // 切換上下文
        if (prev_task)
        {
            swapcontext(&prev_task->context, &next_task->context);
        }
        else
        {
            swapcontext(&ctx_main, &next_task->context);
        }
    }
}