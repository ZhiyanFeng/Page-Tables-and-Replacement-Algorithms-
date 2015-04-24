#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	
	int vict;
	static int clock_p = 0; 
	pgtbl_entry_t *u;	
	int condition=1;	
	while (condition){
	 	u = (pgtbl_entry_t *) coremap[clock_p].pte;	
		if (!(u->frame & PG_REF)){
			vict = clock_p;			
			condition = 0;		
		}
		else 
			u->frame = u->frame & ~PG_REF;
		clock_p++;			
		clock_p = clock_p % memsize;
		
	} 	
	return vict;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {

	return;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
}
