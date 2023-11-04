#include <bits/stdc++.h>

#define MOD(n, m) (((n) % (m) + (m)) % (m))
#ifdef DEBUG
#define printf(fmt, ...)                                                       \
  do {                                                                         \
    fprintf(stdout, fmt, ##__VA_ARGS__);                                       \
    fflush(stdout);                                                            \
  } while (0)
#else
#define printf(fmt, ...)                                                       \
  do {                                                                         \
  } while (0)
#endif

using namespace std;

typedef uint64_t u64;
typedef int64_t i64;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint8_t u8;
typedef int8_t i8;
typedef size_t st;
typedef pair<int, int> pii;
typedef pair<i64, i64> pll;
typedef pair<double, double> pdd;
typedef pair<float, float> pff;
typedef vector<int> vi;
typedef vector<i64> vl;
typedef vector<double> vd;
typedef vector<float> vf;
typedef vector<bool> vb;
typedef vector<string> vs;

#include <fstream>

// reads /sys/firmware/acpi/tables/MCFG
// and returns the base address of the first PCI segment
// found in the MCFG table

u64 mcfg_r() {
  fstream mcfg("/sys/firmware/acpi/tables/MCFG", ios::in | ios::binary);
  for (int i = 0; i < 44; i++)
    mcfg.get();
  u64 base = 0;
  for (int i = 0; i < 8; i++) {
    u64 tmp = mcfg.get();
    base |= tmp << (i * 8);
  }
  u16 seg = 0;
  for (int i = 0; i < 2; i++) {
    u16 tmp = mcfg.get();
    seg |= tmp << (i * 8);
  }

  cout << "seg: " << hex << seg << endl;
  u8 bus_start = mcfg.get();
  u8 bus_end = mcfg.get();
  cout << "bus_start: " << hex << (int)bus_start << endl;
  cout << "bus_end: " << hex << (int)bus_end << endl;

  return base;
}

void setupIO(string = "");

int main(int argc, char *argv[]) {
  setupIO();
  u64 base = mcfg_r();
  // print in 0x0000... format
  char buf[64];
  sprintf(buf, "0x%016lx", base);
  cout << buf << endl;
}

void setupIO(string PROB) {
  ios_base::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  if (PROB != "") {
    freopen((PROB + ".in").c_str(), "r", stdin);
    freopen((PROB + ".out").c_str(), "w", stdout);
  }
}
