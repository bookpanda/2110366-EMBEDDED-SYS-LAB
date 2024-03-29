#include<bits/stdc++.h>
using namespace std;

int main(void) {
    //Lab 5.1 2 UARTs communicating
    //UART2's TX sends to UART1's RX (wire PA2(TX) to resistor to PA10(RX))
    //check UART1 global interrupt in NVIC
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
    {
        if(huart == &huart1) {
            HAL_UART_Transmit(&huart2, "b", 1, 100);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        }
    }
    char c[1];
    while (1)
    {
        if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {
            HAL_UART_Transmit(&huart2, "a", 1, 100);
            HAL_Delay(50);
            HAL_UART_Receive_IT(&huart1, c, 1); //must be _IT
            while(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {}
        }
    }

    
}