#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


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
  int get_oldest(cachentry[]);
  cachentry * cche_getset(cachentry[], int);
  void printset(cachentry[], int);

  int cche_addentry_indexed(cachentry[], int, int);
  cachentry * initialize_cache();
  int address[] = {
    0x11100030,
    0x12100030,
    0x13100030,
    0x14100030,
    0x15100030,
    0x16100030,
    0x17100030,
    0x18100030,
  },
  added;

  cachentry * cache = initialize_cache();

  srand(time(NULL));

  added = cche_addentry_indexed(cache, address[0], 2);
  added = cche_addentry_indexed(cache, address[1], 1);
  added = cche_addentry_indexed(cache, address[2], 0);
  added = cche_addentry_indexed(cache, address[3], 3);
  added = cche_addentry_indexed(cache, address[4], 1);
  added = cche_addentry_indexed(cache, address[5], 1);
  added = cche_addentry_indexed(cache, address[6], 1);

  printf("%s\n", argv[1]);
  printf("Added: %d; Tag: %06x\n", added, cache[2].tag);
  printf("Oldest: %d\n", get_oldest(cche_getset(cache, address[0])));
  printset(cache, 0);

  exit(0);
}


void printset(cachentry cache[], int setindex) {
  char * entrytos(cachentry);
  int i = setindex * LINES;

  printf("{\n");
  for (; i < ((setindex+1) * LINES); i ++) {
    printf("%s\n", entrytos(cache[i]));
  }
  printf("}\n");
}


char * entrytos(cachentry entry) {
  char validstr[3], agestr[3], tagstr[9],
  validlabel[] = "\t{\n\t\tValid: ",
  agelabel[] = "\n\t\tAge: ",
  taglabel[] = "\n\t\tTag: ",
  end[] = "\n\t},\n",
  *fullstr;

  fullstr = malloc(75*sizeof(char));

  sprintf(validstr, "%02x", entry.valid);
  sprintf(agestr, "%02x", entry.age);
  sprintf(tagstr, "%08x", entry.tag);

  strcat(fullstr, validlabel);
  strcat(fullstr, validstr);

  strcat(fullstr, agelabel);
  strcat(fullstr, agestr);

  strcat(fullstr, taglabel);
  strcat(fullstr, tagstr);

  strcat(fullstr, end);

  return fullstr;
}


void getinput(int argc, char *argv[], int *arr_addr) {

}


/* Problem: Lookup in the cache a given address. If the address is not found as
 * an entry, it is assumed that a miss occured. If the address is found, then it
 * is assumed that a hit occured. The following will happen in each event.
 *
 * HIT:
 *    return 0
 * MISS:
 *    update the cache with address. If needed, replace an existing entry in the
 *    set corresponding to address. The value in methodology determines which
 *    entry will be replaced.
 *    return 1
 *
 * Returns: 1 if a miss occured. 0 if a hit occured.
 *
 * Solution: Use cche_getentry to find the entry that contains address. If no
 * entry is found, then a miss occured and cche_update is called to replace an
 * entry with a new entry which is populated with the data in address. The
 * methodology is passed to the update function (cche_update) and the method of
 * finding the index is done in that function. If a miss occured, after the
 * update function is called, 1 is returned. If a hit occured, then nothing is
 * done except for returning 0.
 */
int querycache(cachentry cache[], int address, int methodology) {
  cachentry * cche_getentry(cachentry[], int);
  void cche_update(cachentry[], int, int);
  cachentry * entry;

  entry = cche_getentry(cache, address);
  if (entry == NULL) {
    cche_update(cache, address, methodology);
    return 1;
  }
  return 0;
}


/* Problem: Return the index of the oldest entry in the set based on the age of
 * each entry. The oldest entry will be returned.
 *
 * Returns: the index of the oldest entry in the set or the first empty slot in
 * the set.
 *
 * Solution: First, check to see if the set contains any empty slots. If so,
 * return the first empty one. Otherwise, set the oldest entry to the first
 * entry in the set. Then, iterate through the set. Upon each entry in
 * iteration, if its age is older than the entry at oldest_index at the time,
 * reset oldest_index to the current iteration index. After the loop is done,
 * return the value of oldest_index.
 */
int get_oldest(cachentry set[]) {
  int empties(cachentry[]);
  int blockindex = 0, oldest_index = 0, empty_signifier;

  empty_signifier = empties(set);
  if (empty_signifier > -1) {
    return empty_signifier;
  }

  for (; blockindex < LINES; blockindex ++) {
    if (set[blockindex].age > set[oldest_index].age) {
      oldest_index = blockindex;
    }
  }

  return oldest_index;
}


