#include<bits/stdc++.h>
using namespace std;

int main(void) {
    //Lab 4.1 Timer increase brightness
    //100ms timer: 0.1 = ( (1 + 1) * (399 + 1) )/8M
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

    //Lab4.2
    
}