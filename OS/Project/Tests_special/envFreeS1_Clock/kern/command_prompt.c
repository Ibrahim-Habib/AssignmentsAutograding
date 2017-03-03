// Simple command-line kernel prompt useful for
// controlling the kernel and exploring the system interactively.

/*
KEY WORDS
==========
CONSTANTS:	WHITESPACE, NUM_OF_COMMANDS
VARIABLES:	Command, commands, name, description, function_to_execute, number_of_arguments, arguments, command_string, command_line, command_found
FUNCTIONS:	readline, cprintf, execute_command, run_command_prompt, command_kernel_info, command_help, strcmp, strsplit, start_of_kernel, start_of_uninitialized_data_section, end_of_kernel_code_section, end_of_kernel
=====================================================================================================================================================================================================
 */

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>
#include <inc/disk.h>


#include <kern/console.h>
#include <kern/command_prompt.h>
#include <kern/memory_manager.h>
#include <kern/trap.h>
#include <kern/kdebug.h>
#include <kern/user_environment.h>
#include <kern/file_manager.h>
#include <kern/sched.h>
#include <kern/kheap.h>


//Structure for each command
struct Command
{
	char *name;
	char *description;
	// return -1 to force command prompt to exit
	int (*function_to_execute)(int number_of_arguments, char** arguments);
};

//Functions Declaration
extern uint32 enableBuffering();
extern uint32 isBufferingEnabled();
extern void setModifiedBufferLength(uint32 length) ;
extern uint32 getModifiedBufferLength();

int execute_command(char *command_string);
int command_writeusermem(int number_of_arguments, char **arguments);
int command_readusermem(int number_of_arguments, char **arguments);
int command_readuserblock(int number_of_arguments, char **arguments);
int command_remove_table(int number_of_arguments, char **arguments);
int command_allocuserpage(int number_of_arguments, char **arguments);
int command_meminfo(int number_of_arguments, char **arguments);

int command_set_page_rep_FIFO(int number_of_arguments, char **arguments);
int command_set_page_rep_CLOCK(int number_of_arguments, char **arguments);
int command_set_page_rep_LRU(int number_of_arguments, char **arguments);
int command_set_page_rep_ModifiedCLOCK(int number_of_arguments, char **arguments);
int command_print_page_rep(int number_of_arguments, char **arguments);

int command_set_heap_plac_FIRSTFIT(int number_of_arguments, char **arguments);
int command_set_heap_plac_BESTFIT(int number_of_arguments, char **arguments);
int command_set_heap_plac_NEXTFIT(int number_of_arguments, char **arguments);
int command_set_heap_plac_WORSTFIT(int number_of_arguments, char **arguments);
int command_print_heap_plac(int number_of_arguments, char **arguments);

int command_disable_modified_buffer(int number_of_arguments, char **arguments);
int command_enable_modified_buffer(int number_of_arguments, char **arguments);

//2016
int command_disable_buffering(int number_of_arguments, char **arguments);
int command_enable_buffering(int number_of_arguments, char **arguments);

int command_set_modified_buffer_length(int number_of_arguments, char **arguments);
int command_get_modified_buffer_length(int number_of_arguments, char **arguments);

//2016: Kernel Heap Tests
extern int test_kmalloc();
extern int test_kfree();
extern int test_kheap_phys_addr();
extern int test_kheap_virt_addr();
extern int test_three_creation_functions();
int command_test_kmalloc(int number_of_arguments, char **arguments);
int command_test_kfree(int number_of_arguments, char **arguments);
int command_test_kheap_phys_addr(int number_of_arguments, char **arguments);
int command_test_kheap_virt_addr(int number_of_arguments, char **arguments);
int command_test_three_creation_functions(int number_of_arguments, char **arguments);

//Array of commands. (initialized)
struct Command commands[] =
{
		{ "help", "Display this list of commands", command_help },
		{ "kernel_info", "Display information about the kernel", command_kernel_info },
		{ "wum", "writes one byte to specific location in given environment" ,command_writeusermem},
		{ "rum", "reads one byte from specific location in given environment" ,command_readusermem},
		{ "wm", "writes one byte to specific location" ,command_writemem_k},
		{ "rm", "reads one byte from specific location" ,command_readmem_k},
		{ "rub", "reads block of bytes from specific location in given environment" ,command_readuserblock},
		{ "kill", "kill the given environment (by its ID) from the system", command_kill_program},
		{ "rut", "", command_remove_table},
		{ "aup", "", command_allocuserpage},
		{ "meminfo", "", command_meminfo},

