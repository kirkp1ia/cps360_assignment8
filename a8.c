#include <stdio.h>
#include <stdlib.h>


#define SETS 8192
#define LINES 4


/*
 * {bit_extractor, least_significant_bit_index}
 */
int ADDR_TAG[] = {0x1fff, 19};
int ADDR_SETINDEX[] = {0x1fff, 6};
int ADDR_OFFSET[] = {0x3f, 0};


struct cachentry {
  char valid;
  unsigned char age;
  int tag;
};
typedef struct cachentry cachentry;


int main(int argc, char *argv[]) {
  int cche_addentry(cachentry[], int, int);
  cachentry * initialize_cache();
  int address = 0x11100030, address2 = 0x15100030, added;

  cachentry * cache = initialize_cache();

  added = cche_addentry(cache, address, 2);
  added = cche_addentry(cache, address2, 2);

  printf("Added: %d; Tag: %06x\n", added, cache[2].tag);

  exit(0);
}


/* Problem: Create an array of cachentry structs that have an age of 0, tag of
 * 0, and valid of 0.
 *
 * Returns: the array of cachentry structs with empty slots.
 *
 * Solution: Create the array using malloc, then iterate over each item and set
 * every member of every entry to 0. If an entry has an age of 0, it is assumed
 * to be an empty entry slot.
 */
cachentry * initialize_cache() {
  int i = 0;
  cachentry *cache = malloc(SETS * LINES * sizeof(cachentry));
  for (; i < (SETS * LINES); i ++) {
    cache[i].valid = 0x00;
    cache[i].age = 0x00;
    cache[i].tag = 0;
  }
  return cache;
}


/* Problem: Return the set corresponding to address. Use the 6th - 18th bits in
 * address to find the set.
 *
 * Returns: the set of cachentry's that corresponds to address.
 *
 * Solution: Find the index in cache of the first element in the set. Return a
 * pointer to this entry as an array of entries starting with the first element
 * in the set.
 *
 * Limitations: The set will contain all entries after the first element, not
 * just the four belonging to it. Funtions using this must only loop 4 elements
 * at a time to maintain consistancy in cache size.
 */
cachentry * cche_getset(cachentry cache[], int address) {
  int setindex;
  setindex = (address >> ADDR_SETINDEX[1]) & ADDR_SETINDEX[0];
  return (cachentry *)(cache + (LINES * setindex * sizeof(cachentry)));
}


/* Problem: Add an address referencing data in memory to the cache. This address
 * will be put into the set indicated by the set bits in the address at bits
 * 6 - 18. It will be put into the set at the offset indicated by offset. This
 * function assumes that the slot in the set at offset is empty. If this
 * condition is not met, 0 is returned.
 *
 * Returns: 1 if entry is added, 0 otherwise.
 *
 * Solution: Find the set indicated by the 6th - 18th bits. Then get the entry
 * in that set at the slot indicated by offset. If this entry is empty, add a
 * new entry for address at that slot. Use initialize_cache_entry() to create
 * the entry object. If this entry is not empty, do nothing and return 0.
 */
int cche_addentry(cachentry cache[], int address, int offset) {
  cachentry * cche_getset(cachentry[], int);
  cachentry * initialize_cache_entry(int);
  cachentry *set, *entry_toadd;

  set = cche_getset(cache, address);

  if (set[offset].age == 0) {
    entry_toadd = initialize_cache_entry(address);
    set[offset] = *entry_toadd;
    return 1;
  }
  return 0;
}


/* Problem: Create a cache entry struct to contain information of address.
 *
 * Returns: A cachentry pointer that contains information in address (tag,
 * valid, age).
 *
 * Solution: use addr_gettag() to isolate the tag from address and set it as the
 * entry's tag. Set age to 1 to show that it is the most recent entry. The
 * oldest entry in a set will have an age of 4.
 */
cachentry * initialize_cache_entry(int address) {
  int addr_gettag(int);
  cachentry * entry = malloc(sizeof(cachentry));
  entry->age = 0x01;
  entry->valid = 0x01;
  entry->tag = addr_gettag(address);
  return entry;
}


/* Problem: Return the entry in the cache that has a tag identical to that
 * in the tag of address. This function only looks in the set that corresponds
 * to the 6th - 18th bits in address.
 *
 * Returns: The entry matching address by the tag in cache and set bits in
 * address. Null if nothing matches (Signifies a miss).
 *
 * Solution: Isolate the tag from address and the set from the cache. Look at
 * the four blocks in the set and compare each blocks tag with the tag in
 * address. If the two match, return the location of the entry as a pointer. If
 * no entry matches address by tag, NULL is returned and it can be assumed that
 * the query yeilded a miss.
 */
cachentry * cche_getentry(cachentry cache[], int address) {
  cachentry * cche_getset(cachentry[], int);
  int addr_gettag(int);
  int blockindex = 0, addr_tag;
  cachentry * set, crnt_entry;

  addr_tag = addr_gettag(address);
  set = cche_getset(cache, address);

  for (; blockindex < LINES; blockindex ++) {
    crnt_entry = set[blockindex];
    if (crnt_entry.tag == addr_tag) {
      return (cachentry *)(set + blockindex);
    }
  }

  return NULL;
}


/* Problem: Return the tag of address isolated from the rest of address at the
 * lowest 13 bits of an int.
 *
 * Returns: The tag contained in address
 *
 * Solution: Shift all bits in address right to get the tag's least significant
 * bit as the least significant bit of address. Then extract the first 13 bits
 * from that integer to isolate the tag.
 */
int addr_gettag(int address) {
  return (address >> ADDR_TAG[1]) & ADDR_TAG[0];
}
