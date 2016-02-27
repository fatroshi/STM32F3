/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include "usart.h"
#include "gpio.h"

//-------------------------------------------------//
//      User: lib includes
//-------------------------------------------------//
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <stdlib.h>

//-------------------------------------------------//
//      User: Define Constants
//-------------------------------------------------//
#define ENTER 'x'
#define SPACE ' '
#define OPERATIONS 6
#define OPERATION_LIST 20
#define EMPTY OPERATION_LIST+1
#define BUFFER_LENGTH 200
#define CHAR_INTERRUPT_SIZE 1

//-------------------------------------------------//
//      User: Global variables
//-------------------------------------------------//
char input[1];

//-------------------------------------------------//
//      User: Structs & Enums
//-------------------------------------------------//
typedef enum{
	true = 1,
	TRUE = 1,
	false = 0,
	FALSE = 0
}Boolean;

struct Turtle{
	char * options[OPERATIONS];             // Operations the turtle can perform
	char * feedback[OPERATIONS];		// Feed back response to the user
	char * errors[3];			// Errors that could occur

	int operations[OPERATION_LIST];		// List of all operations added by user
	int values[OPERATION_LIST];		// Values for each operation
	int index;				// Reference counter for knowing quantity of the operations
	int N;					// Reapeat value for commands between [ c1 v1 c2 v2 ... cn vn]
};

struct Buffer
{
	char input;				// Input char from terminal
	char db[BUFFER_LENGTH];		        // Buffer storing the inputs from terminal
	int index;				// Is used for the buffer, ex Buffer->db[index++] = 'a'
};

//-------------------------------------------------//
//      User: Turtle Fucntions
//-------------------------------------------------//
void forward (int disetance){
  printf("forward(disetance)\n");
}

void rotate (int decrease){
  if(decrease < 0){
    // Rotate left
  }else{
    // Rotate right
  }

  printf("rotate(decrease)\n");
}

void penDown(){
  printf("penDown()\n");
}

void penUp(){
  printf("penUp()\n");
}

//-------------------------------------------------//
//      User: Turtle Fucntions
//-------------------------------------------------//
/*	Convert string to int value */
int stringToInt(char string[]){
	char * endptr;
	int base = 10;

	return strtoimax(string,&endptr,base);
}
// Add task to the task list
Boolean addTask(char command[], char value[], struct Turtle * turtle){
  Boolean foundCommand = false;
  // Check if the command exists
  for (int operationIndex = 0; operationIndex < OPERATIONS; ++operationIndex){
    if(strcmp(turtle->options[operationIndex],command) == 0){
      
      foundCommand = true;
      
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

// Return true if the command exists 
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

// Print out the tasks in the list
void printTasks(struct Turtle * turtle){
    int size = turtle->index;

    for (int i = 0; i < size; ++i)
    {
        int index = turtle->operations[i];
        printf("%s %d\n", turtle->options[index], turtle->values[i]);
    }
}

// Check if the userinput contains valid commands
Boolean isValidInput(struct Buffer * buffer, struct Turtle * turtle){
  Boolean commandValid = true;
  
  // Tmp variables
  int bufferLength = buffer->index;
  char data[BUFFER_LENGTH];

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
          //hr();
          printf("%s --> %s\n", tmpCommand, turtle->errors[0]); 
          //hr();
          //
          commandValid = false;
          //break;
        }
      }else if(inputChar == ENTER){
        //Stop the buffer scan
        break;
      }

      // Reset charCounter
      charCounter = 0;
      spaces++;
      continue;
    }

    // Store data
    data[charCounter++] = inputChar;
    data[charCounter+1] = '\0';

  }
  return commandValid;
}

