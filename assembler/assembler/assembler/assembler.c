#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "assembler.h"

// Input - file name to open and opening mode: "r" or "w"
// Output - File pointer
// Function - opening the file name with the expected mode
FILE *open_file(char *file_name, char *mode)
{
	FILE *file;
	file = fopen(file_name, mode);
	if (file == NULL)
	{
		printf("Error while opening the file\n");
		exit(1);
	}
	return file;
}

// Input - The file that we want to close
// Output - nothing.
// Function - Closing the file. Check if the pointer in not NULL first.
void close_file(FILE *file_name)
{
	if (file_name == NULL) {
		printf("Error while closing the file");
		exit(1);
	}
	fclose(file_name);
	return;
}


// Input - a pointer to an array of .word instructions.
// Output - nothing.
// Function - Initializing the is_valid field of the word struct to false.
void init_Words(Word_instruction *Words)
{
	int i = 0;
	for (i; i < NUM_MEMIN_LINES; i++) {
		Words[i].is_valid = false;
	}
	return;
}

// Input - a pointer to an array of instructions.
// Output - nothing.
// Function - that initialize the array by setting: is valid -> false, is_imm_instruction -> false.
void init_Instructions(Intruction *Instructions)
{
	int i = 0;
	for (i; i < NUM_MEMIN_LINES; i++) {
		Instructions[i].is_valid = false;
		Instructions[i].is_imm_instruction = false;
	}
	return;
}

// Input - a pointer to an array of labels.
// Output - nothing.
// Function -  that initialize the array by setting: is valid -> false.
void init_Labels(Label *Labels)
{
	int i = 0;
	for (i; i < NUM_MEMIN_LINES; i++) {
		Labels[i].is_valid = false;
	}
	return;
}

// Input - string.
// Output - returns lowercase version of the input string.
// Function - make sure all the letters are lowercase.
void to_lower(char *str)
{
	int i = 0;
	while (str[i] != '\0') {
		str[i] = tolower(str[i]);
		i++;
	}
}

// Input - string that contains decimal number or hexadecimal number.
// Output - true if the number is hexadecimal, else false.
// FUnction - to make sure that the number is hexa number.
bool is_hex(char *str)
{
	to_lower(str);
	if (*str == '0' && *(str + 1) == 'x') {
		return true;
	}
	return false;
}

// Input - name of opcode, such as: "add", "sub", "mul" etc.
// Output - corresponding opcode number.
//Function - recives name of opcode and returns the int opcode number.
int get_opcode(char *opcode) {
	int i = 0;
	to_lower(opcode);
	char *opcodes[] = { "add", "sub", "mul", "and", "or", "xor", "sll", "sra", "srl", "beq", "bne", "blt", "bgt", "ble", "bge", "jal",
							"lw", "sw", "halt" };
	for (i = 0; i < OPCODES_NUM; i++) {
		if (strcmp(opcode, opcodes[i]) == 0) {
			return i;
		}
	}
	printf("There is an incorrect command!, replaced with exit command - 'halt'\n"); // in case we got an invalid opcode -> return opcode = halt.
	return 18;
}

