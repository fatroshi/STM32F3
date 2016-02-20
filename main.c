#include <stdio.h>
#include <string.h>


typedef enum{
	true = 1,
	TRUE = 1,
	false = 0,
	FALSE = 0
}Boolean;

struct Turtle{
	char * options[5];
	int nextOperation;
	
};

struct Buffer
{
	char input;
	char db[22];
	int index;
};


int commandExists(struct Buffer * buffer, struct Turtle * turtle){
	Boolean commandFound = false;

	for (int operationIndex = 0; operationIndex < 5; ++operationIndex)
	{
		if(strcmp(turtle->options[operationIndex],buffer->db) == 0){
			printf("Found command\n");
			// Set nextOperation
			turtle->nextOperation = operationIndex;
			// Reset char counter
			buffer->index = 0;
			commandFound = true;
			break;
		}
	}
	return commandFound;
}

void addInputChar(struct Buffer * buffer){

	int index = buffer->index;
	char input = buffer->input;
	
	// Add character to the db
	buffer->db[index] = input;
	// Increase charCounter
	buffer->index += 1;
}

Boolean isInputEnter(char c){
	Boolean isInputEnter = false;
	// Check if char is enter (change x --> enter ascii sign)
	if(c == 'x'){
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
	//strcpy(buffer.db, "Farhad");
	
	char c = 'x';

	if(isInputEnter(c)){
		// Check if the command exists
		if(commandExists(&buffer, &turtle) == true){
			printf("Next operation is: %s\n", turtle.options[turtle.nextOperation]);
		}
	}else{
		buffer.input = c;
		addInputChar(&buffer);
	}

  	return 0;
}