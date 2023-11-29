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
#define RE_BTN_PIN GPIO_PIN_5
#define RE_BTN_PORT GPIOB
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
bool read_btn(void);
uint8_t get_current_changing_position(uint8_t changing_time);
/* Privete functions ---------------------------------------------------------*/
bool read_btn(void)
{
  static uint8_t last_state = 0;
  bool current_state = GPIO_ReadInputPin(RE_BTN_PORT, RE_BTN_PIN) == 0;
  if (current_state != last_state)
  {
    last_state = current_state;
    return current_state;
  }
  return FALSE;
}
uint8_t get_current_changing_position(uint8_t changing_time)
{
  uint8_t currentChangingTime = 0;
  for (uint8_t j = 0; j < 8; j++)
  {
    if (changing_time & (1 << j))
    {
      currentChangingTime = j;
      break;
    }
  }
  return currentChangingTime;
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

  uint8_t max_changing_time = 0b01000000;
  uint8_t changing_time = max_changing_time;

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
  // uint8_t buf[7] = {0x07, 0x17, 0x06, 0x12, 0xB, 0x20, 0x00};

  // 1 - date
  // 2 - month
  // 3 - year
  // 4 - hour
  // 5 - min
  // 6 - sec
  uint8_t alps_steps[6] = {30, 12, 99, 24, 60, 60};

  // uint8_t buf[7] = {0x07, 0x17, 0x06, 0x12, 0xB, 0x20, 0x00};
  DS1302_No_Reset_Init();

  // DS1302_WriteTime(buf);

/* SSD1306 configuration */
#ifdef SSD1306_I2C_CONTROL
  SSD1306_I2cInit();
#endif
#ifdef SSD1306_SPI_CONTROL
  SSD1306_SpiInit();
#endif

  // SSD1306_Bitmap((uint8_t *)picture);
  // delay_ms(2000);

  uint8_t font_width = 5;
  uint8_t font_height = 7;
  uint8_t font_scale = 3;

  GFX_SetFont(font_7x5);
  GFX_SetFontSize(font_scale);

  uint8_t buf_position;
  uint8_t current_changing_position = get_current_changing_position(changing_time);

  /* Infinite loop */
  for (uint8_t i = 0;; i += 10)
  {
    alps_poll();
    
    SSD1306_Clear(BLACK);

    uint8_t buf[7];
    DS1302_ReadTime(buf);

    alps_poll();

    if (read_btn())
    {
      i = 0;

      if (changing_time >= max_changing_time)
      {
        changing_time = 0b00000001;
      }
      else
      {
        changing_time = changing_time << 1;
      }

      current_changing_position = get_current_changing_position(changing_time);
      alps_init(alps_steps[current_changing_position], ALPS_ROLLOVER);

      buf_position = current_changing_position + 1;
      switch (current_changing_position)
      {
      case 0:
        buf_position = 3;
        break;
      case 1:
        buf_position = 2;
        break;
      case 2:
        buf_position = 1;
        break;
      }
      alps_set_value(buf[buf_position]);
    }

    alps_poll();

    if (changing_time != max_changing_time)
    {
      uint8_t value = alps_value();
      if (value != buf[buf_position])
      {
        i = 128;
      }
      buf[buf_position] = value;
      DS1302_WriteTime(buf);
    }

    char str[20];
    // draw date
    sprintf(str, "%02d.%02d.%02d", buf[3], buf[2], buf[1]);
    GFX_DrawString(4, 2, str, WHITE, BLACK);
    // draw time
    sprintf(str, "%02d:%02d:%02d", buf[4], buf[5], buf[6]);
    GFX_DrawString(4, 20, str, WHITE, BLACK);

    alps_poll();

    if (i < 128)
    {
      // find position of current changing time
      uint8_t position = get_current_changing_position(changing_time);
      bool new_line = position > 2;
      uint8_t y_position = 2;
      if (new_line)
      {
        position -= 3;
        y_position = 20;
      }

      uint8_t skip_x = (position * font_width * font_scale + position) * 3;
      uint8_t skip_x_to_second_num = font_width * font_scale;

      uint8_t x_position = 4 + skip_x + skip_x_to_second_num + 1;

      // blink clock segment
      GFX_DrawFillRectangle(x_position, y_position, font_width, font_height, WHITE);
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
