#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include "assembler.h"

int main(int argc, char *argv[]) {

	FILE *asm_file, *memin ;
	Label Labels[NUM_MEMIN_LINES];//changed
	Intruction Instructions[NUM_MEMIN_LINES];
	Word_instruction Words[NUM_MEMIN_LINES];
	init_Labels(Labels);
	init_Instructions(Instructions);
	init_Words(Words);
	asm_file =fopen( "fib.asm", "r");
	first_pass(asm_file, Labels);
	second_pass(asm_file, Instructions, Words);
	fclose(asm_file);
	// write to memin.txt
	memin = fopen("memim.txt", "w");
	print_memin(memin, Instructions, Labels);
	fclose(memin);
	// write to dmemin.txt
	//dmemin = open_file(argv[3], "w");
	//print_dmem(dmemin, Words);
	//close_file(dmemin);
	return 0;
}