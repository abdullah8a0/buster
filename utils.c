#include "utils.h"

struct dentry *dump_file, *dump_dir;
char log_buffer[LOG_SIZE] = {};
uint64_t vaddrs[N_VADDR] = {};
size_t idx = 0;


void* mymalloc(size_t size) {
  void *ptr = kmalloc(size, GFP_KERNEL);
  if (!ptr) {
    printk(BUSTER_ERR "Failed to allocate memory\n");
    return NULL;
  }
  return ptr;
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
