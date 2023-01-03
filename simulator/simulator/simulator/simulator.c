#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "simulator.h"

// input - a variable of type Data Memory
// output - none
// a function that initializes a data memory type variable
void init_DataMemory(DataMemory* Data_Memory) {
	for (int i = 0; i < MAX_LEN_DMEM; i++) {
		Data_Memory[i].data = 0;
	}
}
// input - a variable of type Instruction Memory
// output - none
// a function that initializes an Instruction Memory type variable
void init_InstructionMemory(InstructionMemory* Instruction_Memory) {
	for (int i = 0; i < MAX_LEN_IMEM; i++) {
		Instruction_Memory[i].is_valid = false;
		Instruction_Memory[i].is_imm_cmd = false;
		Instruction_Memory[i].write_reg0_or_reg1 = false;
		Instruction_Memory[i].write_readonly_IOregs = false;//check if rellevant
	}
}
#if 0
// input - a variable of type Hard Drive
// output - none
// a function that initializes a Hard Drive type variable
void init_HardDrive(HardDrive *hard_drive) {
	for (int i = 0; i < NUM_OF_SECTORS; i++) {
		for (int j = 0; j < NUM_OF_WORDS_IN_SECTOR; j++) {
			hard_drive->sectors[i][j] = 0;
		}
	}
}

// input - a variable of type Monitor
// output - none
// a function that initializes a Monitor type variable
void init_Monitor(Monitor *monitor) {
	for (int i = 0; i < X_PIXELS; i++) {
		for (int j = 0; j < Y_PIXELS; j++) {
			monitor->frame_buffer[i][j] = 0;
		}
	}
}

#endif
// input - a variable of type Register File
// output - none
// a function that initializes a Register File type variable
void init_RegisterFile(RegisterFile* file_registers) {
	file_registers->PC = 0;
	file_registers->prev_leds = 0;//check if rellevant
	file_registers->instructions_count = 0;
	file_registers->busy_with_interrupt = false;//check if rellevant
	for (int i = 0; i < NUM_OF_REGS; i++) {
		file_registers->regs[i] = 0;
	}
	for (int j = 0; j < NUM_OF_IO_REGS; j++) {//check if rellevant
		file_registers->IOregs[j] = 0;
	}
}

// Input - file name to open and opening mode: "r" or "w"
// Output - File pointer
// Function to open file and check if the opening successed. else it prints an error and exits from the program.
FILE *open_file(char *file_name, char *mode) {
	FILE *file;
	file = fopen(file_name, mode);
	if (file == NULL) {
		printf("Error while opening the file\n");
		exit(1);
	}
	return file;
}

// Input - file name to close.
// Output - nothing.
// Function checks wheter the file pointer is not NULL, if NULL it prints an error and exits from the programe, else closes the file.
void close_file(FILE *file) {
	if (file == NULL) {
		printf("Error while closing the file");
		exit(1);
	}
	fclose(file);
	return;
}

// Input - source string, destination string and an end char - until that char we want to copy the source string to the destination string.
// Output - returns version of the input string without whitespaces.
// Function that deletes all whitespaces in the input string.
void copy_str_by_delimiter(const char *source, char *destination, char end_char) {
	int i = 0;
	for (i = 0; source[i] != end_char && source[i] != '\0'; i++) {
		destination[i] = source[i];
	}
	destination[i] = '\0';
}
// input - a source string, a destination string and an integerindicating the number of characters to be copied
// output - none
// the function copies a specific number of integers from one string to another
void copy_n_chars(const char *source, char *destination, int num_of_chars) {
	int i = 0;
	for (i = 0; i < num_of_chars && source[i] != '\0'; i++) {
		destination[i] = source[i];
	}
	destination[i] = '\0';
}

// Input - line string.
// Output - returns true if the line is empty, else false.
// a function that searches if line contains only whitespaces.
bool is_empty_line(char *line) {
	for (int i = 0; line[i] != '\0'; i++) {
		if (!isspace(line[i])) {
			return false;
		}
	}
	return true;
}
////////////////////we need to chack this function again.///////////////
// input - line string.
// output - a boolian variable indicating an imm command.
// a function that determines whether a command contains a constant "imm" or not.
bool is_imm_command(char *line, int opcode) {
	char *line_ptr = line;
	line_ptr += 2; // jump over the bits relates to opcode
	if (opcode == 15) {
		return (*(line_ptr) == '1');
	}
	else if (opcode == 21 || opcode == 18) {
		return false;
	}
	else {
		return (strstr(line_ptr, "1") != NULL);
	}
}
// input - a pointer to the opcode number and a line string.
// output - none.
// a function that takes the opcode number out of a command line string and puts it into the value of a pregiven pointer.
void get_opcode(int* opcode, char* line) {
	char temp[LEN_IMEM_LINE];
	copy_n_chars(line, temp, 2);
	sscanf(temp, "%x", opcode);
	if (*opcode > 21 || *opcode < 0) {//we need to check the numbers
		printf("Not valid opcode\n");
	}
}
// input - a pointer to the register number and a line string.
// output - none.
// a function that takes the register number out of a command line string and puts it into the value of a pregiven pointer.
void get_reg(int* reg, char* line) {
	char temp[LEN_IMEM_LINE];
	copy_n_chars(line, temp, 1);
	sscanf(temp, "%x", reg);
}
// input - a pointer to the imm number and a line string.
// output - none.
// a function that takes the imm number out of a command line string and puts it into the value of a pregiven pointer.
void get_imm(int* imm, char* line) {
	sscanf(line, "%x", imm);
	if (*line >= '8') {
		*imm = *imm | 0xFFF00000; // sign extension to 32 bits.
	}
}

