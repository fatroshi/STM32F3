/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include "tim.h"
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
#define ENTER '\r'
#define SPACE ' '
#define OPERATIONS 6
#define OPERATION_LIST 20
#define EMPTY OPERATION_LIST+1
#define BUFFER_LENGTH 200
#define CHAR_INTERRUPT_SIZE 1
#define ONE_CYCLE 400
// OPERATION INDEX
#define FORWARD 0
#define LEFT 1
#define RIGHT 2
#define PEN_UP 3
#define PEN_DOWN 4
#define REPEAT 5
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
  char * feedback[OPERATIONS];            // Feed back response to the user
  char * feedbackDone[OPERATIONS];        // Feed back response to the user
  char * errors[3];                       // Errors that could occur

  int operations[OPERATION_LIST];         // List of all operations added by user
  int values[OPERATION_LIST];             // Values for each operation
  int index;                              // Reference counter for knowing quantity of the operations
  int N;                                  // Reapeat value for commands between [ c1 v1 c2 v2 ... cn vn]
};

struct Buffer
{
  char input;       // Input char from terminal
  char db[BUFFER_LENGTH];           // Buffer storing the inputs from terminal
  int index;        // Is used for the buffer, ex Buffer->db[index++] = 'a'
};

//-------------------------------------------------//
//      User: Global variables
//-------------------------------------------------//
char input[1];
Boolean interrupt = false;
//-------------------------------------------------//
//      User: Turtle Fucntions
//-------------------------------------------------//
void initEngine(){
  HAL_GPIO_WritePin(ENABLE_GPIO_Port,ENABLE_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MS1_GPIO_Port,MS1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MS2_GPIO_Port,MS2_Pin, GPIO_PIN_SET);
}

void sendPWM(int pulses){
  for(int i=0; i <= pulses; i++){
    HAL_GPIO_WritePin(STEP1_GPIO_Port,STEP1_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(STEP1_GPIO_Port,STEP1_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(STEP2_GPIO_Port,STEP2_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(STEP2_GPIO_Port,STEP2_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
  }
}

int distanceToPWM(int _distance){
  float diameter = 47;
  float circumference = diameter * 3.14;
  int diff = 5; // Diff with 5mm
  
  // Calculate number of pulses
  int pulses = (int)(_distance + diff) * ONE_CYCLE / circumference;
  
  return pulses;
}

void forward (int _distance){
  // init dir1 and dir 2
  HAL_GPIO_WritePin(DIR1_GPIO_Port,DIR1_Pin, GPIO_PIN_SET);             // LEFT
  HAL_GPIO_WritePin(DIR2_GPIO_Port,DIR2_Pin, GPIO_PIN_SET);             // RIGHT  

  // Send PWM to engines
  sendPWM(distanceToPWM(_distance));
}

void left(int degrees){
  // init dir1 and dir 2
  HAL_GPIO_WritePin(DIR1_GPIO_Port,DIR1_Pin, GPIO_PIN_SET);             // LEFT
  HAL_GPIO_WritePin(DIR2_GPIO_Port,DIR2_Pin, GPIO_PIN_RESET);           // RIGHT  

  // Calculate pulses
  float diameter = 120;
  float pi = 3.141592;
  float circumference = diameter * pi;   
  float distance = (circumference / 360) * degrees;
  int pulse = distanceToPWM((int)distance);
  
  // Send PWM to engines
  sendPWM(pulse);
}

void right(int degrees){
  // init dir1 and dir 2
  HAL_GPIO_WritePin(DIR1_GPIO_Port,DIR1_Pin, GPIO_PIN_RESET);             // LEFT
  HAL_GPIO_WritePin(DIR2_GPIO_Port,DIR2_Pin, GPIO_PIN_SET);               // RIGHT  


  // Calculate pulses
  float diameter = 120;
  float pi = 3.141592;
  float circumference = diameter * pi;   
  float distance = (circumference / 360) * degrees;
  int pulse = distanceToPWM((int)distance);
  
  // Send PWM to engines
  sendPWM(pulse);
}

void penDown(){
  sConfigOC.Pulse = (40000 * 3) / 100;
    // PWM
  /* Set the pulse value for channel 2 */
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    /* Configuration Error */
    //Error_Handler();
  }

  /* Start channel 2 */
  if (HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2) != HAL_OK)
  {
    /* PWM Generation Error */
    //Error_Handler();
  }
  
}

void penUp(){
  sConfigOC.Pulse = (40000 * 5) / 100;
  
  /* Set the pulse value for channel 2 */
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    /* Configuration Error */
    //Error_Handler();
  }

  /* Start channel 2 */
  if (HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2) != HAL_OK)
  {
    /* PWM Generation Error */
    //Error_Handler();
  }
}

