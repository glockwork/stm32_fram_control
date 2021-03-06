#include "data_manager.h"
#include "memory_rewrite_handler.h"
#include "stm32f10x.h"

void DataManager_Test()
{
	volatile uint32_t rdErr = 0;
	volatile uint32_t wrErr = 0;

	// Попытка загрузить данные из очищенной памяти.
	DataManager_Load(data8);
	DataManager_Load(dataFloat);
	DataManager_Load(data32);

	assert_param(DataManager_Data.data8 == DataManager_DefaultData.data8);
	assert_param(DataManager_Data.dataFloat == DataManager_DefaultData.dataFloat);
	assert_param(DataManager_Data.data32 == DataManager_DefaultData.data32);

	// Запись данных из локальных переменных
	uint8_t _data8 = 2;
	float _dataFloat = 4;
	uint32_t _data32 = 5;

	DataManager_Write(&_data8, data8);
	DataManager_Write(&_dataFloat, dataFloat);
	DataManager_Write(&_data32, data32);

	_data8 = 0;
	_dataFloat = 0;
	_data32 = 0;

	// Чтение данных в локальные переменные
	DataManager_Read(&_data8, data8);
	DataManager_Read(&_dataFloat, dataFloat);
	DataManager_Read(&_data32, data32);

	assert_param(_data8 == 2);
	assert_param(_dataFloat == 4);
	assert_param(_data32 == 5);

	// копирование данных в структуру данных
	DataManager_Data.data8 = _data8 + 8;
	DataManager_Data.data32 = _data32 + 32;
	DataManager_Data.dataFloat = _dataFloat + 0.56f;

	// запись данных из структуры данных
	DataManager_Store(data8);
	DataManager_Store(dataFloat);
	DataManager_Store(data32);

	// обнуление структуры данных
	DataManager_Data.data8 = 0;
	DataManager_Data.data32 = 0;
	DataManager_Data.dataFloat = 0;

	// загрузка данных в структуру данных
	DataManager_Load(data8);
	DataManager_Load(dataFloat);
	DataManager_Load(data32);

	assert_param(DataManager_Data.data8 == (2 + 8) );
	assert_param(DataManager_Data.dataFloat == (4 + 0.56f));
	assert_param(DataManager_Data.data32 == (5 + 32));


	rdErr = MemoryRewriteHandler_GetReadErrors();
	wrErr = MemoryRewriteHandler_GetWriteErrors();
	assert_param(rdErr == 0);
	assert_param(wrErr == 0);
	if ( (rdErr != 0) || (wrErr != 0) )	{
		MemoryRewriteHandler_ReserErrors();
	}
}
