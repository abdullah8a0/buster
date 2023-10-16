

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
#include <linux/pci.h>
#include <linux/version.h>

#define BUSTER_INFO KERN_INFO "[buster] "
#define BUSTER_ERR KERN_ERR "[buster] "

#define LOG_SIZE 1024
#define LOG_FILE "buster.log"
#define LOG_PATH "/sys/kernel/debug/buster/" LOG_FILE

// print space every 4 bits
#define PRINT_BIN64(x) do { \
    int i; \
    int idx = 0; \
    char bin[65 + 16] = {0}; \
    for (i = 63; i >= 0; i--) { \
        bin[idx++] = '0' + ((x >> i) & 1); \
        if (i % 4 == 0) \
            bin[idx++] = ' '; \
    } \
    printk(BUSTER_INFO #x " = %s\n", bin); \
} while (0)


extern struct dentry *dump_file, *dump_dir;
extern char log_buffer[LOG_SIZE];
#define N_VADDR 50
extern uint64_t vaddrs[N_VADDR];
extern size_t idx;


void* mymalloc(size_t size);
void log_range(uint64_t capability_ptr, uint64_t limit_ptr, int is64); 
void log_addr(uint64_t addr, uint len);
void dump_pg(uint8_t *addr);
uint64_t get_mapping(uint64_t addr, size_t size);
/* void setup_log(void); */ 
/* int buster_log(char *msg, int len); */