//////////we need to delete this func/////////
// input - a pointer to a varible of type "data memory", an address integer, a data integer and a string declaring "read" or "write" (r or w)
// output - if we choose to read: a data value read from the data memory, if we choose writing: 0.
// a function that reads\ writes(depending on "r_or_w" string) to the data memory.
int DMEM(DataMemory *Data_memory, int address, int data, char *r_or_w) {
	if (address < 0 || address > MAX_LEN_DMEM - 1) { // attempt to write/read from invalid address -> new address = old_address modulu 4096 (DMEMIN length)
		address = abs(address % MAX_LEN_DMEM);
	}
	if (strcmp(r_or_w, "r") == 0) {
		return Data_memory[address].data;
	}
	else {
		Data_memory[address].data = data;
	}
	return 0;
}
// input - a pointer to a variable type register file and a boolian variable indicating an "imm" command
// output - none
// a function that increments the "PC" twice whether the command is an imm command and once otherwise.
void PC_incrementation(RegisterFile *Register_file, bool is_imm_cmd) {
	Register_file->PC++;
	if (is_imm_cmd) {
		Register_file->PC++;
	}
}

#if 0
// input - a pointer to a variable type register file and a pointer to a variable type monitor
// output - none
// a function that writes to the monitor from a value contained in the monitordata register
void monitor_handle(RegisterFile *register_file, Monitor *monitor) {
	int x = register_file->IOregs[monitorx];
	int y = register_file->IOregs[monitory];
	monitor->frame_buffer[x][y] = register_file->IOregs[monitordata];
	register_file->IOregs[monitorcmd] = 0;
}
////////////////we need to delete///// ///////
// input - a pointer to a variable type register file , a pointer to a variable type hard drive and a pointer to a variable type data memory/
// output - none
// a function that handles the hard drive. using the necessery I/O registers the function reads or writes to or from the hard drive and changes the disk registers in case necessery/
void disk_handle(RegisterFile *register_file, HardDrive *hard_drive, DataMemory *data_memory) {
	int address = register_file->IOregs[diskbuffer];
	int num_of_sector = register_file->IOregs[disksector];
	if (register_file->IOregs[diskcmd] == 1) {
		register_file->IOregs[diskstatus] = 1;
		for (int i = 0; i < NUM_OF_WORDS_IN_SECTOR; i++) {
			DMEM(data_memory, address + i, hard_drive->sectors[num_of_sector][i], "w");
		}
		hard_drive->clks_counter = 0; // in order to calculate 1024 clock cycles after out command until the disk finish to read/write sector.
	}
	else if (register_file->IOregs[diskcmd] == 2) {
		register_file->IOregs[diskstatus] = 1;
		for (int j = 0; j < NUM_OF_WORDS_IN_SECTOR; j++) {
			hard_drive->sectors[num_of_sector][j] = DMEM(data_memory, address + j, 0, "r");
		}
		hard_drive->clks_counter = 0; // in order to calculate 1024 clock cycles after out command until the disk finish to read/write sector.
	}
	else {
		return;
	}
}
// input - a pointer to a variable type register file and a pointer to a variable type hard drive
// output - none
// a function that checks whether the hard drive operation is done. if so it updates the I/O registers.t
void disk_check(RegisterFile *register_file, HardDrive *hard_drive) {
	if (register_file->IOregs[diskstatus] == 1) {
		hard_drive->clks_counter++;
		if (hard_drive->clks_counter == 1024) {
			hard_drive->clks_counter = 0;
			register_file->IOregs[diskstatus] = 0;
			register_file->IOregs[diskcmd] = 0;
			register_file->IOregs[irq1status] = 1;
		}
	}
}

// input - a pointer to the file hwregtrace to which we wish to print, a string indicating read or write, an integer indicating the cycles count and an integer indicating the data to be printed.
// output - none
// a function that prints into the file hwregtrace in the format specified in the project istructions.
void print_hwregtrace(FILE *hwregtrace, char *READ_or_WRITE, int cycles_count, int num_IOreg, int data) {
	char IOreg_name[MAX_IOREG_NAME + 1];
	char *IOregs[NUM_OF_IO_REGS] = { "irq0enable", "irq1enable", "irq2enable", "irq0status", "irq1status", "irq2status", "irqhandler", "irqreturn", "clks", "leds", "reserved", "timerenable", "timercurrent", "timermax", "diskcmd", "disksector", "diskbuffer", "diskstatus", "monitorcmd", "monitorx", "monitory", "monitordata" };
	strcpy(IOreg_name, IOregs[num_IOreg]);
	fprintf(hwregtrace, "%d %s %s %08X\n", cycles_count, READ_or_WRITE, IOreg_name, data); //check cycles_count is bigger by 1 than needed.
	return;
}

