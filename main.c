#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>

#define ENTER 'x'
#define SPACE ' '

typedef enum{
	true = 1,
	TRUE = 1,
	false = 0,
	FALSE = 0
}Boolean;

struct Turtle{
	char * options[5];		// Operations the turtle can perform
	int nextOperation;		// Next operation that the turtle will perform
	int value;				// The value parameter of the next operation
};

struct Buffer
{
	char input;
	char db[22];
	int index;
};

/*	Convert string to int value */
int stringToInt(char string[]){
	char * endptr;
	int base = 10;

	return strtoimax(string,&endptr,base);
}

/* 	Check if the command exists in turtle->operations 
* 	if command found then:
*	Reset buffer->index = 0
*	Set turtle->nextOperation
*	set turtle->value
*/
Boolean getCommand(struct Buffer * buffer, struct Turtle * turtle){
		// Get string before space 
	int size = buffer->index; 
	char command[buffer->index];
	char value[] = {0,0,0};
	
	for (int i = 0; i < size; ++i)
	{
		// Extract command
		if(buffer->db[i] != SPACE){
			command[i] = buffer->db[i];
		}else{
			// Get value after space (value)
			// strncpy(dest, src + beginIndex, endIndex - beginIndex);
			strncpy(value, buffer->db + (i+1), size - i);
		}
	}

	// Convert string to integer
	int intValue = stringToInt(value);
	

	printf("Extracted command: %s\n", command);
	printf("Extracted str value: %s\n", value);
	printf("Extracted int value: %d\n", intValue);
	
	// Check if the command exists
	Boolean commandFound = false;
	for (int operationIndex = 0; operationIndex < 5; ++operationIndex){
		if(strcmp(turtle->options[operationIndex],command) == 0){
			// Set nextOperation
			turtle->nextOperation = operationIndex;
			// Set operation value
			turtle->value = intValue;
			// Reset char counter
			buffer->index = 0;
			commandFound = true;
			break;
		}
	}

	return commandFound;
}

/* Add the input character from the user to the buffer */
void addInputChar(struct Buffer * buffer){

	int index = buffer->index;
	char input = buffer->input;

	// Add character to the db
	buffer->db[index] = input;
	// Increase charCounter
	buffer->index += 1;
}

/* Check if the input character from the user was ENTER */
/* OBS!!! char parameter c should be the last element in buffer->db[buffer->index] :) */
Boolean isInputEnter(char c){
	Boolean isInputEnter = false;
	// Check if char is enter (change x --> enter ascii sign)
	if(c == ENTER){
		isInputEnter = true;
	}
	return isInputEnter;
}


int main()
{
	// All commands
	struct Turtle turtle;
	turtle.options[0] = "forward";
	turtle.options[1] = "left";
	turtle.options[2] = "right";
	turtle.options[3] = "penup";
	turtle.options[4] = "pendown";

	struct Buffer buffer;

	buffer.index = 0;

	// User input will be saved  in this var
	// TESTING !!!!!!!
	buffer.input = 'r';
	addInputChar(&buffer);
	buffer.input = 'i';
	addInputChar(&buffer);
	buffer.input = 'g';
	addInputChar(&buffer);
	buffer.input = 'h';
	addInputChar(&buffer);
	buffer.input = 't';
	addInputChar(&buffer);
	buffer.input = ' ';
	addInputChar(&buffer);
	buffer.input = '5';
	addInputChar(&buffer);
	//strcpy(buffer.db, "Farhad");
	buffer.input = ENTER;
	addInputChar(&buffer);

	printf("%s\n", buffer.db);
	
	char lastCharInBuffer = buffer.input;
	
	if(isInputEnter(lastCharInBuffer)){
		// Check if the command exists, if true, set value
		if(getCommand(&buffer, &turtle) == true){
			printf("Next operation is: %s\n", turtle.options[turtle.nextOperation]);
		}else{
			printf("%s --> Command does not exist\n", buffer.db);
		}
	}else{
		// Add character to buffer
	}

  	return 0;
}