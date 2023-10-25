#include "utils.h"
// incl popcount
#include <linux/bitops.h>

struct dentry *dump_file, *dump_dir;
char log_buffer[LOG_SIZE] = {};
u64 vaddrs[N_VADDR] = {};
st idx = 0;
st log_id = 0;

void log_range(u64 start_ptr, u64 limit_ptr, char *msg) {
  // prints quad words from start to lim.
  //
  // Before printing words, print BEGIN.[ID].[start_ptr].[msg]
  // After printing everything, print END.[ID]
  //
  // format is:
  // 1x[addr]: [val]
  //
  // if there is a repeated quad word k times then:
  // kx[addr]: [val]

  // check that msg is less than 64 chars and not null and no special chars
  for (st i = 0; i < 64; i++) {
    if (msg[i] == '\0') {
      break;
    }
    if (msg[i] == '\n' || msg[i] == '\r' || msg[i] == '\t') {
      printk(BUSTER_INFO "err:msg has special chars");
      return;
    }
    if (i == 63) {
      printk(BUSTER_INFO "err:msg too long");
      return;
    }
  }

  if (limit_ptr - start_ptr < 8) {
    printk(BUSTER_INFO "err:limit - start < 8");
    return;
  }

  if (start_ptr > limit_ptr) {
    printk(BUSTER_INFO "err:start > limit");
    return;
  }

  if (start_ptr % 8 != 0) {
    printk(BUSTER_INFO "err:start not aligned");
    return;
  }

  u64 prev_val = 0;
  st count = 0;

  char log_buffer[128] = {0};
  u64 delta = 0;

  prev_val =  *(u64 *)(start_ptr);

  printk(BUSTER_INFO "BEGIN.%lx.%llx.%s\n", log_id, start_ptr, msg);

  for (u64 *ptr = (u64 *)start_ptr + 1; ptr < (u64 *)limit_ptr; ptr++) {
    if (*ptr == prev_val) {
      count++;
      continue;
    }

    delta = (u64)ptr - (u64)start_ptr;
    if (count > 0) {
      snprintf(log_buffer, 127, "%lxx%llx: %llx\n", count + 1,
               (delta - 8 * (count + 1)), prev_val);
      printk(BUSTER_INFO "%s", log_buffer);
      count = 0;
    } else {
      snprintf(log_buffer, 127, "1x%llx: %llx\n", delta - 8, prev_val);
      printk(BUSTER_INFO "%s", log_buffer);
    }
    prev_val = *ptr;
  }

  snprintf(log_buffer, 127, "%lxx%llx: %llx\n", count + 1,
           ((u64)limit_ptr - (u64)start_ptr - 8 * (count + 1)), prev_val);
  printk(BUSTER_INFO "%s", log_buffer);

  printk(BUSTER_INFO "END.%lx\n", log_id);
  log_id++;
}

void *mymalloc(st size) {
  void *ptr = kmalloc(size, GFP_KERNEL);
  if (!ptr) {
    printk(BUSTER_ERR "Failed to allocate memory\n");
    return NULL;
  }
  return ptr;
}

// gets mapping via ioremap and adds the mapping to vaddrs
u64 get_mapping(u64 addr, st size) {
  u64 ptr;
  if (idx > N_VADDR)
    return 0;

  ptr = (u64)ioremap(addr, size);
  vaddrs[idx] = ptr;
  idx++;

  return ptr;
}
