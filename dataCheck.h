#ifndef __CHECK_DATA_H__
#define __CHECK_DATA_H__

#include <mce/BasicThread.h>
#include <rtdb/rdbtable.h>
#include "common.h"


class clsDataCheck
{
private:
	friend class MceUtil::SingletonHolder<clsDataCheck>;
	clsDataCheck();
	~clsDataCheck();
public:
	void start();
	void stop();
	int initTable();
public:
	static clsDataCheck& getInstance();
private:
	static MCE_THR_FUNC_RETURN thread_proc onModbus(void* pThreadPara);
	void do_modbusDigital();
	void do_modbusAnalog();
	void do_modbusAnalogBit();

	Mce::float32 getModbusAnalogValue(const strTbModbusAnalog& obj);
	Mce::uint8 getModbusDigitalValue(const strTbModbusDigital& obj);
	Mce::uint8 getModbusRegBitValue(const strTbModbusRegBit& obj);
	int getRtuStatusByName(const char* rtuName);
	int getChanStatusByName(const char* chanName);

	
private:
	static bool					  _bRunningFlag;
	MceUtil::BasicThread          _modbusThread;
	RdbTable                      _channelTable;
    RdbTable                      _rtuTable;

};


#endif  //__CHECK_DATA_H__