#endif
// input - a pointer to a variable of type register file and a pointer to a variable of type instruction memory.
// output - none
// a function that prevents writing to constant registers.
void attempt_change_read_only_regs(RegisterFile *Register_file, InstructionMemory *instruction_memory) {
	int opcode = instruction_memory[Register_file->PC].instruction.opcode;
	int rd = instruction_memory[Register_file->PC].instruction.rd;
	int rs = instruction_memory[Register_file->PC].instruction.rs;
	int rt = instruction_memory[Register_file->PC].instruction.rt;
	int num_IOreg = Register_file->regs[rs] + Register_file->regs[rt];
	if (num_IOreg < 0 || num_IOreg > NUM_OF_IO_REGS - 1) { // handle attempt to access an invalid IOreg
		num_IOreg = abs(num_IOreg % NUM_OF_IO_REGS);
	}
	instruction_memory[Register_file->PC].write_reg0_or_reg1 = ((opcode <= 8 && opcode >= 0) || opcode == 16 || opcode == 19) && (rd == 0 || rd == 1);
	instruction_memory[Register_file->PC].write_readonly_IOregs = (opcode == 20) && (num_IOreg == clks || num_IOreg == reserved || num_IOreg == diskstatus); // these IOregs are read only registers.
}
// the next 21 functions are responsible of executing an operation.
//each function executes the operation its name indicates.

void add(RegisterFile *Register_file, InstructionMemory *Instruction_memory, int rd, int rs, int rt) {
	if (!Instruction_memory[Register_file->PC].write_reg0_or_reg1) {
		Register_file->regs[rd] = Register_file->regs[rs] + Register_file->regs[rt];
	}
	PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
}

void sub(RegisterFile *Register_file, InstructionMemory *Instruction_memory, int rd, int rs, int rt) {
	if (!Instruction_memory[Register_file->PC].write_reg0_or_reg1) {
		Register_file->regs[rd] = Register_file->regs[rs] - Register_file->regs[rt];
	}
	PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
}

void and(RegisterFile *Register_file, InstructionMemory *Instruction_memory, int rd, int rs, int rt) {
	if (!Instruction_memory[Register_file->PC].write_reg0_or_reg1) {
		Register_file->regs[rd] = Register_file->regs[rs] & Register_file->regs[rt];
	}
	PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
}

void or (RegisterFile *Register_file, InstructionMemory *Instruction_memory, int rd, int rs, int rt) {
	if (!Instruction_memory[Register_file->PC].write_reg0_or_reg1) {
		Register_file->regs[rd] = Register_file->regs[rs] | Register_file->regs[rt];
	}
	PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
}

void xor(RegisterFile *Register_file, InstructionMemory *Instruction_memory, int rd, int rs, int rt) {
	if (!Instruction_memory[Register_file->PC].write_reg0_or_reg1) {
		Register_file->regs[rd] = Register_file->regs[rs] ^ Register_file->regs[rt];
	}
	PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
}

void mul(RegisterFile *Register_file, InstructionMemory *Instruction_memory, int rd, int rs, int rt) {
	if (!Instruction_memory[Register_file->PC].write_reg0_or_reg1) {
		Register_file->regs[rd] = Register_file->regs[rs] * Register_file->regs[rt];

	}
	PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
}

/*/ sll function - if we want to shift by a number >= 32 -> the number become zero. if we want to shift by negative number -> the destination does not change (do nothing).*/
void sll(RegisterFile *Register_file, InstructionMemory *Instruction_memory, int rd, int rs, int rt) {
	if (!Instruction_memory[Register_file->PC].write_reg0_or_reg1) {
		if (Register_file->regs[rt] >= 0 && Register_file->regs[rt] < 32) {
			Register_file->regs[rd] = Register_file->regs[rs] << Register_file->regs[rt];
		}
		else if (Register_file->regs[rt] >= 32) {
			Register_file->regs[rd] = 0;
		}
	}
	PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
}

/*/ sla function - if we want to shift by 32 and bigger -> the number become zero for positive numbers and become -1 (0xFFFFFF) for negative numbers.
	if we want to shift by negative number -> the destination does not change (do nothing).*/
void sra(RegisterFile *Register_file, InstructionMemory *Instruction_memory, int rd, int rs, int rt) {
	if (!Instruction_memory[Register_file->PC].write_reg0_or_reg1) {
		if (Register_file->regs[rt] >= 0 && Register_file->regs[rt] < 32) {
			Register_file->regs[rd] = Register_file->regs[rs] >> Register_file->regs[rt];
		}
		else if (Register_file->regs[rt] >= 32) {
			if (Register_file->regs[rs] >= 0) {
				Register_file->regs[rd] = 0;
			}
			else {
				Register_file->regs[rd] = -1;
			}
		}
	}
	PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
}

