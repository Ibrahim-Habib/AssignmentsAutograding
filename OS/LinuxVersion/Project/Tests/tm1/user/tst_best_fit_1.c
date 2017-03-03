/* *********************************************************** */
/* MAKE SURE PAGE_WS_MAX_SIZE = 1000 */
/* *********************************************************** */
#include <inc/lib.h>

void _main(void)
{	
	int envID = sys_getenvid();

	volatile struct Env* myEnv;
	myEnv = &(envs[envID]);

	int Mega = 1024*1024;
	int kilo = 1024;
	void* ptr_allocations[20] = {0};
	int freeFrames ;
	int usedDiskPages;

	//[0] Make sure there're available places in the WS
	int w = 0 ;
	int requiredNumOfEmptyWSLocs = 50;
	int numOfEmptyWSLocs = 0;
	for (w = 0 ; w < PAGE_WS_MAX_SIZE ; w++)
	{
		if( myEnv->pageWorkingSet[w].empty == 1)
			numOfEmptyWSLocs++;
	}
	if (numOfEmptyWSLocs < requiredNumOfEmptyWSLocs)
		panic("InitialWSError1: Insufficient number of WS empty locations! please increase the PAGE_WS_MAX_SIZE");


	//[1] Allocate all
	{
		//Allocate 3 MB
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		ptr_allocations[0] = malloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[0] < (USER_HEAP_START) || (uint32) ptr_allocations[0] > (USER_HEAP_START + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		//if ((freeFrames - sys_calculate_free_frames()) != 256+1 ) panic("Wrong allocation: ");
		if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  3*256) panic("Wrong page file allocation: ");
		if ((freeFrames - sys_calculate_free_frames()) != 1 ) panic("Wrong allocation: ");

		//Allocate 3 MB
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		ptr_allocations[1] = malloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[1] !=  (USER_HEAP_START + 3*Mega) || (uint32) ptr_allocations[1] > (USER_HEAP_START+ 3*Mega + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		//if ((freeFrames - sys_calculate_free_frames()) != 256 ) panic("Wrong allocation: ");
		if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  3*256) panic("Wrong page file allocation: ");
		if ((freeFrames - sys_calculate_free_frames()) != 1 ) panic("Wrong allocation: ");

		//Allocate 2 MB
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		ptr_allocations[2] = malloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[2] !=  (USER_HEAP_START + 6*Mega) || (uint32) ptr_allocations[2] > (USER_HEAP_START+ 6*Mega + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		//if ((freeFrames - sys_calculate_free_frames()) != 256 ) panic("Wrong allocation: ");
		if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  2*256) panic("Wrong page file allocation: ");
		if ((freeFrames - sys_calculate_free_frames()) != 0 ) panic("Wrong allocation: ");

		//Allocate 2 MB
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		ptr_allocations[3] = malloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[3] != (USER_HEAP_START + 8*Mega) || (uint32) ptr_allocations[3] > (USER_HEAP_START + 8*Mega + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		//if ((freeFrames - sys_calculate_free_frames()) != 256 ) panic("Wrong allocation: ");
		if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  2*256) panic("Wrong page file allocation: ");
		if ((freeFrames - sys_calculate_free_frames()) != 1 ) panic("Wrong allocation: ");

		//Allocate 1 MB
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		ptr_allocations[4] = malloc(1*Mega-kilo);
		if ((uint32) ptr_allocations[4] !=  (USER_HEAP_START + 10*Mega) || (uint32) ptr_allocations[4] > (USER_HEAP_START + 10*Mega + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		//if ((freeFrames - sys_calculate_free_frames()) != 512 + 1) panic("Wrong allocation: ");
		if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  256) panic("Wrong page file allocation: ");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation: ");

		//Allocate 1 MB
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		ptr_allocations[5] = malloc(1*Mega-kilo);
		if ((uint32) ptr_allocations[5] != (USER_HEAP_START + 11*Mega) || (uint32) ptr_allocations[5] > (USER_HEAP_START + 11*Mega + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		//if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: ");
		if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  256) panic("Wrong page file allocation: ");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation: ");

		//Allocate 1 MB
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		ptr_allocations[6] = malloc(1*Mega-kilo);
		if ((uint32) ptr_allocations[6] != (USER_HEAP_START + 12*Mega) || (uint32) ptr_allocations[6] > (USER_HEAP_START + 12*Mega + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		//if ((freeFrames - sys_calculate_free_frames()) != 768 + 1) panic("Wrong allocation: ");
		if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  256) panic("Wrong page file allocation: ");
		if ((freeFrames - sys_calculate_free_frames()) != 1) panic("Wrong allocation: ");

		//Allocate 1 MB
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		ptr_allocations[7] = malloc(1*Mega-kilo);
		if ((uint32) ptr_allocations[7] != (USER_HEAP_START + 13*Mega) || (uint32) ptr_allocations[7] > (USER_HEAP_START + 13*Mega + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		//if ((freeFrames - sys_calculate_free_frames()) != 768 + 1) panic("Wrong allocation: ");
		if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  256) panic("Wrong page file allocation: ");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation: ");
	}

	//[2] Free some to create holes
	{
		//3 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		free(ptr_allocations[1]);
		//if ((sys_calculate_free_frames() - freeFrames) != 256) panic("Wrong free: ");
		if( (usedDiskPages - sys_pf_calculate_allocated_pages()) !=  3*256) panic("Wrong page file free: ");
		if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong free: ");

		//2 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		free(ptr_allocations[3]);
		//if ((sys_calculate_free_frames() - freeFrames) != 512) panic("Wrong free: ");
		if( (usedDiskPages - sys_pf_calculate_allocated_pages()) !=  2*256) panic("Wrong page file free: ");
		if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong free: ");

		//1 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		free(ptr_allocations[5]);
		//if ((sys_calculate_free_frames() - freeFrames) != 768) panic("Wrong free: ");
		if( (usedDiskPages - sys_pf_calculate_allocated_pages()) !=  256) panic("Wrong page file free: ");
		if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong free: ");
	}

	//[3] Allocate again [test best fit]
	{
		//Allocate 512 KB - should be placed in 3rd hole
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		ptr_allocations[8] = malloc(512*kilo);
		if ((uint32) ptr_allocations[8] !=  (USER_HEAP_START + 11*Mega) || (uint32) ptr_allocations[8] > (USER_HEAP_START + 11*Mega + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		//if ((freeFrames - sys_calculate_free_frames()) != 128) panic("Wrong allocation: ");
		if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  128) panic("Wrong page file allocation: ");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation: ");

		//Allocate 1 MB - should be placed in 2nd hole
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		ptr_allocations[9] = malloc(1*Mega - kilo);
		if ((uint32) ptr_allocations[9] != (USER_HEAP_START + 8*Mega) || (uint32) ptr_allocations[9] > (USER_HEAP_START + 8*Mega + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		//if ((freeFrames - sys_calculate_free_frames()) != 256) panic("Wrong allocation: ");
		if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  256) panic("Wrong page file allocation: ");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation: ");

		//Allocate 256 KB - should be placed in remaining of 3rd hole
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		ptr_allocations[10] = malloc(256*kilo - kilo);
		if ((uint32) ptr_allocations[10] !=  (USER_HEAP_START + 11*Mega + 512*kilo) || (uint32) ptr_allocations[10] > (USER_HEAP_START + 11*Mega + 512*kilo + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		//if ((freeFrames - sys_calculate_free_frames()) != 64) panic("Wrong allocation: ");
		if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  64) panic("Wrong page file allocation: ");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation: ");

		//Allocate 4 MB - should be placed in end of all allocations
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		ptr_allocations[11] = malloc(4*Mega - kilo);
		if ((uint32) ptr_allocations[11] != (USER_HEAP_START + 14*Mega) || (uint32) ptr_allocations[11] > (USER_HEAP_START + 14*Mega + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		//if ((freeFrames - sys_calculate_free_frames()) != 1024 + 1) panic("Wrong allocation: ");
		if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  1024) panic("Wrong page file allocation: ");
		if ((freeFrames - sys_calculate_free_frames()) != 1) panic("Wrong allocation: ");
	}

	//[4] Free contiguous allocations
	{
		//1M Hole appended to already existing 1M hole in the middle
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		free(ptr_allocations[4]);
		//if ((sys_calculate_free_frames() - freeFrames) != 256) panic("Wrong free: ");
		if( (usedDiskPages - sys_pf_calculate_allocated_pages()) !=  256) panic("Wrong page file free: ");
		if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong free: ");

		//another 512 KB Hole appended to the hole
		freeFrames = sys_calculate_free_frames() ;
		usedDiskPages = sys_pf_calculate_allocated_pages();
		free(ptr_allocations[8]);
		//if ((sys_calculate_free_frames() - freeFrames) != 256) panic("Wrong free: ");
		if( (usedDiskPages - sys_pf_calculate_allocated_pages()) !=  128) panic("Wrong page file free: ");
		if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong free: ");
	}

	//[5] Allocate again [test best fit]
	{
		//Allocate 2 MB - should be placed in the contiguous hole (2 MB + 512 KB)
		freeFrames = sys_calculate_free_frames();
		usedDiskPages = sys_pf_calculate_allocated_pages();
		ptr_allocations[12] = malloc(2*Mega - kilo);
		if ((uint32) ptr_allocations[12] != (USER_HEAP_START + 9*Mega) || (uint32) ptr_allocations[12] > (USER_HEAP_START + 9*Mega + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		//if ((freeFrames - sys_calculate_free_frames()) != 512+32) panic("Wrong allocation: ");
		if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  2*256) panic("Wrong page file allocation: ");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation: ");
	}
	cprintf("Congratulations!! test BEST FIT allocation (1) completed successfully.\n");

	return;
}
