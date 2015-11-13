/*
	template.h

	Author: MiDi$
*/

#ifndef _TEMPLATE_H
#define _TEMPLATE_H

	//������ ������������ ���� ����� TRN
	ROBOEX_QUIK_STRUCT roboex_quik_trn_template[DDE_TRN_SIZE] = {
		{"TRADENUM", {0}, NULL},					// "�����"
		{"TRADEDATE", {0}, NULL},					// "����"
		{"TRADETIME", {0}, NULL},					// "�����"
		{"SECURITY_SHORT_NAME", {0}, NULL},			// "������ ����."
		{"SECURITY_NAME", {0}, NULL},				// "������"
		{"SECCODE", {0}, NULL},						// "��� ������"
		{"CLASS_NAME", {0}, NULL},					// "�����"
		{"CLASSCODE", {0}, NULL},					// "��� ������"
		{"PRICE", {0}, NULL},						// "����"
		{"QTY", {0}, NULL},							// "���-��"
		{"VALUE", {0}, NULL},						// "�����"
		{"BUYSELL", {0}, NULL},						// "��������"
		{"SETTLECODE", {0}, NULL},					// "��� ��������"
		{"YIELD", {0}, NULL},						// "����������"
		{"ACCRUEDINT", {0}, NULL},					// "�������� �����"
		{"REPORATE", {0}, NULL},					// "������ ����(%)"
		{"REPOVALUE", {0}, NULL},					// "����� ����"
		{"REPO2VALUE", {0}, NULL},					// "����� ������ ����"
		{"REPOTERM", {0}, NULL}						// "���� ����"	
	};

	//������ ������������ ���� ����� CVAL
	ROBOEX_QUIK_STRUCT roboex_quik_cval_template[DDE_CVAL_SIZE] = {
		{"LONGNAME", {0}, NULL},			//������
		{"SHORTNAME", {0}, NULL},			//������ ����.
		{"CODE", {0}, NULL},				//��� ������
		{"ISINCODE", {0}, NULL},			//ISIN-��� ������
		{"REGNUMBER", {0}, NULL},			//���.����� 
		{"CLASSNAME", {0}, NULL},			//�����
		{"CLASS_CODE", {0}, NULL},			//��� ������
		{"TRADE_DATE_CODE", {0}, NULL},		//����������
		{"MAT_DATE", {0}, NULL},			//���������
		{"DAYS_TO_MAT_DATE", {0}, NULL},	//�� ���������
		{"SEC_FACE_VALUE", {0}, NULL},		//�������
		{"SEC_FACE_UNIT", {0}, NULL},		//������
		{"SEC_SCALE", {0}, NULL},			//��������
		{"SEC_PRICE_STEP", {0}, NULL},		//��� ����
		{"STATUS", {0}, NULL},				//������
		{"BID", {0}, NULL},					//�����
		{"BIDDEPTH", {0}, NULL},			//���. �����
		{"BIDDEPTHT", {0}, NULL},			//���. �����
		{"NUMBIDS", {0}, NULL},				//������ ���.
		{"OFFER", {0}, NULL},				//�����.
		{"OFFERDEPTH", {0}, NULL},			//���. �����.
		{"OFFERDEPTHT", {0}, NULL},			//���. �����.
		{"NUMOFFERS", {0}, NULL},			//������ ����.
		{"OPEN", {0}, NULL},				//����.
		{"HIGH", {0}, NULL},				//����. ����
		{"LOW", {0}, NULL},					//���. ����
		{"LAST", {0}, NULL},				//���� ������.
		{"TIME", {0}, NULL},				//����� ������.
		{"VOLTODAY", {0}, NULL},			//����� ���-��
		{"VALTODAY", {0}, NULL},			//������
		{"TRADINGSTATUS", {0}, NULL},		//������
		{"VALUE", {0}, NULL},				//������ ����.
		{"YIELD", {0}, NULL},				//����������
		{"NUMTRADES", {0}, NULL},			//���-�� ������
		{"QUANTITY", {0}, NULL},			//���-�� ������.
		{"SETTLEPRICE", {0}, NULL},			//����. ����
		{"LASTTOPREVSTLPRC", {0}, NULL},	//��� � �� ����.
		{"PREVSETTLPRICE", {0}, NULL},		//�������� ���.
		{"PRICEMVTLIMIT", {0}, NULL},		//��� ��� ���.
		{"PRICEMVTLIMITT1", {0}, NULL},		//���.���.���.
		{"MAXOUTVOLUME", {0}, NULL},		//���.���.����.
		{"PRICEMAX", {0}, NULL},			//����.����.����
		{"PRICEMIN", {0}, NULL},			//���.����.����
		{"NEGVALTODAY", {0}, NULL},			//������ ����.
		{"NEGNUMTRADES", {0}, NULL},		//���.����.����.
		{"NUMCONTRACTS", {0}, NULL},		//���-�� ���.���.
		{"STRIKE", {0}, NULL},				//������
		{"OPTIONTYPE", {0}, NULL},			//��� �������
		{"OPTIONBASE", {0}, NULL},			//���.�����
		{"PRICEVALUE", {0}, NULL},			//����.��.���.
		{"OPENPOSITIONCHAN", {0}, NULL},	//���.����.���.
		{"FIRSTTRADEDATE", {0}, NULL},		//����.�����
		{"LASTTRADEDATE", {0}, NULL},		//����.�����
		{"DAYSTOMATURITY", {0}, NULL},		//��.�� ���.
		{"PREVDATE", {0}, NULL},			//���� ���.���
		{"PRICESCANRANGE", {0}, NULL},		//����.����.
		{"SELLDEPO", {0}, NULL},			//�� ��������
		{"BUYDEPO", {0}, NULL},				//�� ����������
		{"CHANGE", {0}, NULL},				//�����. � ����.
		{"PRICEMINUSPREVWA", {0}, NULL},	//� ����. ����.
		{"QTY", {0}, NULL},					//���-�� ������.
		{"ACCRUEDINT", {0}, NULL},			//% �����
		{"WAPRICE", {0}, NULL},				//��. ���. ����
		{"HIGHBID", {0}, NULL},				//����. �����
		{"LOWOFFER", {0}, NULL},			//����. ����
		{"YIELDATWAPRICE", {0}, NULL},		//��. ���. �����.
		{"CLOSEPRICE", {0}, NULL},			//����.
		{"CLOSEYIELD", {0}, NULL},			//����. �����.
		{"PREVWAPRICE", {0}, NULL},			//����.��.
		{"YIELDATPREVWAPRI", {0}, NULL},	//����. ����. ����
		{"LOTSIZE", {0}, NULL},				//���
		{"PREVPRICE", {0}, NULL}			//���� ����
	};

#endif