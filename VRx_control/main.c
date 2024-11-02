#include "stm32f401xc.h"

#define SYNTHESIZER_REG_A                           0x00
#define SYNTHESIZER_REG_B                           0x01
#define SYNTHESIZER_REG_C                           0x02
#define SYNTHESIZER_REG_D                           0x03
#define VCO_SWITCH_CAP_CONTROL_REGISTER             0x04
#define DFC_CONTROL_REGISTER                        0x05
#define SIXM_AUDIO_DEMODULATOR_CONTROL_REGISTER     0x06
#define SIXM5_AUDIO_DEMODULATOR_CONTROL_REGISTER    0x07
#define RECEIVER_CONTROL_REGISTER_1                 0x08
#define RECEIVER_CONTROL_REGISTER_2                 0x09
#define POWER_DOWN_CONTROL_REGISTER                 0x0A
#define STATE_REGISTER                              0x0F

#define RX5808_READ_CTRL_BIT                        0x00
#define RX5808_WRITE_CTRL_BIT                       0x01
#define RX5808_ADDRESS_R_W_LENGTH                   5
#define RX5808_DATA_LENGTH                          20
#define RX5808_PACKET_LENGTH                        25

#define periodMicroSec  1450


//const uint16_t frequencyTable[48] = {
//    5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // A
//    5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // B
//    5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // E
//    5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // F
//    5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917, // R
//    5333, 5373, 5413, 5453, 5493, 5533, 5573, 5613  // L
//};
const uint16_t frequencyTable[40] = {
  5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725,//A band
  5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866,//B band
  5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945,//E band
  5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880,//F band
  5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917,//r Band
};
uint16_t frequencyTable_RSSI[40];
uint16_t adcData[2];

int currentIndex;

void Init(void);
void SetMode(int modemix);
void SendIndexCmd(uint8_t index);
void delay (unsigned int cnt);
void rtc6705WriteRegister(uint32_t buf);
uint32_t rtc6705readRegister(uint8_t readRegister);

void Init(void)
{
	RCC->AHB1ENR |= 0x4;
	GPIOC->MODER |= 0x4000000;
	GPIOC->OSPEEDR |= 0x8000000;
	GPIOC->PUPDR |= 0x8000000;
	GPIOC->BSRR = 0x20000000;
	GPIOC->BSRR = 0x0002000;		
	
	RCC->APB1ENR |= 0x1;					//T2
	TIM2->CR1 = 0x10;							//down
	TIM2->PSC = 0;								//prescaler

//    ModuleBase::Init();
	RCC->AHB1ENR |= 0x1;	//PA
	
//    pinMode(PIN_MOSI, OUTPUT);
//    pinMode(PIN_CLK, OUTPUT);
//    pinMode(PIN_CS, OUTPUT);
	GPIOA->MODER |= 0x15;	//A0,1,2
	GPIOA->OSPEEDR |= 0x2A;
	
//    digitalWrite(PIN_CLK, LOW);
//    digitalWrite(PIN_MOSI, HIGH);
//    digitalWrite(PIN_CS, HIGH);
//	GPIOA->PUPDR = 0x16;	//0CLK, 1MOSI, 2CS
	GPIOA->BSRR = 0x10006;	//0CLK, 1MOSI, 2CS

	delay(100);
	SetMode(1);
}

void delay (unsigned int cnt)
{
	TIM2->CNT = cnt;							//value
	TIM2->CR1 |= 0x1;							//start
	while((TIM2->SR&1) == 0);
	TIM2->CR1 &= ~0x1;						//stop
	TIM2->SR &= ~0x1;
}
	

void SendIndexCmd(uint8_t index)
{
//    DBG("Setting index ");
//    DBGLN("%x", index);

    uint16_t f = frequencyTable[index];
    uint32_t data = ((((f - 479) / 2) / 32) << 7) | (((f - 479) / 2) % 32);
    uint32_t newRegisterData = SYNTHESIZER_REG_B  | (RX5808_WRITE_CTRL_BIT << 4) | (data << 5);

    uint32_t currentRegisterData = SYNTHESIZER_REG_B | (RX5808_WRITE_CTRL_BIT << 4) | rtc6705readRegister(SYNTHESIZER_REG_B);

    if (newRegisterData != currentRegisterData)
    {
        rtc6705WriteRegister(SYNTHESIZER_REG_A  | (RX5808_WRITE_CTRL_BIT << 4) | (0x8 << 5));
        rtc6705WriteRegister(newRegisterData);
    }
    currentIndex = index;
}

void SetMode(int modemix)
{
    if (modemix == 1)
    {
//        digitalWrite(PIN_CLK, HIGH);
			GPIOA->BSRR = 0x1;	//0CLK, 1MOSI, 2CS
      delay(100);
//        digitalWrite(PIN_CLK, LOW);
			GPIOA->BSRR = 0x10000;	//0CLK, 1MOSI, 2CS
       delay(500);
    }
    uint16_t f = frequencyTable[currentIndex];
    uint32_t data = ((((f - 479) / 2) / 32) << 7) | (((f - 479) / 2) % 32);
    uint32_t registerData = SYNTHESIZER_REG_B  | (RX5808_WRITE_CTRL_BIT << 4) | (data << 5);

    rtc6705WriteRegister(SYNTHESIZER_REG_A  | (RX5808_WRITE_CTRL_BIT << 4) | (0x8 << 5));
    delay(500);
    rtc6705WriteRegister(SYNTHESIZER_REG_A  | (RX5808_WRITE_CTRL_BIT << 4) | (0x8 << 5));
    rtc6705WriteRegister(registerData);
}