// Input - name of registr, such as: "$zero", "$imm", "$t0" etc.
// Output - corresponding register number.
//Function that recives name of register and returns the cooresponding register number.
int get_register(char *reg) {
	int i = 0;
	to_lower(reg);
	char *registers[16] = { "$zero", "$imm", "$v0", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$s0", "$s1", "$s2", "$gp", "$sp", "$ra" };
	for (i = 0; i < REGISTERS_NUM; i++) {
		if (strcmp(reg, registers[i]) == 0) {
			return i;
		}
	}
	printf("There is an incorrect register name!, replaced with $zero\n"); // in case we got an invalid register -> return register %zero.
	return 0;
}


// Input - string that contains decimal number or hexadecimal number.
// Output - returns int value of the number.
//Function that recives string that contains number, check wheter the number is decimal or hexadecimal and returns the cooresponding int number.
int get_imm(char *imm) {
	int imm_value = 0;
	to_lower(imm);
	if (is_hex(imm)) {
		sscanf(imm, "%x", &imm_value);
		return imm_value;
	}
	else {
		sscanf(imm, "%d", &imm_value);
		return imm_value;
	}
}

// Input - string "imm" that contains a label name and an array of labels we create while the first pass over the .asm file.
// Output - returns the address of that label.
// Function that recives string that contains a label name, iterating over our array of labels until finds the corresponding label and returns its address number.
int get_label(char *imm, Label labels[]) {
	int i = 0;
	while (labels[i].is_valid) {
		if (strcmp(labels[i].name, imm) == 0) {
			return labels[i].address;
		}
		i++;
	}
	return -1;
}

// Input - source string, destination string and an end char - until that char we want to copy the source string to the destination string.
// Output - nothing, change the destination string given as input.
// Function that copy the source string to the destination string until a specific char reached.
void copy_str_by_delimiter(const char *source, char *destination, char end_char) {
	int i = 0;
	for (i = 0; source[i] != end_char && source[i] != '\0'; i++) {
		destination[i] = source[i];
	}
	destination[i] = '\0';
}

// Input - string.
// Output - returns version of the input string without whitespaces.
// Function that delete all whitespaces in the input string.
char *delete_whitespaces(char *str) {
	char* temp = str;
	int str_index = 0;
	int temp_index = 0;
	while (str[str_index] != '\0') {
		if (!isspace(str[str_index])) {
			temp[temp_index] = str[str_index];
			temp_index++;
		}
		str_index++;
	}
	temp[temp_index] = '\0';
	return temp;
}

// Input - arg1 and arg2 indicate if line contains :, # accordingly, arg3 and arg4 are pointers to the first occurence of :, # in line, accordingly.
// Output - true if the line is label line, else false.
// Function - Helps us to know if Label line by: if it contains (: and !#).
bool is_Label(bool contains_colon, bool contains_hash /*,char* colon_ptr, char* hash_ptr*/) {
	return (contains_colon && !contains_hash);
}

// Input - a char we want to be searched, line to be scanned and NULL pointer.
// Output - returns true if the line contains char_to_find, false otherwise. Also the function change the pointer to point to the first occurence of char_to_find.
// Function that check whether line cotains a specific char and update the pointer to that char.
bool is_contain_char(char* line, char* char_to_find, char** char_ptr) {
	*char_ptr = strstr(line, char_to_find);
	return (*char_ptr != NULL);
}

// Input - arg1 and arg2 indicate if line contains #, $imm accordingly, arg3 and arg4 are pointers to the first occurence of #, $imm in line, accordingly.
// Output - returns true if the line is imm command, else false.
bool is_imm_command(char *line, bool contains_hash, bool contains_imm, char* imm_ptr, char* hash_ptr) {
	char *halt_ptr = strstr(line, "halt");
	char *jal_ptr = strstr(line, "jal");
	char *comma_ptr = strstr(line, ",");
	bool halt_command = (!contains_hash && halt_ptr != NULL) || (contains_hash && halt_ptr != NULL && halt_ptr < hash_ptr);
	bool jal_command = (!contains_hash && jal_ptr != NULL) || (contains_hash && jal_ptr != NULL && jal_ptr < hash_ptr);
	if (halt_command) {
		return false;
	}
	else if (jal_command) {
		if (contains_imm == true) {
			return (imm_ptr < comma_ptr);
		}
		else {
			return false;
		}
	}
	else {
		return (!contains_hash && contains_imm) || (contains_hash && contains_imm && imm_ptr < hash_ptr);
	}
}

// Input - line string.
// Output - returns true if the line is empty, else false.
// Function that search if line contains only whitespaces.Search for: ' ' - space , '\n' - newline , '\t' - horizontal tab ,'\v' - vertical tab ,
//'\f' - form feed , '\r' - Carraige return
bool is_empty_line(char *line) {
	for (int i = 0; line[i] != '\0'; i++) {
		if (!isspace(line[i])) {
			return false;
		}
	}
	return true;
}
#if 0
// Input - line string and a boolean variable which tells us if the line contains label.
// Output - returns true if the line cotains label and command, for examaple: "L1:  add &s0, &zero, %zero, 0", else false.
bool is_label_and_command(char *line, bool is_label) {
	char *dollar_ptr = strstr(line, "$");
	char *word_ptr = strstr(line, ".word");
	char *hash_ptr = strstr(line, "#");
	char *colon_ptr = strstr(line, ":");
	if (hash_ptr != NULL) {
		if (is_label && (dollar_ptr != NULL && dollar_ptr < hash_ptr)) {
			return true;
		}
		else
			if (is_label && (dollar_ptr != NULL && dollar_ptr > colon_ptr)) {
				return true;
			}
	}
	return false;
}

// Input - line string and a bollean var which tells us if the line contains label.
// Output - returns true if the line cotains label and .word command, for examaple: "L1:  .word 1024 7", else false.
bool is_label_and_word_command(char *line, bool is_label) {
	char *word_ptr = strstr(line, ".word");
	char *hash_ptr = strstr(line, "#");
	char *colon_ptr = strstr(line, ":");
	if (hash_ptr != NULL) {
		if (is_label && (word_ptr != NULL) && (word_ptr < hash_ptr)) {
			return true;
		}
		else
			if (is_label && (word_ptr != NULL) && (word_ptr > colon_ptr)) {
				return true;
			}
	}
	return false;
}
#endif
// Input - opcode number and string that contains imm value.
// Output - returns true if the line cotains label as an operand, else false.
/*/ Function that checks whether the opcode may contain label as an operand (for example branch instructions) and check if the imm value is a label name or a constant numeric value
by checking if its first character is alphabet. /*/
bool is_label_operand(char *imm) {
	delete_whitespaces(imm);
	if (isalpha(imm[0])) {
		return true;
	}
	else {
		return false;
	}
}

// Input - string line, a boolean var which tells us if the string line contains "#" and a pointer to the first occurence of that "#".
// Output - returns true if the line is .word instruction, else false.
// Function that finds the first occurence of ".word" and checks whether it appears before the first occurence of "#" (start of comment).
bool is_word_instruction(const char *line, bool contains_hash, char *hash_ptr) {
	while (isspace(*line)) {
		line++;
	}
	char *word_ptr = strstr(line, ".word"); // pointer to .word part in line
	if (word_ptr == NULL) {
		return false;
	}
	else {
		if (contains_hash) {
			return (word_ptr < hash_ptr);
		}
		else {
			return true; // if there is no # in line but we have .word -> it is a .word command
		}
	}
}

// Input - string.
// Output - returns the string from the first non whitspace char.
// Function that iterates over the input string until a non whitespace char is found.
int jump_over_whitespaces(char *str) {
	int i = 0;
	while (isspace(str[i])) {
		i++;
	}
	return i;
}

// Input - line string, pointer to instruction struct and a boolean var that tells us whether the line contains an imm command.
// Output - nothing, but change the instruction struct by using it's pointer.
// Function to handle line instruction and parse the line to the relevant fields: opcode, rd, rs, rt, imm.
void handle_line_instruction(char *line, Intruction *instruction, bool is_imm_command) {	char *temp = delete_whitespaces(line);	char *dollar_ptr = NULL;	is_contain_char(temp, "$", &dollar_ptr);	copy_str_by_delimiter(temp, instruction->opcode, '$');	strcpy(instruction->rd, strtok(dollar_ptr, ","));	strcpy(instruction->rs, strtok(NULL, ","));//in order to get next token and to continue with the same string NULL is passed as first argument since strtok maintains a static pointer to your previous passed string:	strcpy(instruction->rt, strtok(NULL, ","));	strcpy(instruction->imm, strtok(NULL, "#"));	if (is_imm_command) {		instruction->is_imm_instruction = true;	}	else {		instruction->is_imm_instruction = false;	}	instruction->is_valid = true;}


// Input - line string, pointer to .word struct.
// Output - nothing, but change the .word struct by using it's pointer.
// Function to handle line instruction and parse the line to the relevant fields: address, data.
void handle_word_instruction(char *line, Word_instruction *word) {
	char temp[MAX_LINE_LEN + 2];
	to_lower(line);
	int i = jump_over_whitespaces(line);
	strcpy(temp, line + i);
	if (strcmp(".word", strtok(temp, " ")) == 0) {
		strcpy(temp, (strtok(NULL, " "))); //copying from word to #
		strcpy(temp, temp + jump_over_whitespaces(temp));//we want to ignore the spaces before the address
		if (is_hex(temp)) {
			sscanf(temp, "%x", &word->address);
		}
		else {
			sscanf(temp, "%d", &word->address);
		}
		strcpy(temp, strtok(NULL, "#"));
		delete_whitespaces(temp);
		if (is_hex(temp)) {
			sscanf(temp, "%x", &word->data);
		}
		else {
			sscanf(temp, "%d", &word->data);
		}
	}
	else {
		printf("No such command\n");
		return;
	}
	word->is_valid = true;
	return;
}

// Input - file pointer and an array of label structs.
// Output - nothing, but change the array of labels.
/*/ Function in order to do the first pass over the .asm file. During that function we collect the labels and theirs addresse by updating the PC according the type of the instruction:
	if the instruction uses $imm -> PC +=2, else PC+=1. /*/
void first_pass(FILE *file, Label Labels[]) {
	int i = 0, PC = 0;
	bool contains_hash = false, contains_colon = false, contains_imm = false, is_label = false;
	char *hash_ptr = NULL, *colon_ptr = NULL, *imm_ptr = NULL;
	char line[MAX_LINE_LEN + 2]; // to keep space for '\n' and '\0'
	char lower_case_line[MAX_LINE_LEN + 2]; // to keep space for '\n' and '\0'
	Label label;
	while (!feof(file) && fgets(line, MAX_LINE_LEN + 2, file)) {
		if (is_empty_line(line)) { // go over empty lines
			continue;
		}
		strcpy(lower_case_line, line);
		to_lower(lower_case_line); // we make a copy of line with lower case chars in order to parse the line correctly
		contains_hash = is_contain_char(lower_case_line, "#", &hash_ptr);
		contains_imm = is_contain_char(lower_case_line, "$imm", &imm_ptr);
		contains_colon = is_contain_char(lower_case_line, ":", &colon_ptr);
		if (is_Label(contains_colon, contains_hash/*, colon_ptr, hash_ptr*/)) {
			is_label = true;
			copy_str_by_delimiter(line, label.name, ':');
			label.address = PC;
			label.is_valid = true;
			Labels[i] = label;
			i++;
		}
		if (!is_label) {
			if (is_imm_command(lower_case_line, contains_hash, contains_imm, imm_ptr, hash_ptr)) {
				PC += 2;
			}
			else if (is_word_instruction(lower_case_line, contains_hash, hash_ptr)) {
				PC += 0; // .word command does not increment our PC
			}
			else {
				PC += 1;
			}
		}
		contains_hash = false;
		contains_colon = false;
		contains_imm = false;
		is_label = false;
	}
	rewind(file); // in order to read from start of the assembler file in the second pass
	return;
}

// Input - file pointer, array of instruction structs, array of .word instruction structs
// Output - nothing, but change the array of instructions and the array of .word instructions.
/*/ Function in order to do the second pass over the .asm file. During that function we collect the instructions, parsing them using 'handle_line_instruction' function,
	collect the .word instructions. and parsing them  using 'handle_word_instruction'. In the second pass we ignore lines that contains only labels/*/
void second_pass(FILE *file, Intruction instructions[], Word_instruction Words[]) {
	int num_instruction = 0;
	int PC = 0;
	bool contains_hash = false, contains_colon = false, contains_imm = false, is_label = false, is_imm_instruction = false;
	char *hash_ptr = NULL, *colon_ptr = NULL, *imm_ptr = NULL, *dollar_ptr = NULL;
	char line[MAX_LINE_LEN + 2]; // to keep space for '\n' and '\0'
	char lower_case_line[MAX_LINE_LEN + 2]; // to keep space for '\n' and '\0'
	Intruction *instruction = (Intruction*)malloc(sizeof(Intruction));
	if (instruction == NULL) { // check malloc
		printf("memory allocation failed");
		exit(1);
	}
	Word_instruction *word = (Word_instruction*)malloc(sizeof(Word_instruction));
	if (word == NULL) { // check malloc
		printf("memory allocation failed");
		exit(1);
	}
	while (!feof(file) && fgets(line, MAX_LINE_LEN + 2, file)) {
		if (is_empty_line(line)) { // go over empty lines
			continue;
		}
		strcpy(lower_case_line, line);
		to_lower(lower_case_line); // we make a copy of line with lower case chars in order to parse the line correctly
		contains_hash = is_contain_char(lower_case_line, "#", &hash_ptr);
		contains_imm = is_contain_char(lower_case_line, "$imm", &imm_ptr);
		contains_colon = is_contain_char(lower_case_line, ":", &colon_ptr);
		is_label = is_Label(contains_colon, contains_hash, colon_ptr, hash_ptr);
		is_imm_instruction = is_imm_command(lower_case_line, contains_hash, contains_imm, imm_ptr, hash_ptr);
		if (is_label) {
			continue;
		}
		else { // the line contains an instruction.
			if (is_word_instruction(lower_case_line, contains_hash, hash_ptr)) {
				handle_word_instruction(line, word);
				Words[word->address] = *word; // add new .word instruction to Words array.
			}
			else {
				handle_line_instruction(line, instruction, is_imm_instruction);
				instructions[num_instruction] = *instruction; // add new intruction to MEMIN
				num_instruction++;
			}
		}
	}
	free(instruction); // free dynamic allocation
	free(word); // free dynamic allocation
	return;
}

// Input - file pointer, array of instruction structs, array of label structs
// Output - nothing, but print to the stream pointed by the pointer.
/*/ Function that iterates over the array of intructions and print each instruction and its fields in hexadecimal format using 5 digits. Also if the instruction contains register $imm,
	we print the imm value (whether it is a label address or a constant numeric value) in an additional line/*/
void print_memin(FILE *memin, Intruction instructions[], Label labels[]) {
	int i = 0;
	int opcode, rd, rs, rt, imm;
	bool contains_label_operand = false;
	while (instructions[i].is_valid) {
		opcode = get_opcode(instructions[i].opcode);
		rd = get_register(instructions[i].rd);
		rs = get_register(instructions[i].rs);
		rt = get_register(instructions[i].rt);
		contains_label_operand = is_label_operand(instructions[i].imm);
		if (contains_label_operand) {
			imm = get_label(instructions[i].imm, labels);
		}
		else {
			imm = get_imm(instructions[i].imm);
		}
		fprintf(memin, "%02X%X%X%X\n", opcode, rd, rs, rt);
		if (instructions[i].is_imm_instruction) {
			fprintf(memin, "%05X\n", imm & FFFFF); // $FFFFF in order to deal with negative numbers.
		}
		i++;
	}
}
#if 0
// Input - file pointer, array of .word instruction structs.
// Output - nothing, but print to the stream pointed by the pointer.
/*/ Function that iterates over the array of .word intructions and print each instruction and its fields in hexadecimal format using 8 digits.
	if no .word instruction has been used we print 00000000 /*/
void print_dmem(FILE *dmemin, Word_instruction Words[]) {
	int i = 0;
	for (i; i < NUM_DMEM_LINES; i++) {
		if (Words[i].is_valid) {
			fprintf(dmemin, "%08X\n", Words[i].data & FFFFFFFF); // $FFFFF in order to deal with negative numbers.
		}
		else
		{
			fprintf(dmemin, "00000000\n");
		}
	}
	return;
}
#endif