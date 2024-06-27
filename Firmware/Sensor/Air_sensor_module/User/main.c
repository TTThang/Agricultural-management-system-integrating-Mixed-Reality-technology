 /*
 * Modbus RTU request data sample
 * Reference: https://www.modbustools.com/modbus.html#function05
 */


/*
 * Modbus RTU register table
 * ------------------------------------------------------------------
 * |Content           | Register Address | Function Code |  Default  |
 * ------------------------------------------------------------------
 * |Temperature data  |      0x0000      |      0x04     |    N/A    |
 * ------------------------------------------------------------------
 * |Humidity data     |      0x0001      |      0x04     |    N/A    |
 * ------------------------------------------------------------------
 * |Light sensor data |      0x0002      |      0x04     |    N/A    |
 * ------------------------------------------------------------------
 * |CO2 data          |      0x0003      |      0x04     |    N/A    |
 * ------------------------------------------------------------------
 * |Device address    |      0x0100      |   0x03/0x06   |    0x11   |
 * ------------------------------------------------------------------
 * |Device baudrate   |      0x0101      |   0x03/0x06   |   3:9600  |
 * ------------------------------------------------------------------
 */

#include "debug.h"
#include "BH1750_HAL/BH1750_HAL.h"
#include "SCD4X_HAL/SCD4X_HAL.h"
#include "RS485_SLAVE_HAL/RS485_SLAVE_HAL.h"
#include "string.h"
/* Global define */


/* Global Variable */
RS485_Config RS485_Conf;
uint8_t Device_Address = 0x12;
volatile bool Rxfinish_FLag = 0;
volatile u8 Rx_buffer[252];
volatile u8 Rx_size = 0;

uint16_t co2, temp, humi;
float temperature, humidity;
uint16_t lux;

/*********************************************************************
 * @fn      Shutdown_Device
 *
 * @brief   Shutdown device
 *
 * @return  none
 */

void Shutdown_Device(void){
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    scd4x_stop_periodic_measurement();
    bh1750_power_down();

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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();

    RS485_Conf.UART_Map = NoRemap;
    RS485_Conf.UART_Baudrate = 9600;
    RS485_Conf.RE_Port = GPIOD;
    RS485_Conf.RE_Pin = GPIO_Pin_3;
    RS485_Conf.DE_Port = GPIOD;
    RS485_Conf.DE_Pin = GPIO_Pin_4;
    RS485_Conf.Rxfinish_FLag = &Rxfinish_FLag;
    RS485_Conf.Rx_buffer = Rx_buffer;
    RS485_Conf.Rx_size = &Rx_size;

    RS485_Init(&RS485_Conf);

//    RS485_Send_Mode();
//    printf("SystemClk:%d\r\n",SystemCoreClock);
//    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
//    RS485_Receive_Mode();

    I2C_HAL_Init(100000,0x02);

    bh1750_init(0);
    bh1750_setup(BH1750_MODE_CONTINUOUS, BH1750_RES_HIGH);
    bh1750_set_measurement_time(69);

    scd4x_init();
    scd4x_wake_up();
    scd4x_stop_periodic_measurement();
    scd4x_reinit();
    scd4x_start_periodic_measurement();
    RS485_Receive_Mode();

    while(1){
        bool scd_ready = false;
        scd4x_get_data_ready_status(&scd_ready);
        bh1750_read(&lux);
        if(scd_ready){
            scd4x_read_measurement(&co2, &temperature, &humidity);
            temp = (uint16_t)(temperature * 100);
            humi = (uint16_t)(humidity * 100);
//            RS485_Send_Mode();
//            printf("CO2: %u ppm\r\n", co2);
//            char ch = 'C';
//            printf("Temperature: %02d.%02d\xC2\xB0%c\r\n", (uint8_t)(temp/100), (uint8_t)(temp - ((uint8_t)(temp/100))*100), ch);
//            printf("Humidity: %02d.%02d%%\r\n", (uint8_t)(humi/100), (uint8_t)(humi - ((uint8_t)humi/100))*100);
//            printf("Lux: %d lx\r\n", lux);
//            RS485_Receive_Mode();

        }

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
                            case 0x0002:{
                                data[++data_index] = (uint8_t)(lux>>8);
                                data[++data_index] = (uint8_t)lux;
                            }
                            case 0x0003:{
                                data[++data_index] = (uint8_t)(co2>>8);
                                data[++data_index] = (uint8_t)co2;
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
