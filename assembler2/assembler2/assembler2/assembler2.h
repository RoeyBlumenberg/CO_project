// import libraries
#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include <stdio.h>
#include <stdbool.h>

// define constants
#define OPCODES_NUM 19 // num of opcodes accroding to the project instructions
#define REGISTERS_NUM 16 // num of registers accroding to the project instructions
#define MAX_OPCODE_NAME_LEN 4 // max num of chars in instruction name
#define MAX_REG_NAME_LEN 5 // max num of chars in register name
#define MAX_LABEL_LEN 50   // max label length accroding to the project instructions
#define MAX_LINE_LEN 300  // max line length accroding to the project instructions
#define NUM_MEMIN_LINES 4096
#define IMM_BITS 20
#define DATA_BITS 32
#define FFFFF 0xFFFFF

typedef struct _Instruction
{
	char opcode[MAX_OPCODE_NAME_LEN + 1];
	char rd[MAX_REG_NAME_LEN + 1];
	char rs[MAX_REG_NAME_LEN + 1];
	char rt[MAX_REG_NAME_LEN + 1];
	char imm[MAX_LABEL_LEN + 1]; // imm can also contain label name so limited by max(MAX_LABEL_LEN, numdigits(2^20 - 1)) = MAX_LABEL_LEN.
	bool is_imm_instruction;
	bool is_valid;  // indicates whether the struct contains meaningful data or not
} Intruction;

typedef struct _Word_instruction
{
	int address; // the address of the Word_instruction
	int data;
	bool is_valid;  // indicates whether the struct contains meaningful data or not
} Word_instruction;

typedef struct _Label
{
	char name[MAX_LABEL_LEN + 1];
	int address;
	bool is_valid;  // indicates whether the struct contains meaningful data or not
} Label;

FILE *open_file(char *file_name, char *mode);
void close_file(FILE *file);
void first_pass(FILE *file, Label Labels[]);
void second_pass(FILE *file, Intruction instructions[], Word_instruction words[]);
void print_memin(FILE *memin, Intruction instructions[], Label labels[]);
void init_Words(Word_instruction *Words);
void init_Instructions(Intruction *Instructions);
void init_Labels(Label *Labels);
#endif

