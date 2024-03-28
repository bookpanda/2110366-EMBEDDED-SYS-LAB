#include<bits/stdc++.h>
using namespace std;

int main(void) {
    //settings > preferences > keys > content assist

    //How to look at port chart: outer=code name | inner=in-board name | e.g. PA5(code) corresponds to D13(inner)

    //Lab 1
    while (1)
    {
        /* USER CODE END WHILE */
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(200);
        /* USER CODE BEGIN 3 */
    }
    //the GPIO_PIN_5 is LD2 LED (center of board)

    //Lab 2.1 same as Lab 1

    //Lab 2.2 external LED (Do not connect both ends on same vertical line on breadboard, it won't work)
    //LED: bigger side = negative
    //PA5 is already configured to have GPIO_Output in ioc file
    //Say we want to use PA1, which corresponds to A1 on board, we have to set PA1 in ioc file to be GPIO_Output as well
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
    HAL_Delay(200);

    //Lab 2.3 push button to toggle LED
    if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) { // C13 is linked to blue push button
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	    HAL_Delay(50);
		while(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {}
	}

    //Lab 2.4 UART
    //ioc
    //Connectivity > USART2 aka USB, see Baud Rate = 115200 bits/s

    // Mac
    // ls /dev/tty.* #search for port
    // screen //port// 115200 #connect to port with buad rate
    // screen -ls #list all active serial port
    // screen -X -S //identifier// quit #quit that serial port

    //For Windows, device manager > COM port
    //open PuTTY, serial, hostname=COM3, speed=115200
    char c[1];
    while(1) {
        if(HAL_UART_Receive(&huart2, c, 1, 100) == HAL_OK) {
            HAL_UART_Transmit(&huart2, c, 1, 100);
            if(c[0] == '\r') { // no need for if-else idk why
		        HAL_UART_Transmit(&huart2, "\n", 1, 100);
		    }
        }
    }

    //Lab 2.5 UART on/off command to LED
    void uartPrint(const char* msg) {
    	HAL_UART_Transmit(&huart2, msg, strlen(msg), 100);
    }

    void uartPrintln(const char* msg) {
        uartPrint(msg);
        uartPrint("\r\n");
    }

    uint8_t uartInput[10];
    char line[100];
    int length = 0;
    while (1)
    {
        HAL_StatusTypeDef status = HAL_UART_Receive(&huart2, uartInput, 1, 100);
        if(status == HAL_OK) {
            HAL_UART_Transmit(&huart2, uartInput, 1, 100);
            line[length++] = uartInput[0];

            if(length > 100) {
                length = 0;
                uartPrintln("\r\nLength limit exceeded, overwriting line!");
                continue;
            }

            if(uartInput[0] == '\r') {
                uartPrint("\n");
                line[length] = '\0'; //mark end of line, or else it will scan old inputs
                length = 0;

                if(!strcmp(line, "on\r")) {
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
                    uartPrintln("LED is ON");
                } else if(!strcmp(line, "off\r")) {
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
                    uartPrintln("LED is OFF");
                } else {
                    uartPrintln("Command not found");
                }
            }
        }
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }

}