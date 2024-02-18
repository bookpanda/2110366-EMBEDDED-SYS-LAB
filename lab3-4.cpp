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

    //Timer
    //if we want to output timer signal to external, select pin with timer e.g. B6 has TIM4_CH1
    //Timers > TIM4 > check internal clock, PWM Generation on channel 1, enable auto-reload
    //desired interval = ( (Prescaler + 1) * (counter period + 1) )/HCLK
    //e.g. we want timer every 0.5 seconds: 0.5 = ( (49999+1)*(1999+1) ) / 10*10^6
    //pulse=what duty cycle we want (50% duty cycle of 9V = 4.5V), usually ( (counter period+1)/2 ) + 1
    //for timers that output to external, add this:
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

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

    //Lab 3.2
}