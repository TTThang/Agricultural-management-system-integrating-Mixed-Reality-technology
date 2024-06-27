/*
 * Modbus RTU request data sample
 * Reference: https://www.modbustools.com/modbus.html#function05
 */

/*
 * Modbus RTU register table
 * ------------------------------------------------------------------
 * |Content          | Register Address | Function Code |  Default  |
 * ------------------------------------------------------------------
 * |Data temperature |      0x0000      |      0x04     |    N/A    |
 * ------------------------------------------------------------------
 * |Data humi        |      0x0001      |      0x04     |    N/A    |
 * ------------------------------------------------------------------
 * |Device address   |      0x0100      |   0x03/0x06   |    0x11   |
 * ------------------------------------------------------------------
 * |Device baudrate  |      0x0101      |   0x03/0x06   |   3:9600  |
 * ------------------------------------------------------------------
 */


#include "debug.h"
#include "DS18B20_HAL/DS18B20_HAL.h"
#include "RS485_SLAVE_HAL/RS485_SLAVE_HAL.h"

RS485_Config RS485_Conf;
uint8_t Device_Address = 0x11;
volatile bool Rxfinish_FLag = 0;
volatile u8 Rx_buffer[252];
volatile u8 Rx_size = 0;

onewire_addr_t addrs[1];
size_t sensor_count = 0;
float temps[1];
uint16_t temp;
uint16_t humi;


/*********************************************************************
 * @fn      ADC_Function_Init
 *
 * @brief   Initializes ADC collection.
 *
 * @return  none
 */
void ADC_Function_Init(void)
{
    ADC_InitTypeDef  ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_T1_CC3;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_InjectedSequencerLengthConfig(ADC1, 1);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_241Cycles);

    ADC_DiscModeChannelCountConfig(ADC1, 1);
    ADC_Calibration_Vol(ADC1, ADC_CALVOL_50PERCENT);
    ADC_InjectedDiscModeCmd(ADC1, ENABLE);
    ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

/*********************************************************************
 * @fn      TIM1_PWM_In
 *
 * @brief   Initializes TIM1 PWM output.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void TIM1_PWM_In(u16 arr, u16 psc, u16 ccp)
{
    GPIO_InitTypeDef        GPIO_InitStructure = {0};
    TIM_OCInitTypeDef       TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_TIM1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
    TIM_Cmd(TIM1, ENABLE);
}

/*********************************************************************
 * @fn      Average_multi_sample
 *
 * @brief   Get multi sample and calculate averag
 *
 * @param   num_sample - the number of samples to be taken
 *
 * @return  none
 */

/*********************************************************************
 * @fn      Shutdown_Device
 *
 * @brief   Shutdown device
 *
 * @return  none
 */

void Shutdown_Device(void){
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = RS485_Conf.RE_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RS485_Conf.RE_Port, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = RS485_Conf.DE_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RS485_Conf.DE_Port, &GPIO_InitStructure);

    GPIO_WriteBit(RS485_Conf.RE_Port, RS485_Conf.RE_Pin, 1);
    GPIO_WriteBit(RS485_Conf.DE_Port, RS485_Conf.DE_Pin, 0);

    RCC_LSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
//    PWR_AWU_SetPrescaler(PWR_AWU_Prescaler_10240);
//    PWR_AWU_SetWindowValue(25);
    PWR_AutoWakeUpCmd(DISABLE);
    PWR_EnterSTANDBYMode(PWR_STANDBYEntry_WFI);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */

int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
//    USART_Printf_Init(115200);
    RS485_Conf.UART_Map = Remap2;
    RS485_Conf.UART_Baudrate = 9600;
    RS485_Conf.RE_Port = GPIOD;
    RS485_Conf.RE_Pin = GPIO_Pin_3;
    RS485_Conf.DE_Port = GPIOD;
    RS485_Conf.DE_Pin = GPIO_Pin_4;
    RS485_Conf.Rxfinish_FLag = &Rxfinish_FLag;
    RS485_Conf.Rx_buffer = Rx_buffer;
    RS485_Conf.Rx_size = &Rx_size;

    RS485_Init(&RS485_Conf);
    ADC_Function_Init();
    TIM1_PWM_In(1000, 48000 - 1, 500);
    ds18x20_init(GPIOC, GPIO_Pin_5);
    ds18x20_scan_devices(addrs, 1, &sensor_count);