		{ "run", "runs a single user program", command_run_program },
		{"load", "load a single user program to mem with status = NEW", commnad_load_env},
		{"runall", "run all loaded programs", command_run_all},
		{"printall", "print all loaded programs", command_print_all},
		{"killall", "kill all environments in the system", command_kill_all},
		{"lru", "set replacement algorithm to LRU", command_set_page_rep_LRU},
		{"fifo", "set replacement algorithm to FIFO", command_set_page_rep_FIFO},
		{"clock", "set replacement algorithm to CLOCK", command_set_page_rep_CLOCK},
		{"modifiedclock", "set replacement algorithm to modified CLOCK", command_set_page_rep_ModifiedCLOCK},
		{"rep?", "print current replacement algorithm", command_print_page_rep},

		{"firstfit", "set heap placement strategy to FIRST FIT", command_set_heap_plac_FIRSTFIT},
		{"bestfit", "set heap placement strategy to BEST FIT", command_set_heap_plac_BESTFIT},
		{"nextfit", "set heap placement strategy to NEXT FIT", command_set_heap_plac_NEXTFIT},
		{"worstfit", "set heap placement strategy to WORST FIT", command_set_heap_plac_WORSTFIT},
		{"heap?", "print current heap placement strategy", command_print_heap_plac},

		//2016
		{"nobuff", "", command_disable_buffering},
		{"buff", "", command_enable_buffering},

		{"nomodbuff", "", command_disable_modified_buffer},
		{"modbuff", "", command_enable_modified_buffer},

		{"modbufflength?", "", command_get_modified_buffer_length},
		{"modbufflength", "", command_set_modified_buffer_length},

		{"tstkmalloc", "Kernel Heap: test kmalloc (return address, size, mem access...etc)", command_test_kmalloc},
		{"tstkfree", "Kernel Heap: test kfree (freed frames, mem access...etc)", command_test_kfree},
		{"tstkphysaddr", "Kernel Heap: test kheap_phys_addr", command_test_kheap_phys_addr},
		{"tstkvirtaddr", "Kernel Heap: test kheap_virt_addr", command_test_kheap_virt_addr},
		{"tst3functions", "Env Load: test the creation of new dir, tables and pages WS", command_test_three_creation_functions},
};

//Number of commands = size of the array / size of command structure
#define NUM_OF_COMMANDS (sizeof(commands)/sizeof(commands[0]))

unsigned read_eip();

// ********** This DosKey supported readline function is implemented by **********
// ********** Abdullah Najuib (now FCIS T.A.), 3rd year student, FCIS, 2012
#define history_max 10
#define history_min -1
int history_size = 0;
int history_write = 0;
char history_list[10][1024];

