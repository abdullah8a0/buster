#include <linux/pci.h>

#include "utils.h"
// diable all error messages
// get capabilties of pci device

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Muhammad Abdullah <abdullah8a0@gmail.com>");
MODULE_DESCRIPTION("Dumping the IOMMU");

// Base address of enhanced configuration mechanism 
#define BASE 0x00000000e0000000

/*
 * Map the configureation space of a function
 */
void* map_conf(int bus, int dev, int func){
  uint64_t phy_addr;
  uint64_t va_addr;
 
  phy_addr = BASE + (bus << 20 | dev << 15 | func << 12);
  va_addr = ioremap(phy_addr, 4096);
  printk(BUSTER_INFO "%02x:%02x.%x -> 0x%016llx\n", bus, dev, func, phy_addr);
  printk(BUSTER_INFO "VA: 0x%016llx\n", va_addr);
  return (void*)va_addr;
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

void log_range(uint64_t capability_ptr, uint64_t limit_ptr){
  // log all non zero dwords

  for(uint64_t i = capability_ptr; i < limit_ptr; i += 4){
    uint32_t dword = *((uint32_t*)i);
    if(dword == 0)
      continue;
    printk(BUSTER_INFO "%x: %08x\n", i - capability_ptr, dword);
  }
}


// dumps a page of memory in base 16
void dump_pg(uint8_t *addr){ 
}

void freeall();

uint32_t* get_cap_space(int bus, int dev, int func){
  /* struct pci_dev *pdev = pci_get_domain_bus_and_slot(0, bus, PCI_DEVFN(dev, func)); */
  uint64_t conf_space;

  if (idx > N_VADDR) 
    return NULL;

  conf_space = map_conf(bus,dev,func);
  vaddrs[idx] = conf_space;
  idx++;
  
  uint32_t cap_off = *(uint32_t*) ((uint8_t*)conf_space + 0x34);
  cap_off &= 0xff;
  uint64_t cap_ptr = conf_space + cap_off;
  uint64_t lim_ptr = conf_space + 4096;

  printk(BUSTER_INFO "capabilites %02x:%02x.%x + 0x%08x\n", bus, dev, func, cap_off);
  log_range(cap_ptr, lim_ptr);

  return (uint32_t*)cap_ptr;
}

void get_iommu_regs(uint32_t cap_ptr) {
  uint32_t base_addr_low = *((uint32_t*)cap_ptr + 1);
  uint32_t base_addr_high = *((uint32_t*)cap_ptr + 2);
  uint64_t base_addr = base_addr_low | ((uint64_t)base_addr_high << 32);
  int enable = base_addr_low & 0x1;
  if(enable == 0){
    printk(BUSTER_INFO "IOMMU area is disabled\n");
    return;
  }
  printk(BUSTER_INFO "IOMMU base address: 0x%016llx\n", base_addr);
}

#define NMIOMMU 4

static int __init buster_ini(void){
  printk(BUSTER_INFO "buster loaded\n");
  
  size_t busnums[NMIOMMU] = {0x00, 0x40, 0x80, 0xc0};
  uint32_t* cap_ptr;
  for(int i = 0; i < NMIOMMU; i++){
    if ((cap_ptr = get_cap_space(busnums[i], 0, 2)) == NULL)
      return -1;
    get_iommu_regs(cap_ptr);
  }

  return -1;
}

static void __exit buster_fini(void){
  printk(BUSTER_INFO "buster unloaded\n");
  
  if(dump_file != NULL)
    debugfs_remove(dump_file);
  if(dump_dir != NULL)
    debugfs_remove(dump_dir);
  for(int i = 0; i < N_VADDR; i++){
    if((void*)vaddrs[i] != NULL)
      iounmap((void*)vaddrs[i]);
  } 
}

void freeall(){
  printk(BUSTER_INFO "freeing all\n");
  if(dump_file != NULL)
    debugfs_remove(dump_file);
  if(dump_dir != NULL)
    debugfs_remove(dump_dir);
  for(int i = 0; i < N_VADDR; i++){
    if((void*)vaddrs[i] != NULL)
      iounmap((void*)vaddrs[i]);
  } 
}


module_init(buster_ini);
module_exit(buster_fini);