/*/ srl function - if we want to shift by a number >= 32 -> the number become zero. if we want to shift by negative number -> the destination does not change (do nothing).*/
void srl(RegisterFile *Register_file, InstructionMemory *Instruction_memory, int rd, int rs, int rt) {
	if (!Instruction_memory[Register_file->PC].write_reg0_or_reg1) {
		if (Register_file->regs[rt] >= 0 && Register_file->regs[rt] < 32) {
			Register_file->regs[rd] = (int)((unsigned int)Register_file->regs[rs] >> Register_file->regs[rt]);
		}
		else if (Register_file->regs[rt] >= 32) {
			Register_file->regs[rd] = 0;
		}
	}
	PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
}

void beq(RegisterFile *Register_file, InstructionMemory *Instruction_memory, int rd, int rs, int rt) {
	int mask = 0x3FF;//we need to check it////
	if (Register_file->regs[rs] == Register_file->regs[rt]) {
		Register_file->PC = Register_file->regs[rd] & mask;
	}
	else {
		PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
	}
}

void bne(RegisterFile *Register_file, InstructionMemory *Instruction_memory, int rd, int rs, int rt) {
	int mask = 0x3FF;
	if (Register_file->regs[rs] != Register_file->regs[rt]) {
		Register_file->PC = Register_file->regs[rd] & mask;
	}
	else {
		PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
	}
}

void blt(RegisterFile *Register_file, InstructionMemory *Instruction_memory, int rd, int rs, int rt) {
	int mask = 0x3FF;
	if (Register_file->regs[rs] < Register_file->regs[rt]) {
		Register_file->PC = Register_file->regs[rd] & mask;
	}
	else {
		PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
	}
}

void bgt(RegisterFile *Register_file, InstructionMemory *Instruction_memory, int rd, int rs, int rt) {
	int mask = 0x3FF;
	if (Register_file->regs[rs] > Register_file->regs[rt]) {
		Register_file->PC = Register_file->regs[rd] & mask;
	}
	else {
		PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
	}
}

void ble(RegisterFile *Register_file, InstructionMemory *Instruction_memory, int rd, int rs, int rt) {
	int mask = 0x3FF;
	if (Register_file->regs[rs] <= Register_file->regs[rt]) {
		Register_file->PC = Register_file->regs[rd] & mask;
	}
	else {
		PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
	}
}

void bge(RegisterFile *Register_file, InstructionMemory *Instruction_memory, int rd, int rs, int rt) {
	int mask = 0x3FF;
	if (Register_file->regs[rs] >= Register_file->regs[rt]) {
		Register_file->PC = Register_file->regs[rd] & mask;
	}
	else {
		PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
	}
}

void jal(RegisterFile *Register_file, InstructionMemory *Instruction_memory, int rd, int rs, int rt) {
	int mask = 0x3FF;
	Register_file->regs[15] = Register_file->PC + 1;
	if (Instruction_memory[Register_file->PC].is_imm_cmd) {
		Register_file->regs[15] += 1;
	}
	Register_file->PC = Register_file->regs[rd] & mask;
}

void lw(RegisterFile *Register_file, InstructionMemory *Instruction_memory, DataMemory *Data_memory, int rd, int rs, int rt) {
	if (!Instruction_memory[Register_file->PC].write_reg0_or_reg1) {
		Register_file->regs[rd] = DMEM(Data_memory, Register_file->regs[rs] + Register_file->regs[rt], 0, "r");
	}
	PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
}

void sw(RegisterFile *Register_file, InstructionMemory *Instruction_memory, DataMemory *Data_memory, int rd, int rs, int rt) {
	DMEM(Data_memory, Register_file->regs[rs] + Register_file->regs[rt], Register_file->regs[rd], "w");
	PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
}

#if 0
void reti(RegisterFile *Register_file) {
	Register_file->PC = Register_file->IOregs[7];
	Register_file->busy_with_interrupt = false;
}

void in(RegisterFile *Register_file, InstructionMemory *Instruction_memory, FILE *hwregtrace, int rd, int rs, int rt) {
	int num_IOreg = Register_file->regs[rs] + Register_file->regs[rt];
	if (num_IOreg < 0 || num_IOreg > NUM_OF_IO_REGS - 1) { // handle attempt to access an invalid IOreg
		num_IOreg = abs(num_IOreg % NUM_OF_IO_REGS);
	}
	int data = Register_file->IOregs[num_IOreg];
	if (!Instruction_memory[Register_file->PC].write_reg0_or_reg1) {
		Register_file->regs[rd] = data;
	}
	print_hwregtrace(hwregtrace, "READ", Register_file->IOregs[clks], num_IOreg, data);
	PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
}

