
obj-m			:= nontrivial-rootkit.o

nontrivial-rootkit-y	+= src/nontrivial-rootkit.o

ccflags-y		:= -I$(PWD)/src -I$(PWD)/src/incl

all:
			make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
			make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
