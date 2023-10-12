

#pragma once
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
// open and close files
#include <linux/version.h>

#define BUSTER_INFO KERN_INFO "[buster] "
#define BUSTER_ERR KERN_ERR "[buster] "

#define LOG_SIZE 1024
#define LOG_FILE "buster.log"
#define LOG_PATH "/sys/kernel/debug/buster/" LOG_FILE


extern struct dentry *dump_file, *dump_dir;
extern char log_buffer[LOG_SIZE];
#define N_VADDR 8
extern uint64_t vaddrs[N_VADDR];
extern size_t idx;


void* mymalloc(size_t size);
/* void setup_log(void); */ 
/* int buster_log(char *msg, int len); */
