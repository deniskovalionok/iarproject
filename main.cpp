/*
testing board with LEDs and TFT
*/
#include "main.h"
#include "lcd.h"
#include "math.h"
#include "graphic.h"


extern const uint16_t JOY_PIN[JOYn];
extern GPIO_TypeDef* JOY_PORT[JOYn] ;

__IO uint8_t bUpdateLCD=0;

static void EXTI9_5_IRQHandler_Config(void);

/** @addtogroup SPI_FullDuplex_ComDMA
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Uncomment this line to use the board as master, if not it is used as slave */
#define MASTER_BOARD
/* Private variables ---------------------------------------------------------*/
/* SPI handler declaration */
SPI_HandleTypeDef SpiHandle;
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
//static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
#ifdef MASTER_BOARD
  GPIO_InitTypeDef  GPIO_InitStruct;
#endif
  /* STM32L4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();




  /* Configure the system clock to 4 MHz */
  SystemClock_Config();
  
    /* -1- Enable GPIO Clock (to be able to program the configuration registers) */
  LED4_GPIO_CLK_ENABLE();
  LED5_GPIO_CLK_ENABLE();
  
  
    /* Enable GPIO clock */
  	EN_GPIO_CLK_ENABLE();
  	CNTRL_GPIO_CLK_ENABLE();
	RESET_GPIO_CLK_ENABLE();
	RS_GPIO_CLK_ENABLE();

  /* -2- Configure IO in output push-pull mode to drive external LEDs */
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct.Pin = LED4_PIN;
  HAL_GPIO_Init(LED4_GPIO_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = LED5_PIN;
  HAL_GPIO_Init(LED5_GPIO_PORT, &GPIO_InitStruct);
  
  
  /* -2- Configure IO in output push-pull mode to drive external LEDs */
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

		
  GPIO_InitStruct.Pin = EN_PIN;
  HAL_GPIO_Init(EN_GPIO_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = CNTRL_PIN;
  HAL_GPIO_Init(CNTRL_GPIO_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RS_PIN;
  HAL_GPIO_Init(RS_GPIO_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RESET_PIN;
  HAL_GPIO_Init(RESET_GPIO_PORT, &GPIO_InitStruct);


 /*##-1- Configure the SPI peripheral #######################################*/
  /* Set the SPI parameters */
  SpiHandle.Instance               = SPIx;
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
  SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  SpiHandle.Init.CRCPolynomial     = 7;
  SpiHandle.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
  SpiHandle.Init.NSS               = SPI_NSS_SOFT;
  SpiHandle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;

#ifdef MASTER_BOARD
  SpiHandle.Init.Mode = SPI_MODE_MASTER;
#else
  SpiHandle.Init.Mode = SPI_MODE_SLAVE;
#endif /* MASTER_BOARD */

  if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  
//  BSP_JOY_Init(JOY_MODE_EXTI);
    JOYx_GPIO_CLK_ENABLE(JOY_DOWN);
    
    GPIO_InitStruct.Pin = JOY_PIN[JOY_DOWN];
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    
    /* Configure Joy pin as input */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(JOY_PORT[JOY_DOWN], &GPIO_InitStruct);
	
	EXTI9_5_IRQHandler_Config();

	lcd_height = LCD_BHEIGHT;
	turnOnLCD();
  

  
  
//  int i=0;
  int x,y;
  int dx, dy;
  x=y=0;
  dx=1;dy=1;
  char text[20];
  float dr = 0;
  while(1)
  {
	HAL_GPIO_TogglePin(LED5_GPIO_PORT, LED5_PIN);
	//HAL_Delay(100);
	lcdbuf_clear();

	if(bUpdateLCD)
	{
		bUpdateLCD=0;
		if(lcd_height==LCD_BHEIGHT)
			lcd_height=LCD_SHEIGHT;
		else
			lcd_height=LCD_BHEIGHT;
		turnOnLCD();
	}
	
	Graphic_setTextWin(0,0,LCD_WIDTH,lcd_height);	//set text window
	Graphic_setTextXY(0,0);	//set start coords in window
	Graphic_setTextWrap(1);
	Graphic_setTextSteps(1,1);

	Graphic_setTextJustify(LEFT);
	Graphic_setCurrentFont(fnt8x8);
	Graphic_outputText("07.09.2016 15:52");
	Graphic_setTextJustify(RIGHT);
	Graphic_outputText("IIII");

	Graphic_setCurrentFont(fnt10x24);
	Graphic_setTextSteps(2,1);
	
	if(lcd_height==LCD_BHEIGHT)
		Graphic_setTextXY(0, 16);	//set start coords in window
	else
		Graphic_setTextXY(0, 8);	//set start coords in window
	dr+=0.001;
	int iret=sprintf(text, "%.3f", dr);
	Graphic_setTextJustify(NONE);
	Graphic_outputText(text);
	Graphic_setTextSteps(1,1);
	Graphic_setTextXY(72, graphic.text.gstrY);	//set start coords in window
	Graphic_outputText("µSv/h");


	
	lcd_show();
	x+=dx;
	if(x>=LCD_WIDTH || x<0)
	{
		dx=-dx;
		x+=dx;

		y+=dy;
		if(y>=lcd_height || y<=0)
		{
			dy=-dy;
			y+=dy;
		}	
	}
  }
}

void checkTransferState()
{
  switch(wTransferState)
  {
    case TRANSFER_COMPLETE :
		;
    break;
    default :
      Error_Handler();
    break;
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            PLL_Q                          = 4
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */


//void SystemClock_Config(void)
//{
//  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//
//  /* MSI is enabled after System reset, activate PLL with MSI as source */
//  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
//  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
//  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
//  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
//  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
//  RCC_OscInitStruct.PLL.PLLM = 1;
//  RCC_OscInitStruct.PLL.PLLN = 40;
//  RCC_OscInitStruct.PLL.PLLR = 2;
//  RCC_OscInitStruct.PLL.PLLP = 7;
//  RCC_OscInitStruct.PLL.PLLQ = 4;
//  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//  {
//    /* Initialization Error */
//    while(1);
//  }
//
//  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
//     clocks dividers */
//  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
//  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
//  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
//  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
//  {
//    /* Initialization Error */
//    while(1);
//  }
//}
//
//









/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = MSI
  *            SYSCLK(Hz)                     = 4000000
  *            HCLK(Hz)                       = 4000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            Flash Latency(WS)              = 0
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* The following clock configuration sets the Clock configuration sets after System reset                */
  /* It could be avoided but it is kept to illustrate the use of HAL_RCC_OscConfig and HAL_RCC_ClockConfig */
  /* and to be eventually adapted to new clock configuration                                               */

  /* MSI is enabled after System reset at 4Mhz, PLL not used */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Select MSI as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  /* Set 0 Wait State flash latency for 4Mhz */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  if(HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Disable Power Control clock */
  __HAL_RCC_PWR_CLK_DISABLE();
}







/**
  * @brief  TxRx Transfer completed callback.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report end of DMA TxRx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* Turn LED3 on: Transfer in transmission/reception process is complete */
//  BSP_LED_On(LED3);
  wTransferState = TRANSFER_COMPLETE;
}

/**
  * @brief  SPI error callbacks.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  wTransferState = TRANSFER_ERROR;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
//  BSP_LED_Off(LED3);
    HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_PIN, GPIO_PIN_SET);
  while(1)
  {
    /* Toogle LED3 for error */
 //   BSP_LED_Toggle(LED3);
    HAL_Delay(1000);
  }
}


															   
/**
  * @brief Provide accurate delay (in milliseconds) based on variable incremented.
  * @note In the default implementation , SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals where uwTick
  *       is incremented.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */
void HAL_Delay(uint32_t Delay)
{
  uint32_t tickstart = 0;
  tickstart = HAL_GetTick();
  while((HAL_GetTick() - tickstart) < Delay)
  {
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE);
  }
}




#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif






/**
  * @brief  Configures EXTI line 9-5 (connected to PA.5 pin) in interrupt mode
  * @param  None
  * @retval None
  */
static void EXTI9_5_IRQHandler_Config(void)
{
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Enable GPIOA clock */
 // __HAL_RCC_GPIOA_CLK_ENABLE();
	DOWN_JOY_GPIO_CLK_ENABLE();
	
  /* Configure PA.5 pin as input floating */
  GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  GPIO_InitStructure.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Enable and set EXTI line 0 Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_5)
  {
    bUpdateLCD = 1;
  }
}