/*
 * i2c.h
 *
 *  Created on: 15 Kas 2018
 *      Author: melih
 */

#ifndef MAIN_I2C_DRV_H_
#define MAIN_I2C_DRV_H_
#include "driver/i2c.h"
#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)

#define I2C_MASTER_NUM I2C_NUMBER(0) /*!< I2C port number for master dev */

#define I2C_MASTER_SCL_IO 19               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 18               	/*!< gpio number for I2C master data  */
//#define I2C_MASTER_NUM I2C_NUMBER(CONFIG_I2C_MASTER_PORT_NUM) /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 400000						        /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */

#define CONFIG_I2C_SLAVE_ADDRESS	0x3C

#define ESP_SLAVE_ADDR CONFIG_I2C_SLAVE_ADDRESS /*!< ESP32 slave address, you can set any 7bit value */
#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */


esp_err_t i2c_master_init(void);
esp_err_t i2c_master_write_slave(i2c_port_t i2c_num, uint8_t *data_wr, size_t size);


#endif /* MAIN_I2C_DRV_H_ */
