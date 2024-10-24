#define Command_lengh 16
uint8_t command_word1[Command_lengh]={0x0F,0x72,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x72,0x00};
uint8_t command_start_freq1[Command_lengh]={0x0F,0x46,0x3D,0x16,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x99,0x00};
const uint16_t freqs[40] = {
  5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725,//A band  5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866,//B band
  5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945,//E band  5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880,//F band
  5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917,//r Band}
 
 if(i < 100) 
 { 
//  prog_freg(5500+1*i);       // диапазон сканировани€ 5500 - 6000 ћ√ц 
  prog_freg(5500+5*i);       // диапазон сканировани€ 5500 - 6000 ћ√ц 
  Packs[i].RX_Freq = (5500+5*i); 
  Packs[i].SendIdx = i; 
 
  i++; 
//  prog_freg(5823);       // диапазон сканировани€ 5500 - 6000 ћ√ц 
 } // scaning 
 else 
 { 
  for(j = 0; j < i; j++) 
  { 
   Packs[j].RSSI = adcData[j]; 
   SummRSSI += Packs[j].RSSI; 
 
   if(Packs[j].RSSI > maxRSSI) 
   { 
    maxRSSI = Packs[j].RSSI; 
    RX_Freq = Packs[j].RX_Freq; 
   } 
  } 
 
  AvgRSSI = SummRSSI/(i + 1); 
  SummRSSI = AvgRSSI; 
  
  if(maxRSSI > AvgRSSI + 100) 
  { 
	HAL_UART_Transmit(&huart2,command_word1,16,20); // на самом деле достаточно должно быть одной стартовой и не нужна така€ больша€ зарежка, но € перестаховывалс€ 
	HAL_Delay(500); 

	freq=RX_Freq;  // тут надо было искать ближайшую частоту из таблицы, но если ты перебираешь таблицу, то это лишнее. 
	command_start_freq[1]=0x46; 
	command_start_freq[2]=freq%256; 
	command_start_freq[3]=freq/256; 
	command_start_freq[14]=command_start_freq[1]+command_start_freq[2]+command_start_freq[3]; 
	HAL_UART_Transmit(&huart2,command_start_freq,16,20); 
	HAL_Delay(5000); // если делать подобие зеркальца то надо 1 минуту, но дл€ старта € 5 сек ставил. 

	AvgRSSI=0; // чтобы следующий поиск максимума не был испорчен завышенным средним изза старого сигнала. 
  } 
  maxRSSI = AvgRSSI; 
  i = 0; 
 
 } // end scaning 
 
 HAL_SPI_MspInit(&hspi1); 
// MX_SPI1_Init(); 
 HAL_Delay(1); // задержка 
 SPI1_SS_En; 
 CLK1_CNT = 0; 
 HAL_SPI_Transmit(&hspi1, SPI1_OUT, 4, 0x01); 
// HAL_SPI_Receive(&hspi1, SPI1_RSV, 4, 0X01); 
// HAL_SPI_TransmitReceive(&hspi1, SPI1_OUT, SPI1_RSV, 4, 0x01); 
 SPI1_SS_Dis; 
 
 HAL_ADC_Start_IT(&hadc1); 
 HAL_Delay(1); // задержка 
 
 adcData[i] = TmpADC; 
/* 
 mcuVoltage = ADC_MAX * ADC_REFERENCE_VOLTAGE / adcData[0]; 
    batteryVoltage = 2 * adcData[1] * mcuVoltage / ADC_MAX; 
*/ 
//    TmpADC = HAL_ADC_GetValue(&hadc1); 
/* 
 SPI2_SS_En; 
 HAL_SPI_Transmit(&hspi2, SPI2_OUT, 4, 0x01); 
 HAL_SPI_Receive(&hspi2, SPI2_RSV, 4, 0X01); 
 SPI2_SS_Dis; 
*/ 
// HAL_Delay(1); // задержка 
// HAL_UART_Transmit(&huart1, UART1_OUT, 2, 0x01); 
// HAL_Delay(1); // задержка 
// HAL_UART_Transmit(&huart2, UART2_OUT, 2, 0x01); 
 
    /* USER CODE END WHILE */ 
 
    /* USER CODE BEGIN 3 */ 
 HAL_Delay(25); // задержка 
 
}