//-------------------------------------------------//
//      User: Fucntions
//-------------------------------------------------//
/*  Convert string to int value */
int stringToInt(char string[]){
  char * endptr;
  int base = 10;

  return strtoimax(string,&endptr,base);
}
// Add task to the task list
Boolean addTask(char command[], char value[], struct Turtle * turtle){
  Boolean foundCommand = false;
  // Check if the command exists
  for (int operationIndex = 0; operationIndex < OPERATIONS; operationIndex++){
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
        printf("%s %d\n\r", turtle->options[index], turtle->values[i]);
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
        strcpy(tmpCommand, data);

        if(commandExists(tmpCommand, turtle) == false){
          printf("%s --> %s\n", tmpCommand, turtle->errors[0]); 
          //
          commandValid = false;
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
  
  // Reset buffer->index = 0 if invalid command was found
  if(commandValid == false){
    buffer->index = 0;
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
        //printf("%s %s\n", tmpCommand, data);
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
void removeTask(struct Buffer * buffer,struct Turtle * turtle){
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
  // Reset buffer character counter
  buffer->index = 0;
}

// Send feedback to user
void sendFeedback(int feedBackIndex, struct Turtle * turtle){
  HAL_Delay(500);
  int msgOk = HAL_UART_Transmit(&huart3, 
              (uint8_t *)turtle->feedback[feedBackIndex], 
              strlen(turtle->feedback[feedBackIndex]),
              1000);
  if(msgOk != HAL_OK){
    //printf("Could not send feedback: %s \n\r", turtle->feedback[feedBackIndex]);
  }else{
    //printf("Feedback send\n\r");
  }
}

void sendFeedbackDone(int feedBackIndex, struct Turtle * turtle){
  HAL_Delay(500);
  int msgOk = HAL_UART_Transmit(&huart3, 
              (uint8_t *)turtle->feedbackDone[feedBackIndex], 
              strlen(turtle->feedback[feedBackIndex]),
              1500);
  if(msgOk != HAL_OK){
    //printf("Could not send feedback done: %s \n\r", turtle->feedbackDone[feedBackIndex]);
  }else{
    //printf("Feedback done send\n\r");
  }
}

// Handle all tasks
void taskHandler(struct Buffer * buffer,struct Turtle * turtle){
  // Check if we have task to do
  if(taskExists(turtle) == true){
    // Check if the first task is "repeat"
    int operationIndex = turtle->operations[0];
    if(strcmp(turtle->options[operationIndex], "repeat") == 0){
      // Repeat function
      turtle->N = turtle->values[0];
      // Remove task
      removeTask(buffer, turtle);
      
      buffer->index = 0;
      printf("Found N = %d\n\r", turtle->N);
      
    }else{
      turtle->N = 1;
      printf("N not found \n\r");
    }
    
    // Print total operations in the list
    printf("Total tasks: %d \n\r", turtle->index);
    
    // Do all task N times
    for (int i = 0; i < turtle->N; ++i)
    {
      // This function should be replaced by a function that performs all the task!
      //printTasks(turtle);
      for(int j=0; j< turtle->index; j++){
        int operation = turtle->operations[j];
        int operationValue = turtle->values[j];
        // Perform the task
        switch(operation){
          case FORWARD:
            // Send feedback - command was accepted
            sendFeedback(FORWARD, turtle);
            // Perform task
            forward(operationValue);
            // Send feedback - operation is finished
            sendFeedbackDone(FORWARD, turtle);
            break;
          case LEFT:
            // Send feedback - command was accepted
            sendFeedback(LEFT, turtle);
            // Perform task
            left(operationValue);
            // Send feedback - operation is finished
            sendFeedbackDone(LEFT, turtle);
            break;
          case RIGHT:
            // Send feedback - command was accepted
            sendFeedback(RIGHT, turtle);
            // Perform task
            right(operationValue);
            // Send feedback - operation is finished
            sendFeedbackDone(RIGHT, turtle);
            break;
          case PEN_UP:
            // Send feedback - command was accepted
            sendFeedback(FORWARD, turtle);
            // Perform task
            penUp(operationValue);
            // Send feedback - operation is finished
            sendFeedbackDone(FORWARD, turtle);
            break;
          case PEN_DOWN:
            // Send feedback - command was accepted
            sendFeedback(PEN_DOWN, turtle);
            // Perform task
            penDown(operationValue);
            // Send feedback - operation is finished
            sendFeedbackDone(PEN_DOWN, turtle);
            break; 
          default:
            break;
        }
      } // End loop j
      
    } // End loop 
    
    // Remove all tasks from the list
    int index = turtle->index;
    for(int i= 0; i < index; i++){
      removeTask(buffer, turtle);
    }
    // Print total operations in the list
    printf("After loop -> Total tasks: %d \n\r", turtle->index);

  }
}

// Set turtle values
void initTurtle(struct Turtle * turtle){
  // Set value
  turtle->index = 0;
  // All commands
  turtle->options[FORWARD]      = "forward";
  turtle->options[LEFT]         = "left";
  turtle->options[RIGHT]        = "right";
  turtle->options[PEN_UP]       = "penup";
  turtle->options[PEN_DOWN]     = "pendown";
  turtle->options[REPEAT]       = "repeat";
  
  // Feedback response for each command
  turtle->feedback[FORWARD]     = "Going forward\r\n";
  turtle->feedback[LEFT]        = "Going left\r\n";
  turtle->feedback[RIGHT]       = "Going right\r\n";
  turtle->feedback[PEN_UP]      = "Pick up pen\r\n";
  turtle->feedback[PEN_DOWN]    = "Pick down the pen\r\n";  
  turtle->feedback[REPEAT]      = "Repeat function\r\n";  
  
  // Feedback response for each command
  turtle->feedbackDone[FORWARD]     = "Forward: done\r\n\r\n\r\n";
  turtle->feedbackDone[LEFT]        = "Left: done\r\n\r\n";
  turtle->feedbackDone[RIGHT]       = "Right: done\r\n\r\n";
  turtle->feedbackDone[PEN_UP]      = "Pen Up: done\r\n\r\n";
  turtle->feedbackDone[PEN_DOWN]    = "Pen Down: done\r\n\r\n";  
  turtle->feedbackDone[REPEAT]      = "Repeat: done\r\n\r\n";  
  
  // Error feedback
  turtle->errors[0]             = "Command does not exist";
  
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
  // SET flag
  UartReady = SET; 
  // SET flag
  interrupt = true;
} 

void wellcomeMsg(){
  char msg[] = "Hi turtle\r\n";
  HAL_UART_Transmit(&huart3,(uint8_t *)msg, strlen(msg),1000);
}

int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM4_Init();
  MX_USART3_UART_Init();

  // init Engine
  initEngine();
  
  /* User var */
  struct Turtle * turtle  = (struct Turtle *)malloc(sizeof(struct Turtle)); 
  // Init. turtle struct
  initTurtle(turtle);

  // Buffer object
  struct Buffer * buffer = (struct Buffer *)malloc(sizeof(struct Buffer)); 
  //buffer = (struct buffer *) malloc(sizeof(struct buffer)); 
  buffer->index = 0;
  
  // Send wellcome message to turtle
  wellcomeMsg();
  
  while (1)
  {
    if(UartReady == SET){
      if(HAL_UART_Receive_IT(&huart3, (uint8_t *)input, CHAR_INTERRUPT_SIZE) !=HAL_OK){
        // Take care of errors
      }
      // 
      UartReady = RESET;
    }
    
    
    // Get the input
    if(interrupt == true){
      // Get the input (one char) from the user and att to the buffer
      buffer->input = input[0];
      addInputChar(buffer);
      
      //HAL_UART_Transmit_IT(&huart3, (uint8_t *)buffer->input, CHAR_INTERRUPT_SIZE);
      
      // Check if the input was enter
      if(isInputEnter(input[0])){
        printf("\n\r");
        // Check if the command exists, if true, set value
        if(isValidInput(buffer,turtle) == true){  
          
          // Print out the input 
          printf("Input: %s \n\r", buffer->db);
          // Get all commands and att to task list
          getCommands(buffer, turtle);
          
          // Perform task/s
          taskHandler(buffer, turtle);
          
        }
      }
      
      //
      interrupt = false;
    }

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
