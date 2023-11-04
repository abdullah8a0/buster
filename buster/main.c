#include "utils.h"
// get capabilties of pci device

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Muhammad Abdullah <abdullah8a0@gmail.com>");
MODULE_DESCRIPTION("Dumping the IOMMU");

// Base address of enhanced pcie configuration mechanism
#define BASE 0x00000000e0000000

void freeall(void) {
  printk(BUSTER_INFO "freeing all\n");
  if (dump_file != NULL)
    debugfs_remove(dump_file);
  if (dump_dir != NULL)
    debugfs_remove(dump_dir);
  for (int i = 0; i < N_VADDR; i++) {
    if ((void *)vaddrs[i] != NULL)
      iounmap((void *)vaddrs[i]);
  }
}

/*
 * Map the configureation space of a pcie function
 */
void *map_conf(int bus, int dev, int func) {
  u64 phy_addr;
  u64 va_addr;

  phy_addr = BASE + (bus << 20 | dev << 15 | func << 12);

  va_addr = get_mapping(phy_addr, 4096);

  printk(BUSTER_INFO "%02x:%02x.%x -> 0x%016llx -> 0x%016llx\n", bus, dev, func,
         phy_addr, va_addr);
  return (void *)va_addr;
}

u32 *get_cap_space(int bus, int dev, int func) {
  u64 conf_space;
  u32 cap_off;
  u64 cap_ptr;
  u64 lim_ptr;
  u64 cap_ptr_init;

  conf_space = (u64)map_conf(bus, dev, func);

  if ((void *)conf_space == NULL) {
    printk(BUSTER_INFO "CONF VA: NULL\n");
    return NULL;
  }

  cap_off = *(u32 *)((u8 *)conf_space + 0x34);
  cap_off &= 0xff;
  cap_ptr = conf_space + cap_off;
  cap_ptr_init = cap_ptr;
  lim_ptr = cap_ptr + 0x18;

  while (cap_off != 0) {
    printk(BUSTER_INFO "capabilites %02x:%02x.%x + 0x%08x\n", bus, dev, func,
           cap_off);
    log_range(cap_ptr, lim_ptr, "capabilites");

    cap_off = *(u32 *)((u8 *)cap_ptr + 1);
    cap_off &= 0xff;
    cap_ptr = conf_space + cap_off;
    lim_ptr = cap_ptr + 0x18; // capability header has 0x0 to 0x14 dwords
  }

  printk(BUSTER_INFO "capabilites %02x:%02x.%x + 0x%08x\n", bus, dev, func,
         cap_off);
  log_range(cap_ptr, lim_ptr, "capabilites");

  return (u32 *)cap_ptr_init;
}

u64 get_iommu_regs(u64 cap_ptr) {
  u64 mmio_base;

  u32 base_addr_low = *((u32 *)cap_ptr + 1);
  u32 base_addr_high = *((u32 *)cap_ptr + 2);
  u64 base_addr = base_addr_low | ((u64)base_addr_high << 32);
  int enable = base_addr_low & 0x1;

  base_addr ^= 0x1;

  if (enable == 0) {
    printk(BUSTER_INFO "IOMMU area is disabled\n");
    return 0;
  }

  if (base_addr == 0) {
    return 0;
  }

  mmio_base = get_mapping(base_addr, 4096);
  printk(BUSTER_INFO "IOMMU VA: 0x%016llx <- 0x%016llx\n", mmio_base,
         base_addr);

  if ((void *)mmio_base == NULL) {
    printk(BUSTER_INFO "IOMMU VA: NULL\n");
    return 0;
  }

  log_range(mmio_base, mmio_base + 4096, "IOMMU BASE");

  return mmio_base;
}

