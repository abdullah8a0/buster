
#include "utils.h"
// diable all error messages
// get capabilties of pci device

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Muhammad Abdullah <abdullah8a0@gmail.com>");
MODULE_DESCRIPTION("Dumping the IOMMU");

// Base address of enhanced configuration mechanism 
#define BASE 0x00000000e0000000

void freeall(void){
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

/*
 * Map the configureation space of a function
 */
void* map_conf(int bus, int dev, int func){
  uint64_t phy_addr;
  uint64_t va_addr;
 
  phy_addr = BASE + (bus << 20 | dev << 15 | func << 12);

  va_addr = get_mapping(phy_addr, 4096);

  printk(BUSTER_INFO "%02x:%02x.%x -> 0x%016llx\n", bus, dev, func, phy_addr);
  printk(BUSTER_INFO "PCI CONF VA: 0x%016llx\n", va_addr);
  return (void*)va_addr;
}



uint32_t* get_cap_space(int bus, int dev, int func){
  /* struct pci_dev *pdev = pci_get_domain_bus_and_slot(0, bus, PCI_DEVFN(dev, func)); */
  uint64_t conf_space;
  uint32_t cap_off;
  uint64_t cap_ptr;
  uint64_t lim_ptr;
  uint64_t cap_ptr_init;

  conf_space = (uint64_t)map_conf(bus,dev,func);

  if ((void *)conf_space == NULL){
    printk(BUSTER_INFO "CONF VA: NULL\n");
    return NULL;
  }
  
  cap_off = *(uint32_t*) ((uint8_t*)conf_space + 0x34);
  cap_off &= 0xff;
  cap_ptr = conf_space + cap_off;
  cap_ptr_init = cap_ptr;
  lim_ptr = cap_ptr + 0x18;

  while(cap_off != 0){
    printk(BUSTER_INFO "capabilites %02x:%02x.%x + 0x%08x\n", bus, dev, func, cap_off);
    log_range(cap_ptr, lim_ptr, 0);

    cap_off = *(uint32_t*) ((uint8_t*)cap_ptr + 1);
    cap_off &= 0xff;
    cap_ptr = conf_space + cap_off; 
    lim_ptr = cap_ptr + 0x18; // capability header has 0x0 to 0x14 dwords
  }

  return (uint32_t*)cap_ptr_init;
}

void get_iommu_regs(uint64_t cap_ptr) {
  uint64_t mmio_base;
  uint64_t lim_ptr;

  uint32_t base_addr_low = *((uint32_t*)cap_ptr + 1);
  uint32_t base_addr_high = *((uint32_t*)cap_ptr + 2);
  uint64_t base_addr = base_addr_low | ((uint64_t)base_addr_high << 32);
  int enable = base_addr_low & 0x1;

  base_addr ^= 0x1;

  if(enable == 0){
    printk(BUSTER_INFO "IOMMU area is disabled\n");
    return;
  }

  if(base_addr == 0){
    return;
  }

  mmio_base = get_mapping(base_addr, 4096);
  printk(BUSTER_INFO "IOMMU VA: 0x%016llx <- 0x%016llx\n", mmio_base, base_addr);

  if((void*)mmio_base == NULL){
    printk(BUSTER_INFO "IOMMU VA: NULL\n");
    return;
  }

  lim_ptr = mmio_base + 4096;
  log_range(mmio_base, lim_ptr, 1);
}

#define NMIOMMU 4

static int __init buster_ini(void){
  size_t busnums[NMIOMMU];
  uint32_t* cap_ptr;
  uint32_t cap_off;

  printk(BUSTER_INFO "buster loaded\n");
  busnums[0] = 0x00;
  busnums[1] = 0x40;
  busnums[2] = 0x80;
  busnums[3] = 0xc0;

  for(int i = 0; i < NMIOMMU; i++){
    if ((cap_ptr = get_cap_space(busnums[i], 0, 2)) == NULL) {
      printk(BUSTER_INFO "cap_ptr is NULL\n");
      continue;
    }

    get_iommu_regs((uint64_t)cap_ptr);
    do {
      cap_off = *(uint32_t*) ((uint8_t*)cap_ptr + 1);
      cap_off &= 0xff;
      cap_ptr = (uint32_t*) ((uint8_t*)cap_ptr + cap_off);
      printk(BUSTER_INFO "capabilites %02lx:%02x.%x + 0x%08x\n", busnums[i], 0, 2, cap_off);
      get_iommu_regs((uint64_t)cap_ptr);
    } while (cap_off != 0x00 && cap_off != 0xff);
  }
  
  freeall();
  return -1;
}

static void __exit buster_fini(void){
  printk(BUSTER_INFO "buster unloaded\n");
  freeall();
}


module_init(buster_ini);
module_exit(buster_fini);