void rtc6705WriteRegister(uint32_t buf)
{
	uint8_t i;

//    digitalWrite(PIN_CS, LOW);
	GPIOA->BSRR = 0x40000;	//0CLK, 1MOSI, 2CS
	
  delay(periodMicroSec);

    for (i = 0; i < RX5808_PACKET_LENGTH; ++i)
    {
//        digitalWrite(PIN_MOSI, buf & 0x01);
			if(buf&1) GPIOA->BSRR = 0x2;	//0CLK, 1MOSI, 2CS
			else GPIOA->BSRR = 0x20000;	//0CLK, 1MOSI, 2CS
			
      delay(periodMicroSec / 4);
			
//      digitalWrite(PIN_CLK, HIGH);
			GPIOA->BSRR = 0x1;	//0CLK, 1MOSI, 2CS
			
      delay(periodMicroSec / 4);
			
//      digitalWrite(PIN_CLK, LOW);
			GPIOA->BSRR = 0x10000;	//0CLK, 1MOSI, 2CS
			
      delay(periodMicroSec / 4);

       buf >>= 1;
    }

	delay(periodMicroSec);
		
//    digitalWrite(PIN_MOSI, HIGH);
	GPIOA->BSRR = 0x2;	//0CLK, 1MOSI, 2CS
//    digitalWrite(PIN_CS, HIGH);
	GPIOA->BSRR = 0x4;	//0CLK, 1MOSI, 2CS
}

uint32_t rtc6705readRegister(uint8_t readRegister)
{
	uint8_t i;
	uint32_t buf = readRegister | (RX5808_READ_CTRL_BIT << 4);
	uint32_t registerData = 0;

//	digitalWrite(PIN_CS, LOW);
	GPIOA->BSRR = 0x40000;	//0CLK, 1MOSI, 2CS
	
	delay(periodMicroSec);

	// Write register address and read bit
	for (i = 0; i < RX5808_ADDRESS_R_W_LENGTH; ++i)
	{
//		digitalWrite(PIN_MOSI, buf & 0x01);
		if(buf&1) GPIOA->BSRR = 0x2;	//0CLK, 1MOSI, 2CS
		else GPIOA->BSRR = 0x20000;	//0CLK, 1MOSI, 2CS
		
		delay(periodMicroSec / 4);
		
//		digitalWrite(PIN_CLK, HIGH);
		GPIOA->BSRR = 0x1;	//0CLK, 1MOSI, 2CS

    delay(periodMicroSec / 4);
		
//    digitalWrite(PIN_CLK, LOW);
		GPIOA->BSRR = 0x10000;	//0CLK, 1MOSI, 2CS
		
		delay(periodMicroSec / 4);

		buf >>= 1;
	}

	// Change pin from output to input
//	pinMode(PIN_MOSI, INPUT);
	GPIOA->MODER &= ~0xC;

	// Read data 20 bits
	for (i = 0; i < RX5808_DATA_LENGTH; i++)
	{
//		digitalWrite(PIN_CLK, HIGH);
		GPIOA->BSRR = 0x1;	//0CLK, 1MOSI, 2CS
		
		delay(periodMicroSec / 4);

//		if (digitalRead(PIN_MOSI))
		if (GPIOA->ODR & 0x2)
		{
			registerData = registerData | (1 << (5 + i));
		}

		delay(periodMicroSec / 4);
		
//		digitalWrite(PIN_CLK, LOW);
		GPIOA->BSRR = 0x10000;	//0CLK, 1MOSI, 2CS
			
		delay(periodMicroSec / 2);
	}

	// Change pin back to output
//	pinMode(PIN_MOSI, OUTPUT);
	GPIOA->MODER |= 0x4;

//	digitalWrite(PIN_MOSI, LOW);
	GPIOA->BSRR = 0x20000;	//0CLK, 1MOSI, 2CS
	
//	digitalWrite(PIN_CS, HIGH);
	GPIOA->BSRR = 0x4;	//0CLK, 1MOSI, 2CS		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	return registerData;
}
 void USART1_init(void)
{
	RCC->APB2ENR |= 0x10;					//usart1 clock enable

	GPIOA->MODER |= 0x280000;			//portA9,10 - alternate function
	GPIOA->AFR[1] |= 0x770;   		//portA alternaive function	AF7 - USART1: rx,tx			

	USART1->BRR = 0x61A8;					//4800
	USART1->CR1 |= 0x202C;				//usart1 enable, 1 Start bit, 8 Data bits, no parity control,OVER8=0, transmitter/receiver enable, interrupt enable
	USART1->CR2 = 0x2000;					// 2 stop bits

	NVIC_EnableIRQ(USART1_IRQn);
}

int main (void)
{
	uint16_t temp,data,freq;
	Init();
	USART1_init();
	MX_ADC1_Init();
	HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcData, 2);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcData, 2);
	freq=0;
	while(1)
	{
		if(!(USART1->SR&0x20))
		{
			data=USART1->DR;		// freq
			SendIndexCmd(data);	// set freq
			delay(10000000);
		}
		else									// scan
			{
				SendIndexCmd(freq++);			// set freq
				delay(10000);
				frequencyTable_RSSI[freq]=adcData[0]; // write rssi
				
				if(freq==40)
				{
					
					for(int i=0;i<freq;i++)
					{
						USART1->DR=frequencyTable[i];
						while(!(USART1->SR&0x80));
						USART1->DR=frequencyTable_RSSI[i];
						while(!(USART1->SR&0x80));
					}
					freq=0;
				}
			}
	}
}










