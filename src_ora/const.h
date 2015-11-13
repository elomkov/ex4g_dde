/*
	const.h

	Author: MiDi$
*/

#ifndef _CONST_H
#define _CONST_H

#ifdef  __cplusplus
extern "C" {
#endif

//Отладка
	#define _DBG_DDE

//Некоторые размерности
	#define WORD_SIZE sizeof(WORD)
	#define WORD_DIM 0xFFFF
	#define DOUBLE_SIZE sizeof(double)
	#define STRUCT_COL_SIZE 24
	#define NUM_LEN 32
	#define INI_LEN 256
	#define MAX_THREADS 64

//Поддерживаемые хранимые процедуры и размерности таблиц
	//--//
	#define DDE_TRN_SIZE 19
	#define DDE_TRN_OK 1
	//--//
	#define DDE_CVAL_SIZE 72
	#define DDE_CVAL_OK 2
	#define CVAL_MAX_THREADS 1 //Количество потоков для CVAL
	//--//
	#define DDE_TXT_TRN_OK 3
	//--//
	#define DDE_PLUGIN_TRN_OK 4

//Коды ошибок
	#define RET_OK		      0				//Все ОК
	#define ERR_MALLOC	      1111111100	//Ошибка выделения памяти
	#define ERR_ARGS	      1111111101	//Ошибка аргумента функции
	#define ERR_ORA_OBJ	      1111111102	//Ошибка определения ORA объекта
	#define ERR_XL_TYPE	      1111111103	//Ошибка определения типа XLTable
	#define ERR_ORA_STATUS    1111111104	//Ошибка определния статуса ORA объекта
	#define ERR_ORA_CONNECT   1111111105	//Ошибка ORA соединения
	#define ERR_OO4O_INIT     1111111106	//Ошибка инициализации OO4O
	#define ERR_OO4O_POOL     1111111107	//Ошибка инициализации пула коннектов
	#define ERR_DB_POOL_IDX   1111111108	//Ошибка получения индекса в контексте пула сессий
	#define ERR_CTX_EXISTS    1111111109	//Попытка повторной инициализации контекста
	#define ERR_MAX_THREADS   1111111110	//Превышен лимит максимального числа потоков
	#define ERR_MAX_DIM       1111111111	//Превышена допустимая размерность переменной
	#define ERR_FUNC_NOT_FIND 1111111112	//Функция не определена
	#define ERR_QUEUE_FULL    1111111113	//Очередь заполнена
	#define ERR_QUEUE_EMPTY   1111111114	//Пустая очередь

//DDE константы
	#define DDE_FLOAT_BLK  0x0001 //Тип tdtFloat
	#define DDE_STRING_BLK 0x0002 //Тип tdtString
	#define DDE_TABLE_BLK  0x0010 //Тип tdtTable

//SHELL
	#define CMD_LEN 8
	#define CMD_EXIT TEXT("exit")
	#define CMD_HELP TEXT("help")

//Имена файлов
	#define INI_FILE_NAME TEXT("ex4g_dde.ini")
	#define LOG_FILE_NAME TEXT("ex4g_dde.log")

#ifdef  __cplusplus
}
#endif

#endif