void command_prompt_readline(const char *prompt, char* buf)
{
	int i, c, echoing, current_command, current_match, found_matches, used_match, current_erase, size, history_index, history_used;
	int matches[NUM_OF_COMMANDS];

	if (prompt != NULL)
		cprintf("%s", prompt);

	found_matches = 0;
	current_match = 0;
	current_erase = 0;
	history_index = 0;
	history_used = 0;
	used_match = 0;
	size = 0;
	i = 0;

	echoing = iscons(0);
	while (1)
	{
		c = getchar();

		if (c < 0)
		{
			found_matches = 0;
			history_used = 0;

			if (c != -E_EOF)
				cprintf("read error: %e\n", c);
			return;
		}
		else if(c == 226 || c == 227)
		{
			found_matches = 0;

			if(history_size == 0)
				continue;

			if(history_used == 0)
			{
				history_index = history_write;
				history_used = 1;
			}

			if(history_used == 1 && c == 226)//Up
			{
				history_index--;
				if(history_index == history_min)
					history_index = history_size - 1;

				strcpy(buf,history_list[history_index]);

				for(current_erase = 0;current_erase < i;current_erase++)
				{
					cputchar('\b');
				}

				i = strlen(history_list[history_index]);
				cprintf("%s",buf);
			}
			else if(history_used == 1 && c == 227)//Down
			{
				history_index++;
				if(history_index >= history_size)
					history_index = 0;

				strcpy(buf,history_list[history_index]);

				for(current_erase = 0;current_erase < i;current_erase++)
				{
					cputchar('\b');
				}

				i = strlen(history_list[history_index]);
				cprintf("%s",buf);
			}
		}
		else if (c >= ' ' && i < BUFLEN-1)
		{
			found_matches = 0;
			history_used = 0;

			if (echoing)
				cputchar(c);
			buf[i++] = c;
		}
		else if (c == '\b' && i > 0)
		{
			found_matches = 0;
			history_used = 0;

			if (echoing)
				cputchar(c);

			i--;
		}
		else if (c == '\n' || c == '\r')
		{
			found_matches = 0;
			history_used = 0;

			if (echoing)
				cputchar(c);

			buf[i] = 0;
			return;
		}
		else if(c == '\t')
		{
			history_used = 0;

			if(found_matches == 0)
			{
				current_match = 0;
				used_match = 0;

				for(current_command = 0;current_command < NUM_OF_COMMANDS;current_command++)
				{
					if(strncmp(buf,commands[current_command].name,i) == 0)
					{
						matches[current_match++] = current_command;
					}
				}

				if(current_match != 0)
					found_matches = 1;
			}

			if(found_matches == 1)
			{
				strcpy(buf,commands[matches[used_match]].name);

				for(current_erase = 0;current_erase < i;current_erase++)
				{
					cputchar('\b');
				}

				i = strlen(commands[matches[used_match]].name);
				cprintf("%s",buf);

				used_match++;
				if(used_match == current_match)
					used_match = 0;
			}
		}
	}
}
// ******************************************************************
// ******************************************************************

int firstTime = 3;
//define the white-space symbols
#define WHITESPACE "\t\r\n "
//invoke the command prompt
void run_command_prompt()
{
	char command_line[BUFLEN];
	setPageReplacmentAlgorithmCLOCK();

	while (1==1)
	{
		if(firstTime == 3)
		{
			firstTime = 2;
			before =calculate_available_frames();

			cprintf("Total available frames = %d\nFree Buffered = %d\nFree Not Buffered = %d\nModified = %d\n",
					before.freeBuffered+ before.freeNotBuffered+ before.modified, before.freeBuffered, before.freeNotBuffered, before.modified);

			int numOfArgs = 0;
			char *args[MAX_ARGUMENTS] ;

			char l1[100] = "load fib 5";
			strsplit(l1, WHITESPACE, args, &numOfArgs) ;
			commnad_load_env(numOfArgs, args);

			char l2[100] = "load fact 5";
			strsplit(l2, WHITESPACE, args, &numOfArgs) ;
			commnad_load_env(numOfArgs, args);

			char l3[100] = "load fos_add 5";
			strsplit(l3, WHITESPACE, args, &numOfArgs) ;
			commnad_load_env(numOfArgs, args);

			char l4[100] = "runall";
			strsplit(l4, WHITESPACE, args, &numOfArgs) ;
			command_run_all(numOfArgs, args);
		}
		else if(firstTime == 2)
		{
			firstTime = 1;
			int numOfArgs = 0;
			char *args[MAX_ARGUMENTS] ;		
			char l4[100] = "killall";
			strsplit(l4, WHITESPACE, args, &numOfArgs) ;
			command_kill_all(numOfArgs, args);
		}
		else if(firstTime == 1)
		{
			firstTime = 0;
			struct freeFramesCounters x = calculate_available_frames();
			if(x.freeBuffered != before.freeBuffered || x.freeNotBuffered != before.freeNotBuffered || x.modified != before.modified)
			{
				cprintf("meminfo not the same before and after\n");
				return;
			}
			else
			{
				cprintf("[[AUTO_GR@DING]Congratulations!! test Scenario 1 completed successfully.\n\n");
			}
		}
		
		//readline("FOS> ", command_line);

		// ********** This DosKey supported readline function is implemented by **********
		// ********** Abdullah Najuib (now FCIS T.A.), 3rd year student, FCIS, 2012
		command_prompt_readline("FOS> ", command_line);

		//save history
		strcpy(history_list[history_write],command_line);

		if(++history_write == history_max)
			history_write = 0;

		if(history_size != 10)
			history_size++;

		//parse and execute the command
		if (command_line != NULL)
			if (execute_command(command_line) < 0)
				break;
	}
}

