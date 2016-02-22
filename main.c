#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <stdlib.h>

#define ENTER 'x'
#define SPACE ' '
#define OPERATIONS 6
#define OPERATION_LIST 20
#define EMPTY OPERATION_LIST+1

typedef enum{
	true = 1,
	TRUE = 1,
	false = 0,
	FALSE = 0
}Boolean;

struct Turtle{
	char * options[OPERATIONS];				// Operations the turtle can perform
	char * feedback[OPERATIONS];			// Feed back response to the user
	char * errors[3];						// Errors that could occur

	int operations[OPERATION_LIST];			// List of all operations added by user
	int values[OPERATION_LIST];				// Values for each operation
	int index;								// Reference counter for knowing quantity of the operations
	int N;									// Reapeat value for commands between [ c1 v1 c2 v2 ... cn vn]
};

struct Buffer
{
	char input;								// Input char from terminal
	char db[1000];							// Buffer storing the inputs from terminal
	int index;								// Is used for the buffer, ex Buffer->db[index++] = 'a'
};


void hr(){
	printf("+-------------------------------------------------+\n");
}

/*	Convert string to int value */
int stringToInt(char string[]){
	char * endptr;
	int base = 10;

	return strtoimax(string,&endptr,base);
}

Boolean addTask(char command[], char value[], struct Turtle * turtle){
	Boolean foundCommand = false;
	
	// Check if the command exists
	for (int operationIndex = 0; operationIndex < OPERATIONS; ++operationIndex){
		if(strcmp(turtle->options[operationIndex],command) == 0){

			int index = turtle->index;
			int intValue = stringToInt(value);

			turtle->operations[index] = operationIndex;
			turtle->values[index] = intValue;
			turtle->index += 1;
			break;
		}
	}
	return foundCommand;
}

Boolean commandExists(char command[], struct Turtle * turtle){
	Boolean foundCommand = false;

	for (int i = 0; i < OPERATIONS; ++i)
	{
		if(strcmp(command,turtle->options[i]) == 0){
			foundCommand = true;
			break;
		}
	}

	return foundCommand;
}

void printTasks(struct Turtle * turtle){
	int size = turtle->index;

	for (int i = 0; i < size; ++i)
	{
		int index = turtle->operations[i];
		printf("%s %d\n", turtle->options[index], turtle->values[i]);
	}
}

Boolean isValidInput(struct Buffer * buffer, struct Turtle * turtle){
	Boolean commandFound = true;
	
	// Tmp variables
	int bufferLength = buffer->index;
	char data[bufferLength];

	int charCounter = 0;
	int spaces = 0;

	// Store values temporary
	char tmpCommand[20];
	for (int i = 0; i < bufferLength; ++i)
	{
		char inputChar = buffer->db[i];

		if(inputChar == '['){
			continue;
		}

		if(inputChar == SPACE){
			if(spaces % 2 == 0){
				// Command value
				// Check if command exist
				//printf("Command: %s\n", data);
				strcpy(tmpCommand, data);

				if(commandExists(tmpCommand, turtle) == false){
					// Print error: Command does not exist
					printf("%s --> %s\n", tmpCommand, turtle->errors[0]); 
					//
					commandFound = false;
					break;
				}
			}

			// Reset charCounter
			charCounter = 0;
			spaces++;
			continue;
		}else if(inputChar == ']'){
			//printf("Value: %s\n", data);
		}

		// Store data
		data[charCounter++] = inputChar;
		data[charCounter+1] = '\0';

	}

	return commandFound;
}

void getCommands(struct Buffer * buffer, struct Turtle * turtle){
	// Tmp variables
	int bufferLength = buffer->index;
	char data[bufferLength];
	int charCounter = 0;
	int spaces = 0;

	// Store values temporary
	char tmpCommand[20];
	for (int i = 0; i < bufferLength; ++i)
	{
		char inputChar = buffer->db[i];
		if(inputChar == '['){
			continue;
		}
		if(inputChar == SPACE){
			if(spaces % 2 == 0){
				// Command value
				strcpy(tmpCommand, data);
			}else{
				// Command value
				// Add command and value to operation list
				addTask(tmpCommand, data, turtle);
				//printf("command %s value: %s \n", tmpCommand, data);
			}
			// Reset charCounter
			charCounter = 0;
			spaces++;
			continue;
		}else if(inputChar == ']'){
			// Add command and value to operation list
			addTask(tmpCommand, data, turtle);

			//printf("command %s value: %s \n", tmpCommand, data);

		}
		// Store data
		data[charCounter++] = inputChar;
		data[charCounter+1] = '\0';
	}
}

