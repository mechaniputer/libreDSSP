/*	This file is part of libreDSSP.

	Copyright 2026 Alan Beadle

	libreDSSP is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libreDSSP is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libreDSSP.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>

#include "tokparse.h"
#include "dict.h"
#include "stack.h"
#include "cmdbuf.h"
#include "corewords.h"
#include "util.h"

#define VERSION "0.6.1"

extern char *optarg;
extern int optind;
cmdbuffer *cmdbuf;
stack *dataStack;
stack *returnStack;
stack * loopStack;
stack * tokenizerStack;
stack * parserStack;
dict * vocab; // Contains all recognized words, including core and user defined
int abort_requested = 0;

int main(int argc, char *argv[]){
	cmdbuf = newCmdBuffer();
	dataStack = newStack();
	returnStack = newStack();
	loopStack = newStack();
	tokenizerStack = newStack();
	parserStack = newStack();

	vocab = malloc(sizeof(dict));
	vocab->core = NULL;
	vocab->grow = NULL;
	vocab->undefined = NULL;

	// Add dictionary entries for every core word (corewords.c)
	define_all_core();

	// Sub-Dictionaries
	vocab->sub = malloc(sizeof(subdict)); // For user defined words, can add more dicts later
	vocab->sub->name = "$DEFAULT";
	vocab->sub->open = 1;
	vocab->sub->next = NULL;
	vocab->sub->wordlist = NULL;
	vocab->sub->varlist = NULL;
	vocab->grow = vocab->sub; // We will grow this dictionary by default

	// Parse args, set flags
	int opt_load_file = 0;      // Load a file, evaluate it, and close unless -i is passed
	int opt_eval_String = 0;    // Evaluate string between loading file and showing prompt (if interactive)
	int opt_print_help = 0;     // Print help/usage information and exit
	int opt_quiet_shell = 0;    // Do not print version/copyright before prompt
	int opt_interactive = 0;    // Keep shell open after evaluating file (if provided)
	int opt_version_info = 0;   // Print version information and exit
	char * fname = NULL;
	char *eval_string = NULL;

	// Arg parse loop
	int opt;
	while((opt = getopt(argc, argv, "f:e:hqiv")) != -1) {
		switch(opt) {
			case 'f':
				opt_load_file = 1;
				fname = optarg;
				break;
			case 'e':
				if (opt_eval_String) {
					fprintf(stderr, "%s: only one -e allowed\n", argv[0]);
					return -1;
				}
				opt_eval_String = 1;
				eval_string = strdup(optarg);
				break;
			case 'h':
				opt_print_help = 1;
				break;
			case 'q':
				opt_quiet_shell = 1;
				break;
			case 'i':
				opt_interactive = 1;
				break;
			case 'v':
				opt_version_info = 1;
				break;
			default:
				fprintf(stderr, "Usage: %s [-f filename] [-e string] [-h] [-q] [-i] [-v]\n", argv[0]);
				return -1;
		}
	}

	if (!opt_load_file && optind < argc) {
		if (optind + 1 < argc) {
			printf("ERR: %s: too many arguments\n", argv[0]);
			return -1;
		}
		opt_load_file = 1;
		fname = argv[optind];
	}
	int status = 0;

	if(opt_version_info) {
		printf("libreDSSP, version %s\n", VERSION);
		return 0;
	}else if(opt_print_help) {
		printf("Usage: %s [-f filename] [-e string] [-h] [-q] [-i] [-v]\n", argv[0]);
		printf("  -f filename   Load and evaluate a DSSP source file\n");
		printf("  -e string     Evaluate a string\n");
		printf("  -h            Print this help message and exit\n");
		printf("  -q            Suppress version/copyright information on startup\n");
		printf("  -i            Keep shell open after executing file (if provided)\n");
		printf("  -v            Print version information and exit\n");
		return 0;
	}

	// Print copyright/version unless told not to
	if(!opt_quiet_shell){
		printf("\nlibreDSSP, version %s\n", VERSION);
		printf("Copyright (C) 2026  Alan Beadle\n\nYou should have received a copy of the GNU General Public License\nalong with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n");
	}

	if(opt_load_file){
		char * bufptr = NULL;
		size_t bufsize = 0;
		size_t characters;
		errno = 0;
		FILE *file = fopen(fname, "r");

		if (file == NULL){
			printf("%s Failed to open file `%s`: [Errno %d] %s\n",argv[0], fname,errno, strerror(errno));
			return -1;
		}else{
			while(EOF != (characters = getline(&bufptr, &bufsize, file))){
				if (characters > 0 && bufptr[characters - 1] == '\n') {
					bufptr[characters - 1] = '\0';
				}
				status = process_line(bufptr); // word_next is called within
				//if(status == 0) word_next();
				free(bufptr);
				bufptr = NULL;
				bufsize = 0;
				//cmdbuf->size = 0;
				//cmdbuf->array[0] = NULL;
			}
			fclose(file);
			free(bufptr);
			bufptr = NULL;
		}
	}

	// Evaluate a string if provided
	if(opt_eval_String) {
		status = process_line(eval_string); // word_next is called within
		free(eval_string);
		//if(status == 0) word_next();
		//cmdbuf->size = 0;
		//cmdbuf->array[0] = NULL;
	}

	// We have evaluated a file and/or string if provided.
	// Unless -i was passed, we are done.
	if((opt_load_file || opt_eval_String) && !opt_interactive) {
		return 0; // If we loaded a file and are not interactive, exit after processing the file
	}

	while(1){
		assert(-1 == returnStack->top); // Ensure we have an empty return stack
		assert(-1 == loopStack->top); // Ensure we have an empty loop stack
		abort_requested = 0;
		// Show prompt, get line of input
		char * line = prompt(status);
		status = process_line(line); // word_next is called within
		free(line);
		//print_codewords(cmdbuf->array);
		//if(status == 0) word_next();
		//cmdbuf->size = 0;
		//cmdbuf->array[0] = NULL;
	}
	return 0;
}