/***** Kernel command prompt command interpreter *****/

//Function to parse any command and execute it
//(simply by calling its corresponding function)
int execute_command(char *command_string)
{
	// Split the command string into whitespace-separated arguments
	int number_of_arguments;
	//allocate array of char * of size MAX_ARGUMENTS = 16 found in string.h
	char *arguments[MAX_ARGUMENTS];


	strsplit(command_string, WHITESPACE, arguments, &number_of_arguments) ;
	if (number_of_arguments == 0)
		return 0;

	// Lookup in the commands array and execute the command
	int command_found = 0;
	int i ;
	for (i = 0; i < NUM_OF_COMMANDS; i++)
	{
		if (strcmp(arguments[0], commands[i].name) == 0)
		{
			command_found = 1;
			break;
		}
	}

	if(command_found)
	{
		int return_value;
		return_value = commands[i].function_to_execute(number_of_arguments, arguments);
		return return_value;
	}
	else
	{
		//if not found, then it's unknown command
		cprintf("Unknown command '%s'\n", arguments[0]);
		return 0;
	}
}

/***** Implementations of basic kernel command prompt commands *****/

//print name and description of each command
int command_help(int number_of_arguments, char **arguments)
{
	int i;
	for (i = 0; i < NUM_OF_COMMANDS; i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].description);

	cprintf("-------------------\n");

	for (i = 0; i < NUM_USER_PROGS; i++)
		cprintf("run %s - %s [User Program]\n", ptr_UserPrograms[i].name, ptr_UserPrograms[i].desc);
	return 0;
}

//print information about kernel addresses and kernel size
int command_kernel_info(int number_of_arguments, char **arguments )
{
	extern char start_of_kernel[], end_of_kernel_code_section[], start_of_uninitialized_data_section[], end_of_kernel[];

	cprintf("Special kernel symbols:\n");
	cprintf("  Start Address of the kernel 			%08x (virt)  %08x (phys)\n", start_of_kernel, start_of_kernel - KERNEL_BASE);
	cprintf("  End address of kernel code  			%08x (virt)  %08x (phys)\n", end_of_kernel_code_section, end_of_kernel_code_section - KERNEL_BASE);
	cprintf("  Start addr. of uninitialized data section 	%08x (virt)  %08x (phys)\n", start_of_uninitialized_data_section, start_of_uninitialized_data_section - KERNEL_BASE);
	cprintf("  End address of the kernel   			%08x (virt)  %08x (phys)\n", end_of_kernel, end_of_kernel - KERNEL_BASE);
	cprintf("Kernel executable memory footprint: %d KB\n",
			(end_of_kernel-start_of_kernel+1023)/1024);
	return 0;
}

int command_writeusermem(int number_of_arguments, char **arguments)
{
	int32 envId = strtol(arguments[1],NULL, 10);
	struct Env* env = NULL;
	envid2env(envId, &env, 0 );

	int address = strtol(arguments[3], NULL, 16);

	if(env == NULL) return 0;

	uint32 oldDir = rcr3();
	//lcr3((uint32) K_PHYSICAL_ADDRESS( env->env_pgdir));
	lcr3((uint32) (env->env_cr3));

	unsigned char *ptr = (unsigned char *)(address) ;

	//Write the given Character
	*ptr = arguments[2][0];
	lcr3(oldDir);

	return 0;
}

int command_writemem_k(int number_of_arguments, char **arguments)
{
	unsigned char* address = (unsigned char*)strtol(arguments[1], NULL, 16)+KERNEL_BASE;
	int size = strtol(arguments[2], NULL, 10);
	int c, i=0;
	int stringLen = strlen(arguments[3]);

	for(c=0; c<size; c++)
	{
		for(i=0;i < stringLen; i++)
		{
			*address = arguments[3][i];
			address++;
		}
	}
	return 0;


}