//    RS485_Send_Mode();
//    printf("SystemClk:%d\r\n",SystemCoreClock);
//    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    RS485_Receive_Mode();

    while(1){
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
        ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC));
        ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);

        ds18b20_measure_and_read(addrs[0], temps);
        temp = (uint16_t)(temps[0]*100);
        humi = (uint16_t)((float)(1023 - ADC1->IDATAR1)*100/1023*100);
//        char ch = 'C';
//        RS485_Send_Mode();
//        printf("Temperature: %02d.%02d\xC2\xB0%c\r\n", (uint8_t)(temp/100), (uint8_t)(temp - ((uint8_t)(temp/100))*100), ch);
//        printf("Humidity: %02d.%02d%%\r\n", (uint8_t)(humi/100), (uint8_t)(humi - ((uint8_t)humi/100))*100);
//        RS485_Receive_Mode();
        if(Rxfinish_FLag){
            RS485_Send_Mode();
            if(Rx_buffer[0] == 0 || Rx_buffer[0] == Device_Address){
                if(Rx_buffer[Rx_size-1] == '\n' || Rx_buffer[Rx_size-1] == '\0'){
                    Rx_size--;
                }
                uint8_t request[Rx_size];
                memcpy(request, Rx_buffer, Rx_size);
                bool crc_result = CRC_Check(request, Rx_size-2);
                if(crc_result){
                    switch(Rx_buffer[1]){

                    case rtu_Read_Input_Registers:{
                        uint8_t data[Rx_buffer[5]*2+1];
                        uint16_t data_index = 0;
                        data[data_index] = Rx_buffer[5]*2;
                        uint16_t quantity = ((uint16_t)(Rx_buffer[4]<<8))|Rx_buffer[5];
                        uint16_t start_register = ((uint16_t)(Rx_buffer[2]<<8))|Rx_buffer[3];
                        for(uint16_t res_index = 0; res_index < quantity; res_index++){
                            switch(start_register+res_index){
                            case 0x0000:{
                                data[++data_index] = (uint8_t)(temp>>8);
                                data[++data_index] = (uint8_t)temp;
                            }
                            break;
                            case 0x0001:{
                                data[++data_index] = (uint8_t)(humi>>8);
                                data[++data_index] = (uint8_t)humi;
                            }
                            break;
                            default:
                                ;
                            }
                        }
                        RS485_Slave_Respone(Device_Address, rtu_Read_Input_Registers, data, Rx_buffer[5]*2+1);
                    }
                    break;

                    case rtu_Read_Holding_Registers:{
                        switch(Rx_buffer[2]){

                        case 0x01:{
                            switch(Rx_buffer[3]){

                            case 0x00:{
                                RS485_Slave_Respone(Device_Address, rtu_Read_Holding_Registers, &Device_Address, 1);
                            }
                            break;

                            case 0x01:{
                                uint32_t current_baudrate = RS485_Conf.UART_Baudrate;
                                uint8_t i = 0;
                                while(current_baudrate != 1200){
                                    current_baudrate = (current_baudrate >> 1);
                                    i++;
                                }
                                RS485_Slave_Respone(Device_Address, rtu_Read_Holding_Registers, &i, 1);
                            }
                            break;

                            default:
                                ;
                            }
                        }
                        break;
                        }
                    }
                    break;

                    case rtu_Write_Single_Register:{
                        switch(Rx_buffer[2]){

                        case 0x01:{
                            switch(Rx_buffer[3]){

                            case 0x00:{
                                Device_Address = Rx_buffer[5];
                                uint8_t tmp = Rx_buffer[5];
                                RS485_Slave_Respone(Rx_buffer[5], rtu_Write_Single_Register, &tmp, 1);      //Same as master request
                            }
                            break;

                            case 0x01:{
                                uint32_t temp_baudrate = 1200;
                                uint8_t i = Rx_buffer[5];
                                uint8_t tmp = Rx_buffer[5];
                                while(i!=0){
                                    temp_baudrate = (temp_baudrate << 1);
                                    i--;
                                }
                                RS485_Conf.UART_Baudrate = temp_baudrate;
                                RS485_Change_Baudrate(&RS485_Conf);
                                Delay_Ms(1000);
                                RS485_Slave_Respone(Device_Address, rtu_Write_Single_Register, &tmp, 1);    //Same as master request
                            }
                            break;

                            case 0xff:{
                                Shutdown_Device();
                            }
                            break;

                            default:
                                ;
                            }
                        }
                        break;

                        default:
                            ;
                        }
                    }
                    break;

                    default:
                        ;
                    }
                }
            }
            Rxfinish_FLag = 0;
            Rx_size = 0;
            RS485_Receive_Mode();
        }
    }
}
