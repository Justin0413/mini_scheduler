#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include "scheduler.h"

static inline unsigned long long rdtsc(void)
{
    unsigned int lo, hi;
    // 使用 asm 指令讀取
    // "a" 表示 eax, "d" 表示 edx
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return ((unsigned long long)hi << 32) | lo;
}
extern int total_active_tasks;

TCB *head = NULL; // Ready Queue 的頭
TCB *tail = NULL;
TCB *cleanup_queue = NULL; // 專門存放已結束、等著被釋放的任務
TCB *current_task = NULL;
ucontext_t ctx_main;
TCB *global_idle_task = NULL;

TCB *create_task(int id, void (*func)())
{
    TCB *task = malloc(sizeof(TCB));
    task->id = id;
    task->state = READY;
    task->stack = malloc(STACK_SIZE);
    getcontext(&task->context);
    task->context.uc_stack.ss_sp = task->stack;
    task->context.uc_stack.ss_size = STACK_SIZE;
    task->context.uc_link = NULL;
    makecontext(&task->context, func, 1, &task->id);
    return task;
}

// 加入任務到 Queue 尾端
void enqueue(TCB *task)
{
    task->next = NULL;
    if (!tail)
    {
        head = tail = task;
    }
    else
    {

        tail->next = task;
        tail = task;
    }
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
    unsigned long long start, end;
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
        if (prev_task && prev_task->state == TERMINATED)
        {
            // 將該任務加入待清理清單
            prev_task->next = cleanup_queue;
            cleanup_queue = prev_task;
        }
        // 切換上下文
        if (prev_task)
        {
            start = rdtsc(); // 記錄開始時間
            swapcontext(&prev_task->context, &next_task->context);
            end = rdtsc(); // 記錄結束時間

            // 這次切換耗費的 Cycles
            printf("Context Switch Latency: %llu cycles\n", end - start);
        }
        else
        {

            swapcontext(&ctx_main, &next_task->context);
        }
    }
}

void idle_task_function()
{
    int count;
    while (count < 2)
    {

        // 檢查是否有任務需要回收
        while (cleanup_queue != NULL)
        {
            TCB *to_free = cleanup_queue;
            cleanup_queue = cleanup_queue->next;

            printf("Idle Task: 正在回收 Task %d 的資源...\n", to_free->id);

            free(to_free->stack); // 釋放stack空間
            free(to_free);        // 釋放 TCB 結構體
            count++;
        }
        // 讓出 CPU，給其他任務機會
        schedule();
    }
    setcontext(&ctx_main);
}

void start_scheduling()
{

    // 儲存 main 的位置，等一下 schedule() 發現沒任務時可以跳回來
    getcontext(&ctx_main);

    // 自動幫使用者建立優先權最低的 Idle Task
    global_idle_task = create_task(0, (void *)idle_task_function);
    enqueue(global_idle_task);

    // 開始第一次排程
    schedule();
}

void scheduler_cleanup()
{

    if (global_idle_task)
    {
        if (global_idle_task->stack)
        {
            free(global_idle_task->stack);
        }
        free(global_idle_task);
        global_idle_task = NULL; // 清空指標
    }
}
