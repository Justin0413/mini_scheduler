// task.h
#ifndef TASK_H
#define TASK_H

#include <ucontext.h>

typedef enum
{
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} TaskState;

typedef struct TCB TCB;

struct TCB
{
    int id;
    ucontext_t context;
    TaskState state;
    void *stack;
    struct TCB *next;
    int priority; // 未來擴充用
};

#endif