#include <stdio.h>
#include <stdlib.h>


#define SETS 8192
#define LINES 4


/*
 * {bit_extractor, least_significant_bit_index}
 */
#define ADDR_TAG = {0x1fff, 19}
#define ADDR_SETINDEX = {0x1fff, 6}
#define ADDR_OFFSET = {0x3f, 0}


struct cachentry {
  char valid;
  unsigned char age;
  int tag;
};
typedef struct cachentry cache;


int main(int argc, char *argv[]) {
  cache * initialize_cache();

  printf("%lu\n", sizeof(int));
  printf("%lu\n", sizeof(char));
  printf("%lu\n", sizeof(int) * 8);
  printf("%lu\n", sizeof(short int) * 8);

  exit(0);
}


cache * initialize_cache() {
  cache *l1 = malloc(SETS * LINES);
  return l1;
}


cache * cche_getset(cache cachelist[], int address) {
  int setindex;
  setindex = (address >> ADDR_SETINDEX[1]) & ADDR_SETINDEX[0];
  return (cache *)(cachelist + (LINES * setindex));
}


/* Problem: Return the entry in the cache that has a tag identical to that
 * in the tag of address.
 *
 */
cache cche_getentry(cache cachelist[], int address) {
  cache * cche_getset(cache[], int);
  int addr_gettag(int);
  int blockindex = 0, addr_tag;
  cache * set, crnt_entry;

  addr_tag = addr_gettag(address)
  set = cche_getset(cachelist, address);

  for (; blockindex < LINES; blockindex ++) {
    crnt_entry = set[blockindex];
    if (crnt_entry.tag == addr_tag) {
      return crnt_entry;
    }
  }

  return NULL;
}


/* Problem: Return the tag of address isolated from the rest of address at the
 * lowest 13 bits of an int.
 *
 */
int addr_gettag(int address) {
  return (address >> ADDR_TAG[1]) & ADDR_TAG[0];
}