int command_readusermem(int number_of_arguments, char **arguments)
{
	int32 envId = strtol(arguments[1],NULL, 10);
	struct Env* env = NULL;
	envid2env(envId, &env, 0 );

	int address = strtol(arguments[2], NULL, 16);

	if(env == NULL) return 0;

	uint32 oldDir = rcr3();
	//lcr3((uint32) K_PHYSICAL_ADDRESS( env->env_pgdir));
	lcr3((uint32)( env->env_cr3));

	unsigned char *ptr = (unsigned char *)(address) ;

	//Write the given Character
	cprintf("value at address %x = %c\n", address, *ptr);

	lcr3(oldDir);
	return 0;
}

int command_readmem_k(int number_of_arguments, char **arguments)
{
	unsigned char* address = (unsigned char*)strtol(arguments[1], NULL, 16)+KERNEL_BASE;
	int size = strtol(arguments[2], NULL, 10);
	int i=0;
	for(;i < size; i++)
	{
		cprintf("%c",*address++);
	}
	cprintf("\n");
	return 0;
}


int command_readuserblock(int number_of_arguments, char **arguments)
{
	int32 envId = strtol(arguments[1],NULL, 10);
	struct Env* env = NULL;
	envid2env(envId, &env, 0 );

	int address = strtol(arguments[2], NULL, 16);
	int nBytes = strtol(arguments[3], NULL, 10);

	unsigned char *ptr = (unsigned char *)(address) ;
	//Write the given Character

	if(env == NULL) return 0;

	uint32 oldDir = rcr3();
	//lcr3((uint32) K_PHYSICAL_ADDRESS( env->env_pgdir));
	lcr3((uint32)( env->env_cr3));

	int i;
	for(i = 0;i<nBytes; i++)
	{
		cprintf("%08x : %02x  %c\n", ptr, *ptr, *ptr);
		ptr++;
	}
	lcr3(oldDir);

	return 0;
}

int command_remove_table(int number_of_arguments, char **arguments)
{
	int32 envId = strtol(arguments[1],NULL, 10);
	struct Env* env = NULL;
	envid2env(envId, &env, 0 );
	if(env == 0) return 0;

	uint32 address = strtol(arguments[2], NULL, 16);
	unsigned char *va = (unsigned char *)(address) ;
	uint32 table_pa = env->env_page_directory[PDX(address)] & 0xFFFFF000;

	//remove the table
	if(USE_KHEAP && !CHECK_IF_KERNEL_ADDRESS(va))
	{
		kfree((void*)kheap_virtual_address(table_pa));
	}
	else
	{
		// get the physical address and Frame_Info of the page table
		struct Frame_Info *table_frame_info = to_frame_info(table_pa);
		// set references of the table frame to 0 then free it by adding
		// to the free frame list
		table_frame_info->references = 0;
		free_frame(table_frame_info);
	}

	// set the corresponding entry in the directory to 0
	uint32 dir_index = PDX(va);
	env->env_page_directory[dir_index] &= (~PERM_PRESENT);
	tlbflush();
	return 0;
}

int command_allocuserpage(int number_of_arguments, char **arguments)
{
	int32 envId = strtol(arguments[1],NULL, 10);
	struct Env* env = NULL;
	envid2env(envId, &env, 0 );
	if(env == 0) return 0;

	uint32 address = strtol(arguments[2], NULL, 16);
	unsigned char *va = (unsigned char *)(address) ;

	// Allocate a single frame from the free frame list
	struct Frame_Info * ptr_frame_info ;
	int ret = allocate_frame(&ptr_frame_info);
	if (ret == E_NO_MEM)
	{
		cprintf("ERROR: no enough memory\n");
		return 0;
	}

	// Map this frame to the given user virtual address
	map_frame(env->env_page_directory, ptr_frame_info, va, PERM_WRITEABLE | PERM_USER);

	return 0;
}

int command_meminfo(int number_of_arguments, char **arguments)
{
	struct freeFramesCounters counters =calculate_available_frames();
	cprintf("Total available frames = %d\nFree Buffered = %d\nFree Not Buffered = %d\nModified = %d\n",
			counters.freeBuffered+ counters.freeNotBuffered+ counters.modified, counters.freeBuffered, counters.freeNotBuffered, counters.modified);
	return 0;
}


