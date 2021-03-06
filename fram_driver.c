﻿/*
 * author:	Елисей Равнюшкин
 * date:	14.04.2014	19:00:00
 * file:	fram_driver.c
 * Драйвер энергонезависимой памяти FRAM
 */
#include "fram_driver.h"
#include "i2c_soft.h"
#include "stm32f10x_conf.h"
#include <string.h>

/*----------------------------------------------------------------------------
 * Прототипы приватных функций
 ----------------------------------------------------------------------------*/
static bool prv_SetAddress ( uint16_t adr );

/*----------------------------------------------------------------------------
 * Глобайльные функции
 ----------------------------------------------------------------------------*/

/**
 *	@brief	Инициализация внешней энергонезависимой памяти
 *	@param	void
 *	@return	void
 */
void Fram_Init()
{
	i2c_soft_Init();
}

/**
 *	@brief	Запись в энергонезависимую память по указанному адресу
 *			блока данных. Размер блока данных не более 4 байт.
 *	@param	void* buf - указатель на данные
 *			size_t blockSize - размер блока данных в байтах
 *			uint16_t blockAdr - адрес в энергонезависимой памяти, начиная
 *								с которого будет производится запись
 *	@return	bool - результат выполнения функции:
 *			true в случае успеха
 *			false в случае ошибки
 */
bool Fram_WriteBlock ( const void* buf, size_t blockSize, uint16_t blockAdr )
{
	assert_param(blockSize <= fram_driverMAX_BLOCK_SIZE);		// максимальный размер блока - 4 байта
																// ибо больше не требуется
	bool result;
	uint8_t data[fram_driverMAX_BLOCK_SIZE + 2];							// буфер на два байта больше.
																// Два байта для адреса ячейки

	memcpy( (void*) data + 2, (void*) buf, blockSize );			// подготавливаем буфер

	data[0] = (uint8_t) (blockAdr >> 8);
	data[1] = (uint8_t) (blockAdr & 0xFF);
																// и отправляем его
	result = i2c_soft_WriteBuffer( fram_driverBUS_ADDRESS, data, blockSize + 2 );
	return result;
}

/**
 *	@brief	Чтение из энергонезависимой памяти по указанному адресу
 *			блока данных. Размер блока данных не более 4 байт.
 *	@param	void* buf - указатель на данные
 *			size_t blockSize - размер блока данных в байтах
 *			uint16_t blockAdr - адрес в энергонезависимой памяти, начиная
 *								с которого будет производится чтение
 *	@return	bool - результат выполнения функции:
 *			true в случае успеха
 *			false в случае ошибки
 */
bool Fram_ReadBlock ( void* buf, size_t blockSize, uint16_t blockAdr )
{
	bool result;
	result = prv_SetAddress( blockAdr );				// сначала устанавливаем адрес чтения
	if ( result == false )
		return false;
	result = i2c_soft_ReadBuffer( fram_driverBUS_ADDRESS, buf, blockSize );	// читаем данные
	return result;
}

/**
 *	@brief	Тестирование ячеек энергонезависимой памяти
 *			ВНИМАНИЕ. Выполнение функции занимает продолжительное время
 *	@param	void
 *	@return	int 0 - успешное тестирование
 *			int (-1) - ошибка тестирования
 */
int Fram_Test()
{
	uint16_t i;
	uint8_t data, readData;

	for (i = 0; i < fram_driverFRAM_SIZE; ++i) {
		Fram_ReadBlock(&data, sizeof(uint8_t), i);
		data = ~data;
		Fram_WriteBlock(&data, sizeof(uint8_t), i);
		Fram_ReadBlock(&readData, sizeof(uint8_t), i);
		if (data != readData)	{
			return -1;
		}
	}
	return 0;
}

/**
 *	@brief	Определение размера энергонезависимой памяти. Во все возможные ячейки
 *			записывается нуль. В нулевую ячейку записывается маркер mark.
 *			Начиная с первой ячейки последовательным чтением ищется маркер
 *			ВНИМАНИЕ. Выполнение функции занимает продолжительное время
 *	@param	void
 *	@return	int количество ячеек памяти
 *			int (-1) - ошибка, маркер не найден.
 */
int Fram_CalculateSize()
{
	uint8_t mark = 0xB0;
	uint8_t data = 0xA0;
	uint16_t i = 0;

	for (i = 0; i < 0xFFFF; ++i) {
		Fram_WriteBlock(&data, sizeof(uint8_t), i);
	}

	Fram_WriteBlock(&mark, sizeof(uint8_t), 0);

	for (i = 1; i < 0xFFFF; ++i) {
		Fram_ReadBlock(&data, sizeof(uint8_t), i);
		if (data == mark)	{
			return i;
		}
	}
	return -1;
}

/**
 *	@brief	Заполнение ячеек памяти данными data
 *			ВНИМАНИЕ. Выполнение функции занимает продолжительное время
 *	@param	uint8_t data - заполняющие данные
 *	@return	void
 */
void Fram_Memset(uint8_t data)
{
	uint16_t i = 0;
	for (i = 0; i < fram_driverFRAM_SIZE; ++i) {
		Fram_WriteBlock(&data, sizeof(uint8_t), i);
	}
}

/*----------------------------------------------------------------------------
 * Приватные функции
 ----------------------------------------------------------------------------*/
/**
 *	@brief	Установить указатель на считываемые данные в энергонезависимой памяти
 *	@param	uint16_t adr - адрес в энергонезависимой памяти
 *	@return	bool - результат выполнения функции:
 *			true в случае успеха
 *			false в случае ошибки
 */
static bool prv_SetAddress ( uint16_t adr )
{
	int result;
	uint8_t buf[2];
	buf[0] = (uint8_t) (adr >> 8);
	buf[1] = (uint8_t) (adr & 0x00FF);
	result = i2c_soft_WriteBuffer( fram_driverBUS_ADDRESS, buf, 2 );
	return result;
}
