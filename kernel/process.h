#ifndef PROCESS_H
#define PROCESS_H

#include "../cpu/types.h"
#include "../cpu/isr.h"

typedef struct process {
    int pid;
    registers_t regs;
    u8 *stack;
    struct process *next;
} process_t;

void process_init(void);
int process_create(void (*entry)());
void schedule(registers_t *r);

#endif
