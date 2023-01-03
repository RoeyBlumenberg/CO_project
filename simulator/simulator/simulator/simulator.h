#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include <stdbool.h>


#define NUM_OF_REGS 16
#define MAX_LINE_LEN_HEX 7
#define LEN_IMEM_LINE 20
#define HEX_BASE 16
#define BIN_CHAR_LEN 4
#define MAX_LEN_DMEM 4096
#define LEN_DMEMIN_LINE 8
#define MAX_LEN_IMEM 1024
#define LEN_IMEMIN_LINE 5
#define NUM_OF_IO_REGS 22
#define NUM_OF_SECTORS 128
#define MAX_IOREG_NAME 12

#define NUM_OF_WORDS_IN_SECTOR 128
#define X_PIXELS 352
#define Y_PIXELS 288
#define GRAY_SCALE 256
#define irq0enable 0
#define irq1enable 1
#define irq2enable 2
#define irq0status 3
#define irq1status 4
#define irq2status 5
#define irqhandler 6
#define irqreturn 7
#define clks 8
#define leds 9
#define reserved 10
#define timerenable 11
#define timercurrent 12
#define timermax 13
#define diskcmd 14
#define disksector 15
#define diskbuffer 16
#define diskstatus 17
#define monitorcmd 18
#define monitorx 19
#define monitory 20
#define monitordata 21


typedef struct Instruction_ {
	int opcode;
	int rd;
	int rs;
	int rt;
} Instruction;

typedef struct RegisterFile_ {
	int PC;
	char INST[MAX_LINE_LEN_HEX];
	int regs[NUM_OF_REGS];
	int IOregs[NUM_OF_IO_REGS];
	int prev_leds;//check if rellevant
	int instructions_count;
	bool busy_with_interrupt;
} RegisterFile;

typedef struct Monitor_ {
	int frame_buffer[X_PIXELS][Y_PIXELS];
} Monitor;

typedef struct DataMemory_ {
	int data;
} DataMemory;

typedef struct InstructionMemory_ {
	char INST[MAX_LINE_LEN_HEX];
	Instruction instruction;
	int imm;
	bool is_imm_cmd;
	bool is_valid;
	bool write_reg0_or_reg1;
	bool write_readonly_IOregs;
} InstructionMemory;

typedef struct HardDrive_ {
	int sectors[NUM_OF_SECTORS][NUM_OF_WORDS_IN_SECTOR];
	int clks_counter;
} HardDrive;

FILE *open_file(char *file_name, char *mode);
void close_file(FILE *file);
void init_DataMemory(DataMemory* Date_Memory);
void init_InstructionMemory(InstructionMemory* Instruction_Memory);
void init_RegisterFile(RegisterFile* file_registers);
void init_HardDrive(HardDrive *hard_drive);
void init_Monitor(Monitor *monitor);
void read_dmemin(FILE *file, DataMemory *Data_memory);
void read_imemin(FILE *file, InstructionMemory* Instruction_Memory, RegisterFile *Register_file);
void read_diskin(FILE *file, HardDrive *Hard_Drive);
void IMEMIN_pass(FILE *trace, InstructionMemory *instruction_memory, RegisterFile *register_file, DataMemory *data_memory, FILE *irq2_in, Monitor *monitor, HardDrive *hard_drive, FILE *leds_file, FILE *hwregtrace); void print_dmemout(FILE *dmemout, DataMemory *Data_memory);
void print_diskout(FILE *diskout, HardDrive *hard_drive);
void print_regout(FILE *regout, RegisterFile Register_File);
void print_cycles(FILE *cycles, RegisterFile Register_File);
void print_hwregtrace(FILE *hwregtrace, char *READ_or_WRITE, int cycles_count, int num_IOreg, int data);
void print_monitor(FILE *monitor_file, Monitor *monitor);
void print_monitor_yuv(FILE *monitor_yuv_file, Monitor *monitor);
#endif#pragma once
#pragma once