void out(RegisterFile *Register_file, InstructionMemory *Instruction_memory, DataMemory *Data_memory, HardDrive *hard_drive, FILE *hwregtrace, Monitor *monitor, int rd, int rs, int rt) {
	int num_IOreg = Register_file->regs[rs] + Register_file->regs[rt];
	if (num_IOreg < 0 || num_IOreg > NUM_OF_IO_REGS - 1) { // handle attempt to access an invalid IOreg
		num_IOreg = abs(num_IOreg % NUM_OF_IO_REGS);
	}
	int data = Register_file->regs[rd];
	if (!Instruction_memory[Register_file->PC].write_readonly_IOregs) {
		if ((num_IOreg >= irq0enable && num_IOreg <= irq2status) || num_IOreg == timerenable || num_IOreg == diskstatus || num_IOreg == monitorcmd) {
			// if there is an attempt to write invalid data to this IOreg -> write 0.
			if (data != 1 && data != 0) {
				data = 0;
			}
		}
		else if (num_IOreg == irqhandler || num_IOreg == irqreturn) { // if there is an attempt to write invalid data to this IOreg -> write data modulu 1024 (IMEMIN length).
			if (data < 0 || data > MAX_LEN_IMEM - 1) {
				data = abs(data % MAX_LEN_IMEM);
			}
		}
		else if (num_IOreg == diskcmd) { // if there is an attempt to write invalid data to this IOreg -> write 0.
			if (data != 1 && data != 0 && data != 2) {
				data = 0;
			}
		}
		else if (num_IOreg == disksector) { // if there is an attempt to write invalid data to this IOreg -> write data modulu 128 (diskin length).
			if (data < 0 || data > NUM_OF_WORDS_IN_SECTOR - 1) {
				data = abs(data % NUM_OF_WORDS_IN_SECTOR);
			}
		}
		else if (num_IOreg == diskbuffer) { // if there is an attempt to write invalid data to this IOreg -> write data modulu 4096 (DMEMIN length).
			if (data < 0 || data > MAX_LEN_DMEM - 1) {
				data = abs(data % MAX_LEN_DMEM);
			}
		}
		else if (num_IOreg == monitorx) { // if there is an attempt to write invalid data to this IOreg -> write data modulu X_PIXELS (352)
			if (data < 0 || data > X_PIXELS - 1) {
				data = abs(data % X_PIXELS);
			}
		}
		else if (num_IOreg == monitory) { // if there is an attempt to write invalid data to this IOreg -> write data modulu Y_PIXELS (288)
			if (data < 0 || data > Y_PIXELS - 1) {
				data = abs(data % Y_PIXELS);
			}
		}
		else if (num_IOreg == monitordata) { // if there is an attempt to write invalid data to this IOreg -> write data modulu GRAY_SCALE (256)
			if (data < 0 || data > GRAY_SCALE - 1) {
				data = abs(data % GRAY_SCALE);
			}
		}
		Register_file->IOregs[num_IOreg] = data;
	}
	print_hwregtrace(hwregtrace, "WRITE", Register_file->IOregs[clks], num_IOreg, data);
	if (Register_file->IOregs[monitorcmd] == 1) {
		monitor_handle(Register_file, monitor);
	}
	if (Register_file->IOregs[diskcmd] == 1 || Register_file->IOregs[diskcmd] == 2) {
		disk_handle(Register_file, hard_drive, Data_memory);
	}
	PC_incrementation(Register_file, Instruction_memory[Register_file->PC].is_imm_cmd);
}
// input - a pointer to a data memory type variable, a pointer to an instruction_memory type variable, a pointer to a register file type variable, a pointer to a monitor type variable, a pointer to a hard drive type variable and a file pointer to the file "hwregtrace".
//output - none
//the function executes an instruction according to the opcode using a function for each opcode.
void execute_instruction(RegisterFile *Register_file, DataMemory *Data_memory, InstructionMemory *Instruction_memory, Monitor *monitor, HardDrive *hard_drive, FILE *hwregtrace) {
	int opcode = Instruction_memory[Register_file->PC].instruction.opcode;
	int rd = Instruction_memory[Register_file->PC].instruction.rd;
	int rs = Instruction_memory[Register_file->PC].instruction.rs;
	int rt = Instruction_memory[Register_file->PC].instruction.rt;
	bool is_imm_cmd = Instruction_memory[Register_file->PC].is_imm_cmd;
	switch (opcode) {
	case 0:
		add(Register_file, Instruction_memory, rd, rs, rt);
		break;
	case 1:
		sub(Register_file, Instruction_memory, rd, rs, rt);
		break;
	case 2:
		and (Register_file, Instruction_memory, rd, rs, rt);
		break;
	case 3:
		or (Register_file, Instruction_memory, rd, rs, rt);
		break;
	case 4:
		xor (Register_file, Instruction_memory, rd, rs, rt);
		break;
	case 5:
		mul(Register_file, Instruction_memory, rd, rs, rt);
		break;
	case 6:
		sll(Register_file, Instruction_memory, rd, rs, rt);
		break;
	case 7:
		sra(Register_file, Instruction_memory, rd, rs, rt);
		break;
	case 8:
		srl(Register_file, Instruction_memory, rd, rs, rt); //check!
		break;
	case 9:
		beq(Register_file, Instruction_memory, rd, rs, rt);
		break;
	case 10:
		bne(Register_file, Instruction_memory, rd, rs, rt);
		break;
	case 11:
		blt(Register_file, Instruction_memory, rd, rs, rt);
		break;
	case 12:
		bgt(Register_file, Instruction_memory, rd, rs, rt);
		break;
	case 13:
		ble(Register_file, Instruction_memory, rd, rs, rt);
		break;
	case 14:
		bge(Register_file, Instruction_memory, rd, rs, rt);
		break;
	case 15:
		jal(Register_file, Instruction_memory, rd, rs, rt);
		break;
	case 16:
		lw(Register_file, Instruction_memory, Data_memory, rd, rs, rt);
		break;
	case 17:
		sw(Register_file, Instruction_memory, Data_memory, rd, rs, rt);
		break;
	case 18:
		reti(Register_file);
		break;
	case 19:
		in(Register_file, Instruction_memory, hwregtrace, rd, rs, rt);
		break;
	case 20:
		out(Register_file, Instruction_memory, Data_memory, hard_drive, hwregtrace, monitor, rd, rs, rt);
		break;
	case 21: // create function that exits simulator!
		break;
	default:
		/*/ in case we have to deal with invalid opcode, the current data in regs does not change
			and we just increment PC according the type of the instruction./*/
		PC_incrementation(Register_file, is_imm_cmd);
	}
}
#endif
// input - file pointer to the file to which we wish to print and a pointer to a register file type variable
// output - none
// the function prints the next line in to the file "trace" in the format specified
void print_trace(FILE *trace, RegisterFile *Register_file) {
	fprintf(trace, "%03X %s", Register_file->PC, Register_file->INST);//converting the PC address to 3 hex digits
	for (int i = 0; i < NUM_OF_REGS; i++) {
		fprintf(trace, " %08X", Register_file->regs[i]);//converting the regs to 8 hex digits
	}
	fprintf(trace, "\n");
}
#if 0
// input - a pointer to a register file type variable
// output - none
// the function handles the timer interrupt management/
void timer_handle(RegisterFile *register_file) {
	if (register_file->IOregs[timercurrent] > pow(2, 32) - 1) {
		register_file->IOregs[timercurrent] = 0;
	}
	if (register_file->IOregs[timerenable] == 1) {
		if (register_file->IOregs[timercurrent] == register_file->IOregs[timermax]) {
			register_file->IOregs[irq0status] = 1;
			register_file->IOregs[timercurrent] = 0;
		}
		else {
			register_file->IOregs[timercurrent] ++;
		}
	}
}

