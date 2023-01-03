#include <stdio.h>
#include "simulator.h"


int main(int argc, char *argv[]) {
	FILE *imemin,/* *dmemin*/, *diskin, *irq2in, *dmemout, *regout, *trace, *hwregtrace, *cycles, *leds_file, *monitor, *monitor_yuv, *diskout;
	DataMemory Data_Memory[MAX_LEN_DMEM];
	InstructionMemory Instruction_Memory[MAX_LEN_IMEM];
	RegisterFile Register_file;
	Monitor Monitor;
	HardDrive Hard_Drive;
	// init structures
	init_DataMemory(Data_Memory);
	init_InstructionMemory(Instruction_Memory);
	init_RegisterFile(&Register_file);
	init_Monitor(&Monitor);
	init_HardDrive(&Hard_Drive);
	// get data memory from dmemin file and create a struct contains our data memory.
	dmemin = open_file(argv[2], "r");
	read_dmemin(dmemin, Data_Memory);
	close_file(dmemin);
	// get disk memory from diskin file and create a struct contains our hard disk memory.
	diskin = open_file(argv[3], "r");
	read_diskin(diskin, &Hard_Drive);
	// open relevant files in order to pass over instruction memory, execute the instruction, handling interrupts and print the needed files.
	imemin = open_file(argv[1], "r");
	irq2in = open_file(argv[4], "r");
	trace = open_file(argv[7], "w");
	hwregtrace = open_file(argv[8], "w");
	leds_file = open_file(argv[10], "w");
	// get intructions memory from imemin file and create a struct contains our instruction memory.
	read_imemin(imemin, Instruction_Memory, &Register_file);
	// pass over the instruction memory
	IMEMIN_pass(trace, Instruction_Memory, &Register_file, Data_Memory, irq2in, &Monitor, &Hard_Drive, leds_file, hwregtrace);
	// close file we finished to use
	close_file(imemin);
	close_file(irq2in);
	close_file(hwregtrace);
	// print dmemout file
	dmemout = open_file(argv[5], "w");
	print_dmemout(dmemout, Data_Memory);
	close_file(dmemout);
	// print regout file
	regout = open_file(argv[6], "w");
	print_regout(dmemout, Register_file);
	close_file(regout);
	// print cycles file
	cycles = open_file(argv[9], "w");
	print_cycles(cycles, Register_file);
	close_file(cycles);
	// print monitor file
	monitor = open_file(argv[11], "w");
	print_monitor(monitor, &Monitor);
	close_file(monitor);
	// print monitor.yuv file in binary mode
	monitor_yuv = open_file(argv[12], "wb");
	print_monitor_yuv(monitor_yuv, &Monitor);
	close_file(monitor_yuv);
	// print diskout file
	diskout = open_file(argv[13], "w");
	print_diskout(diskout, &Hard_Drive);
	close_file(diskout);

	return 0;
}