/* Add the input character from the user to the buffer */
void addInputChar(struct Buffer * buffer){

	int index = buffer->index;
	char input = buffer->input;

	// Add character to the db
	buffer->db[index] = input;
	// Add end of string character
	buffer->db[index+1] = '\0';
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


Boolean taskExists(struct Turtle * turtle){
	Boolean taskExists = true;
	if(turtle->operations[0] == EMPTY){
		taskExists = false;
	}

	return taskExists;
}

void removeTask(struct Turtle * turtle){
	for (int i = 1; i < OPERATION_LIST; ++i)
	{
		turtle->operations[i-1] = turtle->operations[i];
	}
	// Set last element as EMPTY
	turtle->operations[OPERATION_LIST-1] = EMPTY;
	// Set decrease index
	if(turtle->index > 0){
		turtle->index -=1;
	}
}



void taskHandler(struct Turtle * turtle){
	// Check if we have task to do
	if(taskExists(turtle) == true){
		// Check if the first task is "repeat"
		int operationIndex = turtle->operations[0];
		if(strcmp(turtle->options[operationIndex], "repeat") == 0){
			// Repeat function
			turtle->N = turtle->values[0];
			printf("Found repeat function with N = %d \n", turtle->N);
			// line break
			hr();
			// Remove task
			//removeTask(turtle);
		}else{
			turtle->N = 1;
		}

		// Do all task N times
		for (int i = 0; i < turtle->N; ++i)
		{
			// This function should be replaced by a function that performs all the task!
			//
			printTasks(turtle);
			hr();
		}

	}
}

void initTurtle(struct Turtle * turtle){
	// Set value
	turtle->index = 0;
	// All commands
	turtle->options[0] = "forward";
	turtle->options[1] = "left";
	turtle->options[2] = "right";
	turtle->options[3] = "penup";
	turtle->options[4] = "pendown";
	turtle->options[5] = "repeat";
	
	// Feedback response for each command
	turtle->feedback[0] = "Going forwad";
	turtle->feedback[1] = "Going left";
	turtle->feedback[2] = "Going right";
	turtle->feedback[3] = "Pick up pen";
	turtle->feedback[4] = "Pick down the pen";	
	turtle->feedback[5] = "Repeat function";	

	// Error feedback
	turtle->errors[0]	= "Command does not exist";
	// Init task list
	for (int i = 0; i < OPERATION_LIST; ++i)
	{
		turtle->operations[i] = EMPTY;
	}

}

/* This function is only for testing should be removed  later */
void userInput(char string[], struct Buffer * buffer){
	for (int i = 0; i < (int)strlen(string); ++i)
	{
		char c = string[i];
		buffer->input = c;
		addInputChar(buffer);
		
	}
	//printf("%s\n", buffer->db);
}

int main()
{
	// Turtle object
	struct Turtle * turtle  = (struct Turtle *)malloc(sizeof(struct Turtle)); 
	// Init. turtle struct
	initTurtle(turtle);

	// Buffer object
	struct Buffer * buffer = (struct Buffer *)malloc(sizeof(struct Buffer)); 
	//buffer = (struct buffer *) malloc(sizeof(struct buffer));	
	buffer->index = 0;

	// User input will be saved  in this var
	// TESTING !!!!!!!
	char input[] = "repeat 3 [right 12 left 13 forward 22 forward 11 pendown 1 left 99]";
	userInput(input, buffer);

	//printf("Buffer: %s\n", buffer->db);
	printf("input: %s\n", input);
		
	buffer->input = ENTER;
	addInputChar(buffer);
	char lastCharInBuffer = buffer->input;
	
	if(isInputEnter(lastCharInBuffer)){
		// Found Enter 
		// Check if the command exists, if true, set value
		if(isValidInput(buffer,turtle) == true){	
			// Get all commands and att to task list
			getCommands(buffer, turtle);
		}
	}else{
		// Add character to buffer
	}
	
	hr();
	taskHandler(turtle);
	removeTask(turtle);
  	return 0;
}