#pragma once
#pragma GCC diagnostic ignored "-Wdeclaration-after-statement"

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
#include <linux/uaccess.h>

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

#define VERIFY(x) do { \
    if (!(x)) { \
        printk(BUSTER_ERR "Zero value for " #x "\n"); \
        return -1; \
    } \
} while (0)

#define GET_MASK(start,end) ((~0ULL << (start)) & (~0ULL >> (64 - (end) - 1)))


extern struct dentry *dump_file, *dump_dir;
extern char log_buffer[LOG_SIZE];
#define N_VADDR 50
extern uint64_t vaddrs[N_VADDR];
extern size_t idx;


typedef uint64_t u64;
typedef int64_t i64;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint8_t u8;
typedef int8_t i8;
typedef size_t st;


void* mymalloc(st size);
void log_range(u64 start_ptr, u64 limit_ptr, char* msg);
u64 get_mapping(u64 addr, st size);
u64 is_repeat_in(u64 val, u64* arr, st size);
/* void setup_log(void); */ 
/* int buster_log(char *msg, int len); */