// function to do stuff when the module is loaded
void bust_open(u64 arg1, u64 arg2) {
  u64 mmio_regs = arg1;

  if (mmio_regs == 0) {
    return;
  }

#if 0
  u64 devTblBaseReg = *((u64*)mmio_regs);  
  // size = [8:0]  // indicated is (n + 1) * 4 Kbytes. 
  // base = [51:12] // 4K aligned, 

  u64 devTblSize = devTblBaseReg & 0xff;
  devTblSize = (devTblSize + 1) * 4096;

  u64 devTblBase = devTblBaseReg & 0xfffffffffffff000;

  // print two phy addr from mapping and phy_to_virt
  printk(BUSTER_INFO "devTblBaseReg: 0x%016llx\n", devTblBaseReg);

  u64 devTblBase_v = (u64)phys_to_virt(devTblBase);
  printk(BUSTER_INFO "devTblBase: 0x%016llx <- 0x%016llx\n", devTblBase_v, devTblBase);
  /* log_range(devTblBase_v, devTblBase_v + devTblSize, "devTblBase"); */

#endif

  u64 comBaseReg = *((u64 *)mmio_regs + 1);
  // len = [59:56] // power of 2 increment, 1000 is 256 entries, 4kb
  // base = [51:12] // 4K aligned,

  u64 comTblSize = comBaseReg >> 56;
  comTblSize = (1 << comTblSize) * 4096;

  u64 comTblBase = comBaseReg & ((1ull << 51) - 1);

  printk(BUSTER_INFO "comBaseReg: 0x%016llx\n", comBaseReg);

  u64 comTblBase_v = (u64)phys_to_virt(comTblBase);
  printk(BUSTER_INFO "comTblBase_v: 0x%016llx <- 0x%016llx\n", comTblBase_v,
         comTblBase);
  log_range(comTblBase_v, comTblBase_v + 48 * 4096, "COM TBL");

#if 0

  u64 eventLogBaseReg = *((u64*)mmio_regs + 2);
  // len = [59:56] // power of 2 increment, 1000 is 256 entries, 4kb
  // base = [51:12] // 4K aligned,

  u64 eventLogSize = eventLogBaseReg >> 56;
  eventLogSize = (1 << eventLogSize) * 4096;

  u64 eventLogBase = eventLogBaseReg & ((1ull << 51) - 1);

  printk(BUSTER_INFO "eventLogBaseReg: 0x%016llx\n", eventLogBaseReg);
  
  u64 eventLogBase_v = (u64)phys_to_virt(eventLogBase);
  printk(BUSTER_INFO "eventLogBase_v: 0x%016llx <- 0x%016llx\n", eventLogBase_v, eventLogBase);
  /* log_range(eventLogBase_v, eventLogBase_v + 4*4096, "EVENT LOG"); */
#endif

}

#define NMIOMMU 4
static int __init buster_ini(void) {
  st busnums[NMIOMMU];
  u32 *cap_ptr;
  u32 cap_off;
  u64 mmio_base;

  busnums[0] = 0x00;
  busnums[1] = 0x40;
  busnums[2] = 0x80;
  busnums[3] = 0xc0;

  /* dump_to_user(buf, msg, 64); */

  for (int i = 0; i < NMIOMMU; i++) {
    if ((cap_ptr = get_cap_space(busnums[i], 0, 2)) == NULL) {
      printk(BUSTER_INFO "cap_ptr is NULL\n");
      continue;
    }

    mmio_base = get_iommu_regs((u64)cap_ptr);
    if (i == NMIOMMU - 1) 
      bust_open(mmio_base, 0);
   do {
      cap_off = *(u32 *)((u8 *)cap_ptr + 1);
      cap_off &= 0xff;
      cap_ptr = (u32 *)((u8 *)cap_ptr + cap_off);
      mmio_base = get_iommu_regs((u64)cap_ptr);
      /* bust_open(mmio_base, 0); */
    } while (cap_off != 0x00 && cap_off != 0xff);
  }

  freeall();
  return -1;
}

static void __exit buster_fini(void) {
  printk(BUSTER_INFO "buster unloaded\n");
  freeall();
}

module_init(buster_ini);
module_exit(buster_fini);
