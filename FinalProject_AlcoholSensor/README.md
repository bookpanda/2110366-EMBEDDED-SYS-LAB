# Final Project - Alcohol sensor

## Wiring
Connect the MQ-3 Alcohol sensor to the NUCLEO-F411RE as follows (left is MQ-3, right is NUCLEO-F411RE):
- VCC -> 5V
- GND -> GND
- A0 (analog) -> A0
- D0 (digital) no need for this one

## Setup
- ioc > Analog > ADC1 > check IN0 (PA0 aka A0 on the board now is ADC1_IN0)
    ```c
    void uartPrint(const char* msg) {
	    HAL_UART_Transmit(&huart2, msg, strlen(msg), 100);
    }

    void uartPrintln(const char* msg) {
        uartPrint(msg);
        uartPrint("\r\n");
    }

    char line[100];

    while (1)
    {
        HAL_ADC_Start(&hadc1);
        if(HAL_ADC_PollForConversion(&hadc1, 1000) == HAL_OK) {
            int alcoholLevel = HAL_ADC_GetValue(&hadc1);
            int tick = HAL_GetTick();
            sprintf(line, "Time: %d, Alcohol: %d", tick, alcoholLevel);
            uartPrintln(line);

        }
        HAL_Delay(1000);
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    ```

## Measurements
Around 2700-2800 is the threshold for detecting alcohol.
- Ambient air: 600-700
- Normal breath: 600-700
- Intoxicated breath: 
- Put next to open alcohol spray bottle: 1300-2500
- After spraying alcohol on the sensor: 2700-3000
