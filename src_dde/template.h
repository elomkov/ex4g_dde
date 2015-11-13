/*
	template.h

	Author: MiDi$
*/

#ifndef _TEMPLATE_H
#define _TEMPLATE_H

	//Шаблон соответствий имен полей TRN
	ROBOEX_QUIK_STRUCT roboex_quik_trn_template[DDE_TRN_SIZE] = {
		{"TRADENUM", {0}, NULL},					// "Номер"
		{"TRADEDATE", {0}, NULL},					// "Дата"
		{"TRADETIME", {0}, NULL},					// "Время"
		{"SECURITY_SHORT_NAME", {0}, NULL},			// "Бумага сокр."
		{"SECURITY_NAME", {0}, NULL},				// "Бумага"
		{"SECCODE", {0}, NULL},						// "Код бумаги"
		{"CLASS_NAME", {0}, NULL},					// "Класс"
		{"CLASSCODE", {0}, NULL},					// "Код класса"
		{"PRICE", {0}, NULL},						// "Цена"
		{"QTY", {0}, NULL},							// "Кол-во"
		{"VALUE", {0}, NULL},						// "Объем"
		{"BUYSELL", {0}, NULL},						// "Операция"
		{"SETTLECODE", {0}, NULL},					// "Код расчетов"
		{"YIELD", {0}, NULL},						// "Доходность"
		{"ACCRUEDINT", {0}, NULL},					// "Купонный доход"
		{"REPORATE", {0}, NULL},					// "Ставка РЕПО(%)"
		{"REPOVALUE", {0}, NULL},					// "Сумма РЕПО"
		{"REPO2VALUE", {0}, NULL},					// "Объем выкупа РЕПО"
		{"REPOTERM", {0}, NULL}						// "Срок РЕПО"	
	};

	//Шаблон соответствий имен полей CVAL
	ROBOEX_QUIK_STRUCT roboex_quik_cval_template[DDE_CVAL_SIZE] = {
		{"LONGNAME", {0}, NULL},			//Бумага
		{"SHORTNAME", {0}, NULL},			//Бумага сокр.
		{"CODE", {0}, NULL},				//Код бумаги
		{"ISINCODE", {0}, NULL},			//ISIN-код бумаги
		{"REGNUMBER", {0}, NULL},			//Рег.номер 
		{"CLASSNAME", {0}, NULL},			//Класс
		{"CLASS_CODE", {0}, NULL},			//Код класса
		{"TRADE_DATE_CODE", {0}, NULL},		//Дататоргов
		{"MAT_DATE", {0}, NULL},			//Погашение
		{"DAYS_TO_MAT_DATE", {0}, NULL},	//До погашения
		{"SEC_FACE_VALUE", {0}, NULL},		//Номинал
		{"SEC_FACE_UNIT", {0}, NULL},		//Валюта
		{"SEC_SCALE", {0}, NULL},			//Точность
		{"SEC_PRICE_STEP", {0}, NULL},		//Шаг цены
		{"STATUS", {0}, NULL},				//Статус
		{"BID", {0}, NULL},					//Спрос
		{"BIDDEPTH", {0}, NULL},			//Кол. спрос
		{"BIDDEPTHT", {0}, NULL},			//Общ. спрос
		{"NUMBIDS", {0}, NULL},				//Заявки куп.
		{"OFFER", {0}, NULL},				//Предл.
		{"OFFERDEPTH", {0}, NULL},			//Кол. предл.
		{"OFFERDEPTHT", {0}, NULL},			//Общ. предл.
		{"NUMOFFERS", {0}, NULL},			//Заявки прод.
		{"OPEN", {0}, NULL},				//Откр.
		{"HIGH", {0}, NULL},				//Макс. цена
		{"LOW", {0}, NULL},					//Мин. цена
		{"LAST", {0}, NULL},				//Цена послед.
		{"TIME", {0}, NULL},				//Время послед.
		{"VOLTODAY", {0}, NULL},			//Общее кол-во
		{"VALTODAY", {0}, NULL},			//Оборот
		{"TRADINGSTATUS", {0}, NULL},		//Сессия
		{"VALUE", {0}, NULL},				//Оборот посл.
		{"YIELD", {0}, NULL},				//Доходность
		{"NUMTRADES", {0}, NULL},			//Кол-во сделок
		{"QUANTITY", {0}, NULL},			//Кол-во послед.
		{"SETTLEPRICE", {0}, NULL},			//Расч. цена
		{"LASTTOPREVSTLPRC", {0}, NULL},	//Изм к пр расч.
		{"PREVSETTLPRICE", {0}, NULL},		//Предрасч цен.
		{"PRICEMVTLIMIT", {0}, NULL},		//Лим изм цен.
		{"PRICEMVTLIMITT1", {0}, NULL},		//Лим.изм.цен.
		{"MAXOUTVOLUME", {0}, NULL},		//Лим.акт.заяв.
		{"PRICEMAX", {0}, NULL},			//Макс.возм.цена
		{"PRICEMIN", {0}, NULL},			//Мин.возм.цена
		{"NEGVALTODAY", {0}, NULL},			//Оборот внес.
		{"NEGNUMTRADES", {0}, NULL},		//Кол.внес.сдел.
		{"NUMCONTRACTS", {0}, NULL},		//Кол-во отк.поз.
		{"STRIKE", {0}, NULL},				//Страйк
		{"OPTIONTYPE", {0}, NULL},			//Тип опциона
		{"OPTIONBASE", {0}, NULL},			//Баз.актив
		{"PRICEVALUE", {0}, NULL},			//Коеф.ст.поз.
		{"OPENPOSITIONCHAN", {0}, NULL},	//Изм.откр.поз.
		{"FIRSTTRADEDATE", {0}, NULL},		//Перв.торги
		{"LASTTRADEDATE", {0}, NULL},		//Посл.торги
		{"DAYSTOMATURITY", {0}, NULL},		//Дн.до пог.
		{"PREVDATE", {0}, NULL},			//Дата псл.трг
		{"PRICESCANRANGE", {0}, NULL},		//Диап.скан.
		{"SELLDEPO", {0}, NULL},			//ГО продавца
		{"BUYDEPO", {0}, NULL},				//ГО покупателя
		{"CHANGE", {0}, NULL},				//Измен. к закр.
		{"PRICEMINUSPREVWA", {0}, NULL},	//К оцен. пред.
		{"QTY", {0}, NULL},					//Кол-во послед.
		{"ACCRUEDINT", {0}, NULL},			//% Доход
		{"WAPRICE", {0}, NULL},				//Ср. взв. цена
		{"HIGHBID", {0}, NULL},				//Лучш. спрос
		{"LOWOFFER", {0}, NULL},			//Лучш. пред
		{"YIELDATWAPRICE", {0}, NULL},		//Ср. взв. доход.
		{"CLOSEPRICE", {0}, NULL},			//Закр.
		{"CLOSEYIELD", {0}, NULL},			//Закр. доход.
		{"PREVWAPRICE", {0}, NULL},			//Пред.оц.
		{"YIELDATPREVWAPRI", {0}, NULL},	//Пред. оцен. дохо
		{"LOTSIZE", {0}, NULL},				//Лот
		{"PREVPRICE", {0}, NULL}			//Цена закр
	};

#endif