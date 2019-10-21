#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/dirent.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Seb and Gibble");
MODULE_DESCRIPTION("A non-trivial kernel rootkit");
MODULE_VERSION("0.0.1");

#define CLASS_NAME "nontrivial class"
#define DEVICE_NAME "nontrivial-rootkit"

/* prototypes */
static int __init nontrivial_init(void);
static void __exit nontrivial_exit(void);
static int nontrivial_open(struct inode *, struct file *);
static int nontrivial_close(struct inode *, struct file *);
static ssize_t nontrivial_write(struct file*, const char *, size_t, loff_t *);
static ssize_t nontrivial_read(struct file*,  char *, size_t, loff_t *);
/* global variables */
static int majorNum;
static struct class *devClass = NULL;
static struct device *devStruct = NULL;
int module_is_hidden  = 0;

/* syscall variables */
unsigned long *syscall_table;
//sys_getdents_t getDents_original = NULL;

static struct file_operations fops = {
	.read = nontrivial_read,
	.write = nontrivial_write,
	.open = nontrivial_open,
	.release = nontrivial_close,
};

/* functions */
static int __init nontrivial_init(void){
	majorNum = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNum < 0){
		//Unable to register
		printk(KERN_ALERT "Failed to register chrdev\n");
		return majorNum;
	}
	devClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(devClass)){
		//failed to create class, cleanup and return
		return -1; //need to make this better
	}
	devStruct = device_create(devClass, NULL, MKDEV(majorNum, 0), NULL, DEVICE_NAME);
	if (IS_ERR(devStruct)){
		//failed to create device, cleanup
		class_destroy(devClass);
		unregister_chrdev(majorNum, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create device\n");
		return -1; //fix this
	}
	// get syscall table
	printk(KERN_INFO "Successfully registeded module\n");
	return 0;
}
static void __exit nontrivial_exit(void){
	device_destroy(devClass, MKDEV(majorNum,0));
	class_unregister(devClass);
	class_destroy(devClass);
	unregister_chrdev(majorNum, DEVICE_NAME);
}

static int nontrivial_open(struct inode *inode, struct file *f){
	printk(KERN_INFO "device opened\n");
	return 0;
}
static int nontrivial_close(struct inode *inode, struct file*f){
	printk(KERN_INFO "device closed\n");
	return 0;
}
static ssize_t nontrivial_write(struct file *f, const char *buff, size_t len, loff_t *off){
	printk(KERN_INFO "write to device\n");
	return len;

}
static ssize_t nontrivial_read(struct file*f, char *buff, size_t length, loff_t *offset){
	printk(KERN_INFO "read from device\n");
	return 0;
}
module_init(nontrivial_init);
module_exit(nontrivial_exit);
