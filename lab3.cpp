#include<bits/stdc++.h>
using namespace std;

int main(void) {
    //Lab 3.1 Interrupt
    //ioc > System Core > GPIO > change the Pin with External Interrupt to rising edge (this sets the button C13 to generate interrupt)
    //GPIO > NVIC > check EXTI line interrupt

    //we have to increase HAL_Delay() priority (HAL_Delay is an interrupt of SysTick, we need it to have higher priority than other interrupts)
    //System Core > NVIC > change Priority Group to 2-bits > change EXTI line Preemption Priority to 2

    //When we press the button, it will call this in main.c (A6 is set to GPIO_OUTPUT to external LED)
    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
        HAL_Delay(100);
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
    }
    //actual lab3.1
    int delays[] = {200, 1000, 5000};
    int idx = 0;
    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	    idx = (idx+1) % 3;
    }
    while (1)
    {
	    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	    HAL_Delay(delays[idx]);
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }

    //debouncing state by button push
    int state = 0;
    int lastTick = -1000;
    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
        int tick = HAL_GetTick();
        if(tick - lastTick < 100) {
            return;
        }
        lastTick = tick;
        state = (state + 1) % 3;
        HAL_UART_Transmit(&huart2, msg, strlen(msg), 100);
        if(state==0) HAL_UART_Transmit(&huart2, "0", 1, 100);
        if(state==1) HAL_UART_Transmit(&huart2, "1", 1, 100);
        if(state==2) HAL_UART_Transmit(&huart2, "2", 1, 100);
    }

    //Timer
    //if we want to output timer signal to external, select pin with timer e.g. B6 has TIM4_CH1
    //Timers > TIM4 > check internal clock, PWM Generation on channel 1, enable auto-reload
    //desired interval = ( (Prescaler + 1) * (counter period + 1) )/HCLK
    //e.g. we want timer every 0.5 seconds: 0.5 = ( (49999+1)*(999+1) ) / 100*10^6
    //pulse=what duty cycle we want (50% duty cycle of 9V = 4.5V), usually ( (counter period+1)/2 ) + 1
    //check global interrupt for that timer in NVIC (PWM can emit without interrupt)
    //add this to start timer
    HAL_TIM_Base_Start_IT(&htim4);
    //for timers that output to external, MUST ADD PULSE and this:
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

    //Lab 3.3 Timer
    //HCLK 100M
    //500ms timer: 0.5 = ( (49999 + 1) * (999 + 1) )/HCLK
    //490ms timer: 0.490 = ( (49999 + 1) * (980 + 1) )/HCLK
    //1s timer: 1 = ( (49999 + 1) * (1999 + 1) )/HCLK
    /* USER CODE BEGIN 0 */
    int internalLEDCount = 0;
    int externalLEDCount = 0;
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) { //handles global interrupt from timers
        if(htim == &htim3) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            internalLEDCount += 1;
        } else if(htim == &htim4) {
            externalLEDCount += 1;
        } else if(htim == &htim5) {
            int currentTime = HAL_GetTick();
            char *message = (char*) malloc(sizeof(char) * 100);
            sprintf(message, "Time: %d, internal= %d, external= %d\r\n", currentTime, internalLEDCount, externalLEDCount);
            HAL_UART_Transmit(&huart2, message, strlen(message), 100);
            free(message);
        }
    }
    /* USER CODE BEGIN 2 */
    HAL_TIM_Base_Start_IT(&htim3);
    HAL_TIM_Base_Start_IT(&htim4);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_Base_Start_IT(&htim5);

    //Lab 3.4 UART Interrupt: check UART global interrupt in NVIC
    /* USER CODE BEGIN 0 */
    char buffer[10];

    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) //callbacks are declared outside main()
    {
        buffer[1] = '\0';
        HAL_UART_Transmit(&huart2, buffer, 1, 100);

        if (buffer[0] == '\r') {
            HAL_UART_Transmit(&huart2, "\n", 1, 100);
        }
    }
    while (1)
    {
        HAL_UART_Receive_IT(&huart2, buffer, 1);//it has _IT (non-blocking)
    }

}