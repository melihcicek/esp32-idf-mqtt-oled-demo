/*
 * main.h
 *
 *  Created on: 17 Kas 2018
 *      Author: melih
 */

#ifndef MAIN_MAIN_H_
#define MAIN_MAIN_H_

#define GPIO_OUTPUT_IO_1	22
#define GPIO_OUTPUT_IO_0    21
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))



#define OLED_LOCK_MUTEX() \
  do {                       \
  } while (xSemaphoreTake(oled_mutex, portMAX_DELAY) != pdPASS)

#define OLED_UNLOCK_MUTEX() xSemaphoreGive(oled_mutex)


#endif /* MAIN_MAIN_H_ */