void clks_handle(RegisterFile *register_file) {
	if (register_file->IOregs[clks] > pow(2, 32) - 1) {
		register_file->IOregs[clks] = 0;
	}
	else {
		register_file->IOregs[clks]++;
	}
}

// input - a pointer to a register file type variable a file pointer to the irq2_in file and an integer indicating the current irq2 value
// output - none
// the function reads the next value of clock cycles written in the file "irq2_in" upon request and saves it.
void irq2in_read(RegisterFile *register_file, FILE *irq2_in, int *current_irq2) {
	if (!feof(irq2_in)) {
		fscanf(irq2_in, "%d", current_irq2);
	}
}
// input - a pointer to a register file type variable a file pointer to the irq2_in file and an integer indicating the current irq2 value
// output - none
// the function checks if the current clock cycle should activate an irq2 interrupt
void irq2in_handle(RegisterFile *register_file, int *current_irq2, FILE *irq2_in) {
	if (register_file->IOregs[clks] == *current_irq2) {
		register_file->IOregs[irq2status] = 1;
		irq2in_read(register_file, irq2_in, current_irq2);
	}
}

// input - a pointer to a register file type variable and a file pointer to the leds file
// output - none
// the function handles the leds by checking wether the leds status chenged and prints to the leds file if so.
void leds_handle(RegisterFile *register_file, FILE *ledsfile) {
	if (register_file->prev_leds != register_file->IOregs[leds]) {
		fprintf(ledsfile, "%d %08X\n", register_file->IOregs[clks], register_file->IOregs[leds]);
	}
	register_file->prev_leds = register_file->IOregs[leds];
}
// input - a pointer to a register file type variable
// output - none
// the functions handles an interrupt in terms of updating the PC to the interrupt PC and updating IO registers.
void interrupt_handle(RegisterFile *register_file) {
	register_file->busy_with_interrupt = true;
	register_file->IOregs[irqreturn] = register_file->PC;
	register_file->PC = register_file->IOregs[irqhandler];
}
// input - a pointer to a register file type variable
// output - a boolian variable indicating an inerrupt
// the function checks whether an interrupt have been activated in a certein clock cycle.
bool is_interrupt(RegisterFile *register_file) {
	bool irq = false;
	irq = (register_file->IOregs[irq0status] && register_file->IOregs[irq0enable]) || (register_file->IOregs[irq1status] && register_file->IOregs[irq1enable]) || (register_file->IOregs[irq2status] && register_file->IOregs[irq2enable]);
	return irq;
}
#endif 0
// input - a pointer to a data memory type variable, a pointer to an instruction_memory type variable, a pointer to a register file type variable, a pointer to a monitor type variable, a pointer to a hard drive type variable, a file pointer to the file "hwregtrace"  a file pointer to the file "leds" and a file pointer to the file "trace".
// ouput - none
// the function carries all the operations need to be done while executing an instruction
void handle_instruction(DataMemory *data_memory, InstructionMemory *instruction_memory, RegisterFile *register_file, FILE *trace, Monitor *monitor, HardDrive *hard_drive, FILE* hwregtrace, FILE* leds_file) {
	if (instruction_memory[register_file->PC].is_imm_cmd) {
		register_file->regs[1] = instruction_memory[register_file->PC].imm;
	}
	print_trace(trace, register_file);
	register_file->instructions_count++;
	clks_handle(register_file);
	disk_check(register_file, hard_drive);
	timer_handle(register_file);
	attempt_change_read_only_regs(register_file, instruction_memory);
	execute_instruction(register_file, data_memory, instruction_memory, monitor, hard_drive, hwregtrace);
	leds_handle(register_file, leds_file);
}

