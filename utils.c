#include "utils.h"

struct dentry *dump_file, *dump_dir;
char log_buffer[LOG_SIZE] = {};
uint64_t vaddrs[N_VADDR] = {};
size_t idx = 0;

void log_range(uint64_t capability_ptr, uint64_t limit_ptr, int is64) {
  // log all non zero dwords
  if (is64) {
    for(uint64_t i = capability_ptr; i < limit_ptr; i += 8){
      uint64_t qword = *((uint64_t*)i);
      if(qword == 0)
        continue;
      printk(BUSTER_INFO "%llx: %016llx\n", i - capability_ptr, qword);
      PRINT_BIN64(qword);
    }
  } else {
    for(uint64_t i = capability_ptr; i < limit_ptr; i += 4){
      uint32_t dword = *((uint32_t*)i);
      if(dword == 0)
        continue;
      printk(BUSTER_INFO "%llx: %08x\n", i - capability_ptr, dword);
    }
  }
}


void* mymalloc(size_t size) {
  void *ptr = kmalloc(size, GFP_KERNEL);
  if (!ptr) {
    printk(BUSTER_ERR "Failed to allocate memory\n");
    return NULL;
  }
  return ptr;
}

void log_addr(uint64_t addr, uint len){
  printk(BUSTER_INFO "VA: 0x%016llx\n", addr);
  if((void*)addr == NULL){
    printk(BUSTER_INFO "VA: NULL\n");
    return;
  }
  if(len > 256){
    printk(BUSTER_INFO "len is too big\n");
    return;
  }

  for(uint i = 0; i < len; i++){
    printk(BUSTER_INFO "%x: %08x\n", 4*i, *((uint32_t*)addr + i));
  }
}

// gets mapping via ioremap and adds the mapping to vaddrs
uint64_t get_mapping(uint64_t addr, size_t size){
  uint64_t ptr;
  if (idx > N_VADDR) 
    return 0;

  ptr = (uint64_t) ioremap(addr, size);
  vaddrs[idx] = ptr;
  idx++;

  return ptr;
}

// dumps a page of memory in base 16
void dump_pg(uint8_t *addr){ 
}

/* read file operation */
/* static ssize_t myreader(struct file *fp, char __user *user_buffer, */ 
/*     size_t count, loff_t *position) */ 
/* { */ 
/*    return simple_read_from_buffer(user_buffer, count, position, log_buffer, LOG_SIZE); */
/* } */ 

/* /1* write file operation *1/ */
/* static ssize_t mywriter(struct file *fp, const char __user *user_buffer, */ 
/*     size_t count, loff_t *position) */ 
/* { */ 
/*   if(count > LOG_SIZE) */
/*     return -EINVAL; */ 

/*   return simple_write_to_buffer(log_buffer, LOG_SIZE, position, user_buffer, count); */ 
/* } */

/* static const struct file_operations my_fops = { */
/*   .owner = THIS_MODULE, */
/*   .read = myreader, */
/*   .write = mywriter, */
/* }; */

/* void setup_log(void) { */
/*   dump_dir = debugfs_create_dir("buster", NULL); */
/*   if (!dump_dir) { */
/*     printk(BUSTER_ERR "Failed to create debugfs directory\n"); */
/*     return; */
/*   } */

/*   dump_file = debugfs_create_file(LOG_FILE, 0644, dump_dir, &log_buffer, NULL); */
/*   if (!dump_file) { */
/*     printk(BUSTER_ERR "Failed to create debugfs file\n"); */
/*   } */
/* } */

/* int buster_log(char *msg, int len) { */
/*   int ret; */

/*   if (!dump_file) { */
/*     setup_log(); */
  /* } */

  /* if (!dump_file) { */
  /*   printk(BUSTER_ERR "Failed to create debugfs file\n"); */
  /*   return -1; */
  /* } */

  /* if (len > LOG_SIZE) { */
  /*   printk(BUSTER_ERR "Log message too long\n"); */
  /*   return -1; */
  /* } */
  /* printk(BUSTER_INFO "Log message: %s\n", msg); */
  /* // write the msg by opening file */
  /* struct file *fp = filp_open(LOG_FILE, O_WRONLY, 0); */
  /* printk(BUSTER_INFO "fp = %p\n", fp); */
  /* if (fp < 0) { */
  /*   printk(BUSTER_ERR "Failed to open debugfs file\n"); */
  /*   return -1; */
  /* } */
  /* //ssize_t ret = kernel_write(fp, msg, len, 0); */
  /* ret = 0; */
  /* if (ret < 0) { */
  /*   printk(BUSTER_ERR "Failed to write to debugfs file\n"); */
  /*   return -1; */
  /* } */
  /* filp_close(fp, NULL); */
  /* printk(BUSTER_INFO "Log message written\n"); */

  /* return 0; */
/* } */