int command_run_program(int number_of_arguments, char **arguments)
{
	struct Env* env;
	if(USE_KHEAP)
	{
		if(number_of_arguments < 3)
		{
			cprintf("Error: Please specify a working set size in the third argument, aborting.\n");
			return 0;
		}
		env = env_create(arguments[1], strtol(arguments[2], NULL, 10));
	}
	else
	{
		env = env_create(arguments[1], __PWS_MAX_SIZE);
	}

	if(env == NULL) return 0;
	cprintf("\nEnvironment Id= %d\n",env->env_id);

	sched_new_env(env);
	sched_run_env(env->env_id);

	return 0;
}

int command_kill_program(int number_of_arguments, char **arguments)
{
	int32 envId = strtol(arguments[1],NULL, 10);

	sched_kill_env(envId);

	return 0;
}

int commnad_load_env(int number_of_arguments, char **arguments)
{
	struct Env* env;
	if(USE_KHEAP)
	{
		if(number_of_arguments < 3)
		{
			cprintf("Error: Please specify a working set size in the third argument, aborting.\n");
			return 0;
		}
		env = env_create(arguments[1], strtol(arguments[2], NULL, 10));
	}
	else
	{
		env = env_create(arguments[1], __PWS_MAX_SIZE);
	}

	if (env == NULL)
		return 0 ;

	sched_new_env(env) ;

	cprintf("\nEnvironment Id= %d\n",env->env_id);
	return 0;
}

int command_run_all(int number_of_arguments, char **arguments)
{
	sched_run_all();

	return 0 ;
}

int command_print_all(int number_of_arguments, char **arguments)
{
	sched_print_all();

	return 0 ;
}

int command_kill_all(int number_of_arguments, char **arguments)
{
	sched_kill_all();

	return 0 ;
}

int command_set_page_rep_LRU(int number_of_arguments, char **arguments)
{
	setPageReplacmentAlgorithmLRU();
	cprintf("Page replacement algorithm is now LRU\n");
	return 0;
}

int command_set_page_rep_CLOCK(int number_of_arguments, char **arguments)
{
	setPageReplacmentAlgorithmCLOCK();
	cprintf("Page replacement algorithm is now CLOCK\n");
	return 0;
}

int command_set_page_rep_FIFO(int number_of_arguments, char **arguments)
{
	setPageReplacmentAlgorithmFIFO();
	cprintf("Page replacement algorithm is now FIFO\n");
	return 0;
}

int command_set_page_rep_ModifiedCLOCK(int number_of_arguments, char **arguments)
{
	setPageReplacmentAlgorithmModifiedCLOCK();
	cprintf("Page replacement algorithm is now Modified CLOCK\n");
	return 0;
}

/*2015*///BEGIN======================================================
int command_print_page_rep(int number_of_arguments, char **arguments)
{
	if (isPageReplacmentAlgorithmCLOCK())
		cprintf("Page replacement algorithm is CLOCK\n");
	else if (isPageReplacmentAlgorithmLRU())
		cprintf("Page replacement algorithm is LRU\n");
	else if (isPageReplacmentAlgorithmFIFO())
		cprintf("Page replacement algorithm is FIFO\n");
	else if (isPageReplacmentAlgorithmModifiedCLOCK())
		cprintf("Page replacement algorithm is Modified CLOCK\n");
	else
		cprintf("Page replacement algorithm is UNDEFINED\n");

	return 0;
}


int command_set_heap_plac_FIRSTFIT(int number_of_arguments, char **arguments)
{
	setUHeapPlacementStrategyFIRSTFIT();
	cprintf("User Heap placement strategy is now FIRST FIT\n");
	return 0;
}

int command_set_heap_plac_BESTFIT(int number_of_arguments, char **arguments)
{
	setUHeapPlacementStrategyBESTFIT();
	cprintf("User Heap placement strategy is now BEST FIT\n");
	return 0;
}

