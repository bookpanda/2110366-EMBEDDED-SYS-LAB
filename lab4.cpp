#include<bits/stdc++.h>
using namespace std;

int main(void) {
    //Lab 4.1 Timer increase brightness
    //100us timer: 0.0001 = ( (1 + 1) * (399 + 1) )/8M, PWM gen
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    while (1)
    {
        for(int i=0;i<100;i++) {
            TIM2->CCR1 = i;
            HAL_Delay(10);
        }
        for(int i=100;i>0;i--) {
            TIM2->CCR1 = i;
            HAL_Delay(10);
        }
    }

    //Lab4.2 adjust light from LDR
    //ioc > Analog > ADC1 > check IN0, PA0 = ADC1_IN0
    //LDR input:3.3v, output:GND,PA0
    int ldrval = 0;
    char buf[256];
    while (1)
    {
        HAL_ADC_Start(&hadc1);
        if(HAL_ADC_PollForConversion(&hadc1, 1000) == HAL_OK)
            ldrval = HAL_ADC_GetValue(&hadc1);
        sprintf (buf, "%d\r\n" , ldrval);
        HAL_UART_Transmit(&huart2, buf, strlen(buf), 1000);
        HAL_Delay(100);
    }

    //Lab4.3 control LED with LDR
    //create ADC1_IN0 and timer
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    int adcval = 0;
    char buf[256];
    while (1)
    {
        HAL_ADC_Start(&hadc1);
        if (HAL_ADC_PollForConversion(&hadc1, 1000) == HAL_OK) {
            adcval = HAL_ADC_GetValue(&hadc1);
            sprintf (buf, "%d\r\n" , adcval);
            int c = 21 - adcval; //max - curr
            int p = 21 - 1; //max - min
            int brightness = (1 - (c/p)) * 100;
            TIM4->CCR1 = brightness;
            HAL_UART_Transmit(&huart2, buf, strlen(buf), 1000);
        }
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
}