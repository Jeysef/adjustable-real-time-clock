/**
  *****************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  *****************************************************************************
  * @attention
  *
  *  Benchmark code based on:
  *    OLED SSD1306 library for STM32
  *    https://github.com/lamik/OLED_SSD1306_STM32_HAL
  *    Mateusz Salamon <mateusz@msalamon.pl>
  *
  *****************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

#include "delay.h"
#include "hw_setup.h"

/* Private includes ----------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oled_ssd1306.h"
#include "gfx_bw.h"
#include "fonts.h"
#include "picture.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/* Main function -------------------------------------------------------------*/
/**
  * @brief  The application entry point
  * @retval int - don't care
  */
int main(void)
{
  /* STM8 configuration */
  MCU_Config();
  init_ms();

  /* SSD1306 configuration */
#ifdef SSD1306_I2C_CONTROL
  SSD1306_I2cInit();
#endif
#ifdef SSD1306_SPI_CONTROL
  SSD1306_SpiInit();
#endif

  /* SSD1306 demo code */

  // SSD1306_Bitmap((uint8_t*)picture);
  // delay_ms(2000);

  // GFX_SetFont(font_6x4);
  // GFX_SetFontSize(1);

  /* SSD1306_StartScrollLeftUp(0, 7, SCROLL_EVERY_4_FRAMES, 1);
   * SSD1306_StartScrollLeft(0, 7, SCROLL_EVERY_4_FRAMES);
   * SSD1306_StartBlinking(1);
   * SSD1306_ZoomIn(1);
   */

  /* Infinite loop */
  for(;;) {

    SSD1306_Clear(WHITE);
    // GFX_DrawString(0, 0, "HELLO", WHITE, BLACK);

    GPIO_WriteHigh(TEST_Port, TEST_Pin);
    SSD1306_Display();
    GPIO_WriteLow(TEST_Port, TEST_Pin);
  }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
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
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
