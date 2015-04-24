#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

extern char *tracefile;


unsigned line;
unsigned distance;
int MAXLINE=256;

unsigned *DISTANCE;
unsigned file_pos=0;
addr_t *my_track;

/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
	//find the max distance in ptes through coremap
	int i;
	unsigned max = (coremap[0]).distance;
	int idx=0;
	for(i=0; i < memsize; i++){
		if((coremap[i]).distance > max){
			max = (coremap[i]).distance;
			idx = i;
		}
	}
	return idx;
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
 void opt_ref(pgtbl_entry_t *p) {
 	//calculate the distance
 	addr_t current_instruction = my_track[file_pos];
 	unsigned next = file_pos + 1;
 	while(next < line){
 		if(current_instruction != my_track[next]){
 			next++;
 		}else{
 			break;
 		}

 	}
 	if(next==line){
 		p->distance = line + 1;
 	}else{
 		p->distance = next;
 	}
 	file_pos++;
 }


/* Initializes any data structures needed for this
 * replacement algorithm.
 */
 void opt_init() {
 	FILE *infp=NULL;
 	line = 0;
 	char ch;
 	//open the line
 	if((infp = fopen(tracefile, "r")) == NULL) {
 		perror("Error opening tracefile:");
 		exit(1);
 	}
 	//calculate the lines in the file
 	while((ch=fgetc(infp))!=EOF){
 		if (ch=='\n'){
 			line++;
 		}
 	}
 	//allocate the memory for the arrays
 	DISTANCE = malloc( memsize * sizeof(unsigned int));
 	my_track = malloc( line * sizeof(addr_t));
 	//read the instructions
 	unsigned j=0;
 	char buf[MAXLINE];
 	addr_t vaddr = 0;
 	char type;
 	char *test = fgets(buf, MAXLINE, infp);

 	while(fgets(buf, MAXLINE, infp) != NULL) {
 		if(buf[0] != '=') {
 			sscanf(buf, "%c %lx", &type, &vaddr);
 			my_track[j] = vaddr;
 			j++;
			if(debug)  {
				printf("%c %lx\n", type, vaddr);
			}
 		} else {
 			continue;
 		}
 	}


 	fclose(infp);
 }