// input - a pointer to the "trace" file to pass to internal functions (such as "execute instruction")
// an Instruction Memory variable which holds the instructions for the functions to read
// a register File variable in which we have all register values and other variables (such as PC)
// a Data Memory variable which holds the data memory for the function to use while executing instructions
// an "irq2_in" file pointer to check for interrupts
// a hard drive variable which holds disk's content to use for disk interrupts
// a "leds" pointer file to use in case of leds activation
// a "hwregtrace" pointer file to use for tracing IO regs values
// output - none
// this function is the core of the simulator's operation, it manages the execution of instructions. the function passes through instructions (which are saved as a "instruction memory" variable) and executes the instructions whilst managing interuppts handling.
void IMEMIN_pass(FILE *trace, InstructionMemory *instruction_memory, RegisterFile *register_file, DataMemory *data_memory, FILE *irq2_in, Monitor *monitor, HardDrive *hard_drive, FILE *leds_file, FILE *hwregtrace) {
	int current_irq2;
	irq2in_read(register_file, irq2_in, &current_irq2);
	while (instruction_memory[register_file->PC].is_valid && instruction_memory[register_file->PC].instruction.opcode != 21) {
		irq2in_handle(register_file, &current_irq2, irq2_in);
		strcpy(register_file->INST, instruction_memory[register_file->PC].INST);
		if (is_interrupt(register_file) && !register_file->busy_with_interrupt) { // there is an interrupt and we are not busy with another interrupt
			if (instruction_memory[register_file->PC].is_imm_cmd) { // there is an interrupt but we in a middle of imm instruction
				handle_instruction(data_memory, instruction_memory, register_file, trace, monitor, hard_drive, hwregtrace, leds_file);
				interrupt_handle(register_file);
				clks_handle(register_file);
				disk_check(register_file, hard_drive);
				timer_handle(register_file);
				register_file->IOregs[irq2status] = 0;
				leds_handle(register_file, leds_file);
				continue;
			}
			else {
				interrupt_handle(register_file);
			}
		}
		else {
			int current_PC = register_file->PC;
			handle_instruction(data_memory, instruction_memory, register_file, trace, monitor, hard_drive, hwregtrace, leds_file);
			if (instruction_memory[current_PC].is_imm_cmd) {
				irq2in_handle(register_file, &current_irq2, irq2_in);
				if (is_interrupt(register_file) && !register_file->busy_with_interrupt) {
					interrupt_handle(register_file);
				}
				clks_handle(register_file);
				disk_check(register_file, hard_drive);
				timer_handle(register_file);
				register_file->IOregs[irq2status] = 0;
				leds_handle(register_file, leds_file);
			}
		}
	}
	strcpy(register_file->INST, instruction_memory[register_file->PC].INST);
	handle_instruction(data_memory, instruction_memory, register_file, trace, monitor, hard_drive, hwregtrace, leds_file);
	return;
}

