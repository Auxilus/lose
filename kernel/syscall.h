#ifndef SYSCALL_H
#define SYSCALL_H

#include "../cpu/isr.h"
#include "../cpu/types.h"

typedef void (*syscall_func)(registers_t *);

void syscall_init(void);
void register_syscall(int num, syscall_func func);
void syscall_handler(registers_t *r);

/* Example syscall implementations */
void syscall_print(registers_t *r);

int syscall_do(int num, u32 arg0);

#endif
