#include "syscall.h"
#include "../utils/console.h"
#include "../utils/mem.h"

#define MAX_SYSCALLS 256
static syscall_func syscalls[MAX_SYSCALLS];

void syscall_init(void) {
    for (int i = 0; i < MAX_SYSCALLS; i++)
        syscalls[i] = 0;
}

void register_syscall(int num, syscall_func func) {
    if (num < MAX_SYSCALLS)
        syscalls[num] = func;
}

void syscall_handler(registers_t *r) {
    if (r->eax < MAX_SYSCALLS && syscalls[r->eax]) {
        syscall_func func = syscalls[r->eax];
        func(r);
    }
}

void syscall_print(registers_t *r) {
    char *str = (char *)r->ebx;
    console_pre_print(str);
}

int syscall_do(int num, u32 arg0) {
    int ret;
    asm volatile ("int $0x80" : "=a" (ret) : "a" (num), "b" (arg0));
    return ret;
}