int command_set_heap_plac_NEXTFIT(int number_of_arguments, char **arguments)
{
	setUHeapPlacementStrategyNEXTFIT();
	cprintf("User Heap placement strategy is now NEXT FIT\n");
	return 0;
}
int command_set_heap_plac_WORSTFIT(int number_of_arguments, char **arguments)
{
	setUHeapPlacementStrategyWORSTFIT();
	cprintf("User Heap placement strategy is now WORST FIT\n");
	return 0;
}

int command_print_heap_plac(int number_of_arguments, char **arguments)
{
	if (isUHeapPlacementStrategyFIRSTFIT())
		cprintf("User Heap placement strategy is FIRST FIT\n");
	else if (isUHeapPlacementStrategyBESTFIT())
		cprintf("User Heap placement strategy is BEST FIT\n");
	else if (isUHeapPlacementStrategyNEXTFIT())
		cprintf("User Heap placement strategy is NEXT FIT\n");
	else if (isUHeapPlacementStrategyWORSTFIT())
		cprintf("User Heap placement strategy is WORST FIT\n");
	else
		cprintf("User Heap placement strategy is UNDEFINED\n");

	return 0;
}

/*2015*///END======================================================

int command_disable_modified_buffer(int number_of_arguments, char **arguments)
{
	if(!isBufferingEnabled())
	{
		cprintf("Buffering is not enabled. Please enable buffering first.\n");
	}
	else
	{
		enableModifiedBuffer(0);
		cprintf("Modified Buffer is now DISABLED\n");
	}
	return 0;
}


int command_enable_modified_buffer(int number_of_arguments, char **arguments)
{
	if(!isBufferingEnabled())
	{
		cprintf("Buffering is not enabled. Please enable buffering first.\n");
	}
	else
	{
		enableModifiedBuffer(1);
		cprintf("Modified Buffer is now ENABLED\n");
	}
	return 0;
}

/*2016 ============================================================================*/

int command_disable_buffering(int number_of_arguments, char **arguments)
{
	enableBuffering(0);
	enableModifiedBuffer(0);
	cprintf("Buffering is now DISABLED\n");
	return 0;
}


int command_enable_buffering(int number_of_arguments, char **arguments)
{
	enableBuffering(1);
	enableModifiedBuffer(1);
	if(getModifiedBufferLength() == 0)
	{
		cprintf("Modified buffer enabled but with length = 0\n");
		char str[100];
		readline("Please enter the modified buff length = ", str);
		setModifiedBufferLength(strtol(str, NULL, 10));
		cprintf("Modified buffer length updated = %d\n", getModifiedBufferLength());
	}
	cprintf("Buffering is now ENABLED\n");
	return 0;
}

int command_set_modified_buffer_length(int number_of_arguments, char **arguments)
{
	if(!isBufferingEnabled())
	{
		cprintf("Buffering is not enabled. Please enable buffering to use the modified buffer.\n");
	}
	else if (!isModifiedBufferEnabled())
	{
		cprintf("Modified Buffering is not enabled. Please enable modified buffering.\n");
	}
	setModifiedBufferLength(strtol(arguments[1], NULL, 10));
	cprintf("Modified buffer length updated = %d\n", getModifiedBufferLength());
	return 0;
}

int command_get_modified_buffer_length(int number_of_arguments, char **arguments)
{
	if(!isBufferingEnabled())
	{
		cprintf("Buffering is not enabled. Please enable buffering to use the modified buffer.\n");
	}
	else if (!isModifiedBufferEnabled())
	{
		cprintf("Modified Buffering is not enabled. Please enable modified buffering.\n");
	}
	cprintf("Modified buffer length = %d\n", getModifiedBufferLength());
	return 0;
}

int command_test_kmalloc(int number_of_arguments, char **arguments)
{
	test_kmalloc();
	return 0;
}
int command_test_kfree(int number_of_arguments, char **arguments)
{
	test_kfree();
	return 0;
}
int command_test_kheap_phys_addr(int number_of_arguments, char **arguments)
{
	test_kheap_phys_addr();
	return 0;
}
int command_test_kheap_virt_addr(int number_of_arguments, char **arguments)
{
	test_kheap_virt_addr();
	return 0;
}
int command_test_three_creation_functions(int number_of_arguments, char **arguments)
{
	test_three_creation_functions();
	return 0;
}

//END======================================================
