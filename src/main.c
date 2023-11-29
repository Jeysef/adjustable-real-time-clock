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
#include "lib_rotary.h"

#include "DS1302.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define WRITE_TIME FALSE
#define RE_BTN_PIN GPIO_PIN_5
#define RE_BTN_PORT GPIOB
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
bool read_btn(void);
/* Privete functions ---------------------------------------------------------*/
bool read_btn(void)
{
  static uint8_t last_state = 0;
  bool current_state = GPIO_ReadInputPin(RE_BTN_PORT, RE_BTN_PIN) == 0;
  // if (current_state && !last_state)
  // {
  //   last_state = current_state;
  //   return TRUE;
  // }
  // last_state = current_state;
  // return FALSE;
  if (current_state != last_state)
  {
    last_state = current_state;
    return current_state;
  }
  return FALSE;
}
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
  alps_init(60, ALPS_ROLLOVER);
  init_ms();

  GPIO_Init(RE_BTN_PORT, RE_BTN_PIN, GPIO_MODE_IN_PU_NO_IT);

  uint8_t changingTime = 0b00000000;

  // set time to Tu 08:09 13.06.23
  // 1 - year
  // 2 - month
  // 3 - date
  // 4 - hour
  // 5 - min
  // 6 - sec
  // 7 - day
  // uint8_t buf[7] = {00, 23, 06, 13, 08, 09, 02};
  // convert to hex

  if (WRITE_TIME)
  {
    DS1302_Init();
    uint8_t buf[7] = {0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    // uint8_t buf[7] = {0x07, 0x17, 0x06, 0x12, 0xB, 0x20, 0x00};
    DS1302_WriteTime(buf);
  }
  else
  {
    DS1302_No_Reset_Init();
  }

  /* SSD1306 configuration */
#ifdef SSD1306_I2C_CONTROL
  SSD1306_I2cInit();
#endif
#ifdef SSD1306_SPI_CONTROL
  SSD1306_SpiInit();
#endif

  /* SSD1306 demo code */

  // SSD1306_Bitmap((uint8_t *)picture);
  // delay_ms(2000);
  uint8_t font_width = 5;
  uint8_t font_height = 7;

  GFX_SetFont(font_7x5);
  GFX_SetFontSize(3);

  /* Infinite loop */
  for (uint8_t i = 0;; i += 10)
  {
    alps_poll();
    SSD1306_Clear(BLACK);
    if (read_btn())
    {
      // rotate changing time to left
      // if changing time is 0, then set to 1
      if (changingTime == 0)
      {
        changingTime = 0b00000001;
      }
      else
      {
        changingTime = changingTime << 1;
      }
    }

    // uint8_t buf[7];
    // DS1302_ReadTime(buf);
    // char str[20];
    // // draw date
    // sprintf(str, "%02d.%02d.%02d", buf[3], buf[2], buf[1]);
    // GFX_DrawString(4, 2, str, WHITE, BLACK);
    // // draw time
    // sprintf(str, "%02d:%02d:%02d", buf[4], buf[5], buf[6]);
    // GFX_DrawString(4, 20, str, WHITE, BLACK);

    GFX_DrawString(4, 2, "00.00.00", WHITE, BLACK);

    if (i > 128)
    {
      // find position of current changing time
      uint8_t currentChangingTime = 0;
      for (uint8_t j = 0; j < 8; j++)
      {
        if (changingTime & (1 << j))
        {
          currentChangingTime = j;
          break;
        }
      }

      uint8_t skip_x = currentChangingTime * font_width*9;
      uint8_t skip_x_to_second_num = font_width*3;

      uint8_t x_position = 4 + skip_x + currentChangingTime*3 + skip_x_to_second_num + 1;

      // blink clock segment
      GFX_DrawFillRectangle(x_position, 2, font_width, font_height, WHITE);
    }

    SSD1306_Display();
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
  file = file;
  line = line;
  while (1)
  {
  }
}
#endif

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