// Get all the commands from the user input (buffer->db)
void getCommands(struct Buffer * buffer, struct Turtle * turtle){
  // Tmp variables
  int bufferLength = buffer->index;
  char data[BUFFER_LENGTH];
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
        //printf("command %s value: %s \n", tmpCommand, data)
        printf("%s %s\n", tmpCommand, data);
      }
      // Reset charCounter
      charCounter = 0;
      spaces++;
      continue;
    }else if(inputChar == ENTER){
      // Add command and value to operation list
      addTask(tmpCommand, data, turtle);
      
      // Stop the buffer scan
      break;
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

// Return true if we have task in the list
Boolean taskExists(struct Turtle * turtle){
  Boolean taskExists = true;
  if(turtle->operations[0] == EMPTY){
    taskExists = false;
  }
  return taskExists;
}

// Remove task from the list
void removeTask(struct Turtle * turtle){
  // Remove from operations list
  for (int i = 1; i < OPERATION_LIST; ++i)
  {
          turtle->operations[i-1] = turtle->operations[i];
  }
  // Set last element as EMPTY
  turtle->operations[OPERATION_LIST-1] = EMPTY;

  // Remove for values list
  for (int i = 1; i < OPERATION_LIST; ++i)
  {
          turtle->values[i-1] = turtle->values[i];
  }
  // Set last element as EMPTY
  turtle->values[OPERATION_LIST-1] = EMPTY;

  // Set decrease index
  if(turtle->index > 0){
          turtle->index -=1;
  }
}

// Handle all tasks
void taskHandler(struct Turtle * turtle){
  // Check if we have task to do
  if(taskExists(turtle) == true){
    // Check if the first task is "repeat"
    int operationIndex = turtle->operations[0];
    if(strcmp(turtle->options[operationIndex], "repeat") == 0){
      hr();
      // Repeat function
      turtle->N = turtle->values[0];
      printf("Found repeat function with N = %d \n", turtle->N);
      // line break
      hr();
      // Remove task
      removeTask(turtle);
    }else{
      turtle->N = 1;
    }

    // Do all task N times
    for (int i = 0; i < turtle->N; ++i)
    {
      // This function should be replaced by a function that performs all the task!
      //
      printTasks(turtle);
      //hr();
    }

  }
}

// Set turtle values
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





ITStatus UartReady = SET;

void SystemClock_Config(void);

/** * @brief  Rx Transfer completed callback * @param  UartHandle: UART handle 
* @note   This example shows a simple way to report end of IT Rx transfer, and  
*         you can add your own implementation. * @retval None */ 
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) {   
  /* Set transmission flag: trasfer complete*/  
  
  HAL_UART_Transmit_IT(&huart3, (uint8_t *)input, CHAR_INTERRUPT_SIZE);
  UartReady = SET; 

} 

int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();

  // USER CODE
  // Turtle object
  struct Turtle * turtle  = (struct Turtle *)malloc(sizeof(struct Turtle)); 
  // Init. turtle struct
  initTurtle(turtle);

  // Buffer object
  struct Buffer * buffer = (struct Buffer *)malloc(sizeof(struct Buffer)); 
  //buffer = (struct buffer *) malloc(sizeof(struct buffer));	
  buffer->index = 0;
  
  
  while (1)
  {
    if(UartReady == SET){
      if(HAL_UART_Receive_IT(&huart3, (uint8_t *)input, CHAR_INTERRUPT_SIZE) !=HAL_OK){
        // Take care of errors
      }
      // 
      UartReady = RESET;
    }
    
    /*
    // Get the input
    if(UartReady == RESET){
      // Get the input (one char) from the user and att to the buffer
      buffer->input = input[0];
      addInputChar(buffer);
      
      HAL_UART_Transmit_IT(&huart3, (uint8_t *)buffer->db, CHAR_INTERRUPT_SIZE);
      
      // Check if the input was enter
      if(isInputEnter(input[0])){
        // Check if the command exists, if true, set value
        if(isValidInput(buffer,turtle) == true){	
          // Get all commands and att to task list
          getCommands(buffer, turtle);
        }
      }
    }
    */
    //HAL_UART_Receive(&huart3, &Tecken, 1, 5000);     
    //HAL_UART_Transmit(&huart3, &Tecken, 1, 5000); 
  }

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
