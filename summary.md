- Set key binds: settings > preferences > keys > content assist
- How to look at port chart: outer=code name | inner=in-board name | e.g. PA5(code) corresponds to D13(inner)
- Do not connect both ends on same vertical line on breadboard, it won't work
- LED: bigger side = negative
- Say we want to use PA1, which corresponds to A1 on board, we have to set PA1 in ioc file to be GPIO_Output as well
# Lab 2.4 UART
- Connectivity > USART2 aka USB, see Baud Rate = 115200 bits/s
    ls /dev/tty.* #search for port
    screen //port// 115200 #connect to port with buad rate
    screen -ls #list all active serial port
    screen -X -S //identifier// quit #quit that serial port
# Lab 2.5 UART on/off command to LED
- PA6 is connected to green LED, set to GPIO_Output
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
    while (1) {
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
    }
# Lab 3.1 Interrupt
1. ioc > System Core > GPIO > change the Pin with External Interrupt to rising edge (this sets the button C13 to generate interrupt)
- GPIO > NVIC > check EXTI line interrupt
2. we have to increase HAL_Delay() priority (HAL_Delay is an interrupt of SysTick, we need it to have higher priority than other interrupts)
- System Core > NVIC > change Priority Group to 2-bits > change EXTI line Preemption Priority to 2
    int delays[] = {200, 1000, 5000};
    int idx = 0;
    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	    idx = (idx+1) % 3;
    }
    while (1) {
	    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	    HAL_Delay(delays[idx]);
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
# Lab 3.2 Timer
1. if we want to output timer signal to external, select pin with timer e.g. B6 has TIM4_CH1
- Timers > TIM4 > check internal clock, PWM Generation on channel 1, enable auto-reload
2. desired interval = ( (Prescaler + 1) * (counter period + 1) )/HCLK
- e.g. we want timer every 0.5 seconds: 0.5 = ( (49999+1) * (999+1) ) / 100*10^6
- pulse=what duty cycle we want (50% duty cycle of 9V = 4.5V), usually ( (counter period+1)/2 ) + 1
3. check global interrupt for that timer in NVIC (PWM can emit without interrupt)
    //add this to start timer
    HAL_TIM_Base_Start_IT(&htim4);
    //for timers that output to external, MUST ADD PULSE and this:
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
# Lab 3.3 Timer
- HCLK 100M
- 500ms timer: 0.5 = ( (49999 + 1) * (999 + 1) )/HCLK
- 490ms timer: 0.490 = ( (49999 + 1) * (980 + 1) )/HCLK
- 1s timer: 1 = ( (49999 + 1) * (1999 + 1) )/HCLK
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
    HAL_TIM_Base_Start_IT(&htim3);
    HAL_TIM_Base_Start_IT(&htim4);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_Base_Start_IT(&htim5);
# Lab 3.4 UART Interrupt: check UART global interrupt in NVIC
- Callbacks are declared outside main()
    char buffer[10];
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
        buffer[1] = '\0';
        HAL_UART_Transmit(&huart2, buffer, 1, 100);

        if (buffer[0] == '\r') {
            HAL_UART_Transmit(&huart2, "\n", 1, 100);
        }
    }
    while (1) {
        HAL_UART_Receive_IT(&huart2, buffer, 1); //it has _IT (non-blocking)
    }
# Lab 4.1 Timer increase brightness
- 100us timer: 0.0001 = ( (1 + 1) * (399 + 1) )/8M, PWM gen
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    while (1) {
        for(int i=0;i<100;i++) {
            TIM2->CCR1 = i;
            HAL_Delay(10);
        }
        for(int i=100;i>0;i--) {
            TIM2->CCR1 = i;
            HAL_Delay(10);
        }
    }
# Lab 4.2 Adjust light from LDR
- ioc > Analog > ADC1 > check IN0, PA0 = ADC1_IN0
- LDR input:3.3v, output:GND,PA0
    int ldrval = 0;
    char buf[256];
    while (1) {
        HAL_ADC_Start(&hadc1);
        if(HAL_ADC_PollForConversion(&hadc1, 1000) == HAL_OK)
            ldrval = HAL_ADC_GetValue(&hadc1);
        sprintf (buf, "%d\r\n" , ldrval);
        HAL_UART_Transmit(&huart2, buf, strlen(buf), 1000);
        HAL_Delay(100);
    }
# Lab 4.3 Control LED with LDR
- create ADC1_IN0 and timer
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    int adcval = 0;
    char buf[256];
    while (1) {
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
    }
# Lab 5.1 Two UARTs communicating
- UART2's TX sends to UART1's RX (wire PA2(TX) to resistor to PA10(RX))
- check UART1 global interrupt in NVIC
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
        if(huart == &huart1) {
            HAL_UART_Transmit(&huart2, "b", 1, 100);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        }
    }
    char c[1];
    while (1) {
        if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {
            HAL_UART_Transmit(&huart2, "a", 1, 100);
            HAL_Delay(50);
            HAL_UART_Receive_IT(&huart1, c, 1); //must be _IT (interrupt)
            while(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {}
        }
    }
# Lab 5.2 Two SPIs communicating
- SPI2 Transmit Only Master, SPI3 Receive Only Slave + global interrupt
- SPI2 sends to SPI3 (wire MOSI(PC3, PC12), CLK(PB12, PB10) to resistor to each other)
    void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
        if(hspi == &hspi3) {
            HAL_UART_Transmit(&huart2, "b", 1, 100);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        }
    }
    char c;
    while (1) {
        if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {
            HAL_SPI_Transmit(&hspi2, "a", 1, 100);
            HAL_Delay(50);
            HAL_SPI_Receive_IT(&hspi3, &c, 1); //must be _IT (interrupt)
            while(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {}
        }
    }
# Lab 5.3 Two I2Cs communicating
1. I2C1 event, error interrupt
- GPIO > I2C > both I2C1_SDA, I2C1_SCL GPIO Pull-up
2. I2C2 no need to customize, it is master
3. I2C2 sends to I2C1 (wire SDA(PB9, PB7), SCL(PB6, PB10) to resistor to each other)
    void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
        if(hi2c == &hi2c1) {
            HAL_UART_Transmit(&huart2, "b", 1, 100);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        }
    }
    char c;
    while (1) {
        if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {
            HAL_UART_Transmit(&huart2, "a", 1, 100);
            HAL_I2C_Master_Transmit(&hi2c2, 0, "a", 1, 100);
            HAL_Delay(50);
            HAL_I2C_Slave_Receive_IT(&hi2c1, &c, 1); //must be _IT (interrupt)
            while(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {}
        }
    }