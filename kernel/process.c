#include "process.h"
#include "../utils/mem.h"
#include "../utils/string.h"
#include "../utils/console.h"

static process_t *current = 0;
static int next_pid = 1;

void process_init(void) {
    current = 0;
    next_pid = 1;
}

int process_create(void (*entry)()) {
    process_t *p = (process_t *)malloc(sizeof(process_t));
    p->pid = next_pid++;
    p->stack = (u8 *)malloc(4096);
    u32 stack_top = (u32)p->stack + 4096;
    memset((u8 *)&p->regs, 0, sizeof(registers_t));
    p->regs.eip = (u32)entry;
    p->regs.esp = stack_top;
    p->regs.ebp = stack_top;
    p->regs.cs = 0x08;
    p->regs.ds = 0x10;
    p->regs.ss = 0x10;
    p->regs.eflags = 0x202;

    if (!current) {
        current = p;
        p->next = p;
    } else {
        p->next = current->next;
        current->next = p;
    }
    return p->pid;
}

void schedule(registers_t *r) {
    if (!current) return;

    memcpy((u8 *)&current->regs, (u8 *)r, sizeof(registers_t));
    current = current->next;
    memcpy((u8 *)r, (u8 *)&current->regs, sizeof(registers_t));
}

