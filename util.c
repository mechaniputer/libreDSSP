/*	This file is part of libreDSSP.

	Copyright 2019 Alan Beadle

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
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "util.h"
#include "dict.h"
#include "elem.h"
#include "stack.h"

// Deals with ."hello" print statements
void textPrint(char * text){
	assert(text != NULL);
	int i;
	int len = strlen(text) - 1;

	for(i = 2; i < len; i++){
		printf("%c", text[i]);
	}
	return;
}

int isNum(char * foo){
	int i = 0;

	if((foo == NULL) || (strlen(foo) == 0)) return 0;

	// If first is minus, scan from beginning to see if all are digits
	if((foo[0] == '-') && (strlen(foo) > 1)){
		for (i=1; i < (strlen(foo)); i++){
			if(!isdigit(foo[i])) return 0;
		}
	}else for (i=0; i < (strlen(foo)); i++){
		if(!isdigit(foo[i])) return 0;
	}
	return 1;
}

// Looks at cmdTop(cmdstack), decides what to do until cmdstack is empty
void run(stack * workStack, cmdstack * cmdstack, dict * vocab){
	command *temp;

	if((cmdstack->top) > -1) do{
		temp = cmdTop(cmdstack);
		assert(temp != NULL);

		//TODO: Deepen threading support
		if((temp->func) != NULL){
			cmdPop(cmdstack);
			temp->func(workStack, cmdstack, vocab);
		}else if(isNum(temp->text)){ // Numerical constant
			push(workStack, atoi(temp->text));
			cmdPop(cmdstack);

		}else if (!strcmp(temp->text, ":")){ // Function declaration
			defWord(cmdstack,vocab);

		}else if (temp->text[0] == '['){ // Comment
			cmdPop(cmdstack);

		}else if (!strncmp(temp->text, ".\"", 2)){
			textPrint(temp->text);
			cmdPop(cmdstack);

		}else wordRun(cmdstack, workStack, vocab); // word or variable

	}while((cmdstack->top)>=0);
	return;
}

// Takes command line and splits it by spaces, pushes it onto stack in reverse order
void stackInput(char * line, cmdstack * cmdstack){
	char ch;
	int i = 0;
	int j = 0;
	elem * seqprev;
	elem * seqtail;
	command * newcom;

	seqtail = malloc(sizeof(elem));
	seqtail->next = NULL;

	// TODO Not safe or efficient
	// Handles comments, ."hello" printing, general command strings
	while(line[j] != '\0'){
		ch = line[j++];

		if (ch == '[') {
			seqtail->chars[i++] = '[';
			while((ch = line[j++]) != ']'){
				if(i>80) break; // TODO This limits a word or comment to 80 chars due to fixed size in struct
				seqtail->chars[i++] = ch;
			}
			seqtail->chars[i++] = ']';

		} else if((line[j-1] =='.') && (line[j] == '\"')) {
			j++;
			seqtail->chars[i++] = '.';
			seqtail->chars[i++] = '\"';
			while((ch = line[j++]) != '\"'){
				if(i>80) break; // TODO This limits a word or comment to 80 chars due to fixed size in struct
				seqtail->chars[i++] = ch;
			}
			seqtail->chars[i++] = '\"';

		} else if(ch != ' ') { // All normal characters outside comments and print statements
			if(i>80) break; // TODO This limits a word or comment to 80 chars due to fixed size in struct
			seqtail->chars[i++] = ch;

		} else if ((ch == ' ') && (i != 0)) { // If (i == 0) then it's either an extra space or it follows a comment or ."hello" style print
			// Time to append a new sequence element
			seqtail->chars[i] = '\0';
			seqprev = seqtail;
			seqtail = malloc(sizeof(elem));
			seqtail->next = seqprev; // New tail points to prev to make reverse list
			i=0;
		}
	}

	// Get rid of pesky empty element
	if(i > 0){
		seqtail->chars[i] =  '\0';
	}else{
		seqprev = seqtail;
		seqtail = seqtail->next;
		free(seqprev);
	}

	// Now we push it all onto the cmdstack in reverse order
	while(seqtail != NULL){

		// Construct a new command
		newcom = malloc(sizeof(struct command));
		newcom->text = malloc((strlen(seqtail->chars)+1)*sizeof(char)); // This is bad
		strcpy(newcom->text, seqtail->chars);
		newcom->func = NULL;

		cmdPush(cmdstack, newcom);
		seqtail = seqtail->next;
	}
	return;
}

char * prompt(){
	char * line = readline ("* ");
	//Check for EOF.
	if (!line){
		printf("\n");
		return "BYE";
	}
	if(strcmp(line, "")) add_history(line);
	return line;
}

// If top of cmdstack is a word (or a variable), this function knows what to do
void wordRun(cmdstack * cmdstack, stack * workStack, dict * vocab){
	int i;
	command * cmdName = cmdPop(cmdstack);
	coreword * tempCore;
	word * tempWord;
	variable * tempVar;

	if(cmdName->text[0] == '\0') return;

	// Search core dict first
	tempCore = coreSearch(cmdName->text, vocab);
	if(tempCore != NULL){
		// Run built-in function. cmdstack provided so that conditionals can conditionally pop next command{s}
		tempCore->func(workStack, cmdstack, vocab);
		return;
	}

	// Now search all other dicts
	tempWord = wordSearch(cmdName->text, vocab);
	if(tempWord != NULL){
		// Push programmed word onto stack in reverse-order
		for(i = tempWord->length; i >= 0; i--){
			stackInput(tempWord->array[i].text, cmdstack);
			cmdstack->array[cmdstack->top].func = tempWord->array[i].func;
		}
		return;
	}

	tempVar = varSearch(cmdName->text, vocab);
	if (tempVar != NULL){ // It's a variable
		push(workStack,tempVar->value);
		return;
	}else{
		fprintf(stderr,"ERROR: %s unrecognized\n",cmdName->text);
		return;
	}
}

