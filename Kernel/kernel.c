#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveRTC.h>
#include <idtLoader.h>
#include <time.h>
#include <defs.h>
#include <naiveGraphicsConsole.h>
#include <interrupts.h>
#include <speaker.h>
#include <processManagement.h>
#include <scheduler.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();

void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	#ifdef BUDDY
	setupBuddy();
	#endif 

	return getStackBase();
}

void haltProc()
{
	while(1) _hlt();
}

int main()
{	

	_cli();
	#ifdef BUDDY
	ngc_print("[MEMORY] Using Buddy Allocator\n");
	#else
	ngc_print("[MEMORY] Using Default Allocator\n");
	#endif

	char* argv[]= {0};

	PID pid = processCreate(&haltProc,0,argv);
	char * argvshell[] = {"shell"};


	pid = processCreate(sampleCodeModuleAddress,1,argvshell);
	changePriority(pid,HIGH);
	setBackground(pid,FOREGROUND);
	//carga
	load_idt();
	ngc_paint_screen(0x000000);
	((EntryPoint)sampleCodeModuleAddress)();
	
	while(1) _hlt();
	
	return 0;
}