/* Problem: Return the index of the oldest entry in the set based on a random
 * number between 0 and the size of each set (LINES).
 *
 * Returns: the index of the entry in the set by random to replace or the first
 * empty slot in the set.
 *
 * Solution: First, check to see if the set contains any empty slots. If so,
 * return the first empty one. Otherwise, use rand() and mod it with LINES to
 * get a number between 0 and LINES. Return that value.
 */
int get_random(cachentry set[]) {
  int empties(cachentry[]);
  int randindex, empty_signifier;

  empty_signifier = empties(set);
  if (empty_signifier > -1) {
    return empty_signifier;
  }

  randindex = rand() % LINES;
  return randindex;
}


/* Problem: Return the index of the first empty slot in the set.
 *
 * Returns: The index of the first empty slot in the set. Returns -1 if all
 * slots are full.
 *
 * Solution: Loop through the first 4 indexes in set and, upon finding an empty
 * slot (the entry's age is 0), return that index. If the loop finishes, it is
 * assumed that no slot meets this condition and -1 is returned.
 */
int empties(cachentry set[]) {
  int i = 0;
  for (; i < LINES; i ++) {
    if (set[i].age == 0) {
      return i;
    }
  }
  return -1;
}


/* Problem: Replace the entry at the offset of set with the entry in
 * entry_toadd.
 *
 * Returns: 1 if the entry was successfuly added. 0 if otherwise.
 *
 * Solution: If offset would cause the entry to replace an entry not contained
 * in the set (that is, offset is >= the value of LINES), 0 is returned.
 * Otherwise, the data in set at offset is set to entry_toadd which will
 * any current entry contained in that slot.
 */
int replace_entry(cachentry set[], int offset, cachentry entry_toadd) {
  void incr_set_age(cachentry[], int);
  if (offset >= LINES) {
    set[offset] = entry_toadd;
    incr_set_age(set, offset);
    return 1;
  } else {
    return 0;
  }
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


/* Problem: Update the set that corresponds to address by overriting the data
 * held in a slot which this function will find itself in that set.
 *
 * Solution: Get the set corresponding to address by cche_getset(). According
 * to methodology, get the index of the slot in the set to replace, create a
 * cachentry instance, and replace the slot indicated by the slot index with the
 * new cachentry. The following values of methodology correspond to how the
 * index is found.
 *
 * 0 - (F)irst (I)n (F)irst (O)ut. Replaces the oldest entry.
 * 1 - Random. Replaces a random entry.
 *
 * NOTE: If an empty slot is available, it is replaced instead of one found by
 * the methods above. This logic, however, is in each methodology's function so
 * this function (cche_update) does not need to worry about that.
 */
void cche_update(cachentry cache[], int address, int methodology) {
  int get_oldest(cachentry[]), get_random(cachentry[]);
  cachentry * initialize_cache_entry(int);
  int replace_entry(cachentry[], int, cachentry);
  cachentry * cche_getset(cachentry[], int);
  cachentry *set;

  set = cche_getset(cache, address);
  if (methodology == 0)
    replace_entry(set, get_oldest(set), *(initialize_cache_entry(address)));
  else
    replace_entry(set, get_random(set), *(initialize_cache_entry(address)));
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
int cche_addentry_indexed(cachentry cache[], int address, int offset) {
  cachentry * cche_getset(cachentry[], int);
  cachentry * initialize_cache_entry(int);
  void incr_set_age(cachentry[], int);
  cachentry *set, *entry_toadd;

  set = cche_getset(cache, address);

  if (offset >= LINES) {
    return 0;
  }

  if (set[offset].age == 0) {
    entry_toadd = initialize_cache_entry(address);
    set[offset] = *entry_toadd;
    return 1;
  }
  return 0;
}


/* Problem: Increment the age of the first 4 entries in set by 1. Ignore
 * the entry at the index exclude because it's assumed that that entry has just
 * been added so it's age is correct. Just increment the rest of the entries
 *
 * Solution: Iterate through set and increment the age of the entry in each
 * iteration.
 */
void incr_set_age(cachentry set[], int exclude) {
  int i = 0;
  for (; i < LINES; i ++) {
    if (i != exclude && set[i].age > 0) {
      set[i].age ++;
    }
  }
}


/* Problem: Create a cache entry struct to contain information of address.
 *
 * Returns: A cachentry pointer that contains information in address (tag,
 * valid, age).
 *
 * Solution: use addr_gettag() to isolate the tag from address and set it as the
 * entry's tag. Set age to 1 to show that it is the most recent entry. The
 * oldest entry in a set will have an age of 4. However, we assume here that
 * when this entry is added, all entries in the set will have their age
 * incremented by 1, so this entry's age starts at 1 less than actual so that it
 * is correct after incrementation.
 *
 * Limitations: If the set is not incremented in age, this entry's age will be
 * 1 less than its actual age.
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
