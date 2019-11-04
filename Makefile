
obj-m			:= nontrivial-rootkit.o

nontrivial-rootkit-y	+= src/nontrivial-rootkit.o
nontrivial-rootkit-y	+= src/hooking.o

ccflags-y		:= -I$(PWD)/src -I$(PWD)/src/inc

all:
			make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
			make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