// input - a file pointer to the imemin file, a pointer to an instruction memory type variable and a poiter to a register file type variable
// output - none
// the function reads from imemin and saves the instruction memory into a destined instruction memory variable
void read_imemin(FILE *file, InstructionMemory* Instruction_Memory, RegisterFile *Register_file) {
	char line[LEN_IMEMIN_LINE + 2];
	int instructions_count = 0;
	while (!feof(file) && fgets(line, LEN_IMEMIN_LINE + 2, file)) {
		if (is_empty_line(line)) {
			continue;
		}
		copy_str_by_delimiter(line, Instruction_Memory[instructions_count].INST, '\n');
		get_opcode(&Instruction_Memory[instructions_count].instruction.opcode, Instruction_Memory[instructions_count].INST);
		get_reg(&Instruction_Memory[instructions_count].instruction.rd, Instruction_Memory[instructions_count].INST + 2);
		get_reg(&Instruction_Memory[instructions_count].instruction.rs, Instruction_Memory[instructions_count].INST + 3);
		get_reg(&Instruction_Memory[instructions_count].instruction.rt, Instruction_Memory[instructions_count].INST + 4);
		Instruction_Memory[instructions_count].is_valid = true;
		if (is_imm_command(Instruction_Memory[instructions_count].INST, Instruction_Memory[instructions_count].instruction.opcode)) {
			Instruction_Memory[instructions_count].is_imm_cmd = true;
			fgets(line, LEN_IMEMIN_LINE + 2, file);
			instructions_count++;
			copy_str_by_delimiter(line, Instruction_Memory[instructions_count].INST, '\n');
			get_imm(&Instruction_Memory[instructions_count - 1].imm, line);
			Instruction_Memory[instructions_count].is_imm_cmd = false;
			Instruction_Memory[instructions_count].is_valid = false;
		}
		instructions_count++;
	}
}
#if 0
// input - pointer to a file from which we want to read the data and a Data Memory veriable
// output - none
// a function that gets the data from "dmemin" and saves it in a destined variable of type Data Memory
void read_dmemin(FILE *file, DataMemory *Data_memory) {
	char line[LEN_DMEMIN_LINE + 2];
	int i = 0;
	while (!feof(file) && fgets(line, LEN_DMEMIN_LINE + 2, file)) {
		sscanf(line, "%x\n", &Data_memory[i].data);
		i++;
	}
}

#endif
// input - pointer to a file to which we want to print and a Data Memory veriable
// output - none
// a function that prints the data memory into a destiny file
void print_dmemout(FILE *dmemout, DataMemory *Data_memory) {
	for (int i = 0; i < MAX_LEN_DMEM; i++) {
		fprintf(dmemout, "%08X\n", Data_memory[i].data);
	}
}

#if 0
// input - pointer to a file from which we want to read the data and a Hard Drive variable
// output - none
// a function that gets the data from "diskin" and saves it in a destined variable of type Hard Drive
void read_diskin(FILE *file, HardDrive *Hard_Drive) {
	char line[LEN_DMEMIN_LINE + 2];
	for (int i = 0; i < NUM_OF_SECTORS; i++) {
		for (int j = 0; j < NUM_OF_SECTORS; j++) {
			if (!feof(file) && fgets(line, LEN_DMEMIN_LINE + 2, file)) {
				sscanf(line, "%x\n", &(Hard_Drive->sectors[i][j]));
			}
			else {
				return;
			}
		}
	}
}
// input - pointer to a file to which we want to print and a Hard Drive variable
// output - none
// a function that prints the hard drive's content at the end of the simulator's operation
void print_diskout(FILE *diskout, HardDrive *hard_drive) {
	for (int i = 0; i < NUM_OF_SECTORS; i++) {
		for (int j = 0; j < NUM_OF_WORDS_IN_SECTOR; j++) {
			fprintf(diskout, "%08X\n", hard_drive->sectors[i][j]);
		}
	}
}
#endif
// input - pointer to a file to which we want to print and a Register_File variable
// output - none
// a function that prints the values of all registers in the end of the code to the destiny file
void print_regout(FILE *regout, RegisterFile Register_File) {
	for (int i = 2; i < NUM_OF_REGS; i++) {
		fprintf(regout, "%08X\n", Register_File.regs[i]);
	}
}
// input - pointer to a file to which we want to print and a Register_File variable
// output - none
// a function that prints the number of cycles and instructions executed in the end of the simulator's operation
void print_cycles(FILE *cycles, RegisterFile Register_File) {
	fprintf(cycles, "%d\n", Register_File.IOregs[clks]);
	fprintf(cycles, "%d\n", Register_File.instructions_count);
}

#if 0
// input - pointer to a file to which we want to print and a monitor variable
// output - none
// a function that prints the monitor into the destiny file
void print_monitor(FILE *monitor_file, Monitor *monitor) {
	for (int i = 0; i < Y_PIXELS; i++) {
		for (int j = 0; j < X_PIXELS; j++) {
			fprintf(monitor_file, "%02X\n", monitor->frame_buffer[j][i]);
		}
	}
}
// input - pointer to a file to which we want to print and a monitor variable
// output - none
// a function that prints the monitor into the destiny file of .yuv type
void print_monitor_yuv(FILE *monitor_yuv_file, Monitor *monitor) {
	for (int i = 0; i < Y_PIXELS; i++) {
		for (int j = 0; j < X_PIXELS; j++) {
			fwrite(&(monitor->frame_buffer[j][i]), 1, sizeof(monitor->frame_buffer[j][i]) / sizeof(int), monitor_yuv_file);
		}
	}
}

#endif