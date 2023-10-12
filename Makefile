
buster-objs := main.o utils.o
obj-m += buster.o


#INCLUDES := 
#IGNORE_WARNINGS :=

#EXTRA_CFLAGS += $(INCLUDES) $(IGNORE_WARNINGS)
#EXTRA_LDFLAGS += $(IGNORE_WARNINGS)

all: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

comp:
	gcc -o buster buster.c -Wall -Wextra -Werror -pedantic -std=c99


clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
