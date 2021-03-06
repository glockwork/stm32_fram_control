#pragma once

#include "stddef.h"
#include "data_manager_config.h"

#define data_managerLOAD_DEFAULT_DATA		1

DataManager_Data_t DataManager_Data;
#ifdef data_managerLOAD_DEFAULT_DATA
DataManager_Data_t DataManager_DefaultData;
#endif

#define SIZE_OF_MEMBER(s,m) 		((size_t)sizeof(s.m))

// Макросы, обеспечивающие функциональность данного модуля
#define DataManager_Store(x)		DataManager_WriteBlock((void*)&DataManager_Data.x, SIZE_OF_MEMBER(DataManager_Data, x), offsetof(DataManager_Data_t,x))
#define DataManager_Load(x)			DataManager_ReadBlock((void*)&DataManager_Data.x, SIZE_OF_MEMBER(DataManager_Data, x), offsetof(DataManager_Data_t,x))

#define DataManager_Write(s,x)		DataManager_WriteBlock((void*)s, SIZE_OF_MEMBER(DataManager_Data, x), offsetof(DataManager_Data_t,x))
#define DataManager_Read(s,x)		DataManager_ReadBlock((void*)s, SIZE_OF_MEMBER(DataManager_Data, x), offsetof(DataManager_Data_t,x))

void DataManager_Init();
void DataManager_WriteBlock(void *buf, size_t blockSize, uint16_t blockAdr);
void DataManager_ReadBlock(void *buf, size_t blockSize, uint16_t blockAdr);

uint16_t DataManager_GetSizeOfData();
