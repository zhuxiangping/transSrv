#include <mce/SingletonHolder.h>
#include <daq/dacrawdriver.h>
#include <dac/dacprofile.h>
#include <log/ICASCOLog.h>
#include "dataCheck.h"
#include "csvFileParse.h"
#include "shareData.h"


bool clsDataCheck::_bRunningFlag(false);


clsDataCheck::clsDataCheck()
{

}


clsDataCheck::~clsDataCheck()
{

}





void 
clsDataCheck::start()
{
	_bRunningFlag = true;
	_modbusThread.start(onModbus, this);
	ICASCOLog::info("transDataIom:clsDataCheck::start()--check线程启动");
	printf("clsDataCheck::start()--check线程启动\n");
}



void 
clsDataCheck::stop()
{
	_bRunningFlag = false;
	_modbusThread.join();
	ICASCOLog::info("Process:transDataIom, clsDataCheck::stop()--stop thread completed");
	printf("clsDataCheck::stop()--stop thread completed\n");
}





MCE_THR_FUNC_RETURN thread_proc
clsDataCheck::onModbus(void* pThreadPara)
{
	clsDataCheck* pThis = reinterpret_cast<clsDataCheck*>(pThreadPara);
	while (_bRunningFlag) 
	{
		pThis->do_modbusDigital();
		pThis->do_modbusAnalog();
		pThis->do_modbusAnalogBit();
		Mce_os::sleep(1000);
	}  
	return 0;
}





Mce::float32 
clsDataCheck::getModbusAnalogValue(const strTbModbusAnalog& obj)
{
	Mce::float32 ret = 0.0;
	Mce::float32 val;
	Mce::uint8 flag;
	DacRawDriver& pDwf = DacRawDriver::getInstance();

	if(obj.pointType == 0)   //0:yc point 1:rtu status  2:channel status  3:logic calc point
	{
		int rtuNo = obj.sourceContent.sourcePointInfo.sourceRtuNo;
		int pointNo = obj.sourceContent.sourcePointInfo.sourcePointNo;

		int bRet = pDwf.getAnalogValue(rtuNo, pointNo, val, flag);
		if(bRet > 0)
		{
			ret = val;
		}
	}
	else if (obj.pointType == 1) 
	{
		int rtuSts = getRtuStatusByName(obj.sourceContent.pointName);
		ret = static_cast<Mce::float32>(rtuSts);
	}
	else if (obj.pointType == 2) 
	{
		int chanSts = getChanStatusByName(obj.sourceContent.pointName);
		ret = static_cast<Mce::float32>(chanSts);

	}
	else if (obj.pointType == 3) 
	{
		//todo
	}
	return ret;
}





Mce::uint8 
clsDataCheck::getModbusDigitalValue(const strTbModbusDigital& obj)
{
	Mce::uint8 ret = 0;
	Mce::uint8 val;
	Mce::uint8 flag;
	DacRawDriver& pDwf = DacRawDriver::getInstance();

	if(obj.pointType == 0)   //0:yx point 1:rtu status  2:channel status  3:logic calc point
	{
		int rtuNo = obj.sourceContent.sourcePointInfo.sourceRtuNo;
		int pointNo = obj.sourceContent.sourcePointInfo.sourcePointNo;

		int bRet = pDwf.getDigitalValue(rtuNo, pointNo, val, flag);
		if(bRet > 0)
		{
			ret = val;
		}
	}
	else if (obj.pointType == 3)   //calc point
	{
		//todo
	}
	return ret;

}


Mce::uint8 
clsDataCheck::getModbusRegBitValue(const strTbModbusRegBit& obj)
{
	Mce::uint8 ret = 0;
	Mce::uint8 val;
	Mce::uint8 flag;
	DacRawDriver& pDwf = DacRawDriver::getInstance();

	if(obj.pointType == 0)   //0:yx point 1:rtu status  2:channel status  3:logic calc point
	{
		int rtuNo = obj.sourceContent.sourcePointInfo.sourceRtuNo;
		int pointNo = obj.sourceContent.sourcePointInfo.sourcePointNo;

		int bRet = pDwf.getDigitalValue(rtuNo, pointNo, val, flag);
		if(bRet > 0)
		{
			ret = val;
		}
	}
	else if (obj.pointType == 3)   //calc point
	{

		//todo
	}
	return ret;
}


int 
clsDataCheck::getRtuStatusByName(const char* rtuName)
{
	int nCount = _rtuTable.getRecordCount();
	if(nCount > 0)
	{
		for (int j = 0; j < nCount; j++)
		{
			MDB_RTU_POINT* pRtuPoint = (MDB_RTU_POINT*)_rtuTable.getRecordAddr(j);
			if(0 == strcmp(rtuName, pRtuPoint->RtuCode))
			{
				return pRtuPoint->State;
			}
		}
	}

	printf("clsDataCheck::getRtuStatusByName()- rtuName not find in Rtu table,name is %s\n", rtuName); 
	char log[128] = {0};
	mce_snprintf(log, sizeof(log) -1, "transDataIom:clsDataCheck::getRtuStatusByName()- rtuName not find in Rtu table,name is %s", rtuName); 
    ICASCOLog::info(log);
	return -1;
}




int 
clsDataCheck::getChanStatusByName(const char* chanName)
{
	int nCount = _channelTable.getRecordCount();
	if(nCount > 0)
	{
		for (int j = 0; j < nCount; j++)
		{
			MDB_CHANNEL_POINT* pChanPoint = (MDB_CHANNEL_POINT*)_channelTable.getRecordAddr(j);
			if(0 == strcmp(chanName, pChanPoint->ChannelCode))
			{
				return pChanPoint->State;
			}
		}
	}

	printf("clsDataCheck::getChanStatusByName()- chanName not find in chan table,name is %s\n", chanName); 
	char log[128] = {0};
	mce_snprintf(log, sizeof(log) -1, "transDataIom:clsDataCheck::getChanStatusByName()- chanName not find in chan table,name is %s", chanName); 
	ICASCOLog::info(log);
	return -1;
}







void 
clsDataCheck::do_modbusDigital()
{
	std::vector<strTbModbusDigital>& diList01 = groupCsvFile::getInstance().diTableModbus01();    //di 01 func
    for(auto it = diList01.begin(); it != diList01.end(); ++it)
	{
		strTbModbusDigital& objModbusDi = *it;
		const int& index = objModbusDi.transBitNo;
		Mce::uint8 val = getModbusDigitalValue(objModbusDi);
		sharedDataValue::_pInstance->setDigitalData(index, val, READ_COIL);
	}

	std::vector<strTbModbusDigital>& diList02 = groupCsvFile::getInstance().diTableModbus02();    //di 02 func
	for(auto it = diList02.begin(); it != diList02.end(); ++it)
	{
		strTbModbusDigital& objModbusDi = *it;
		int index = objModbusDi.transBitNo;
		Mce::uint8 val = getModbusDigitalValue(objModbusDi);
		sharedDataValue::_pInstance->setDigitalData(index, val, READ_INPUT_DISC);
	}
}




void 
clsDataCheck::do_modbusAnalog()
{
	std::vector<strTbModbusAnalog>& aiList03 = groupCsvFile::getInstance().aiTableModbus03();    //ai 03 func
	for(auto it = aiList03.begin(); it != aiList03.end(); ++it)
	{
		strTbModbusAnalog& objModbusAi = *it;
		std::string dataType = objModbusAi.dataType;
		Mce::float32 rawData = getModbusAnalogValue(objModbusAi);
		Mce::float32 data = rawData * objModbusAi.rate + objModbusAi.offset;
		int regIndex = objModbusAi.transRegisterNo;
		sharedDataValue::_pInstance->setAnalogData(regIndex, dataType, data, READ_HOLDING_REG);
	}
		
	std::vector<strTbModbusAnalog>& aiList04 = groupCsvFile::getInstance().aiTableModbus04();    //ai 04 func
	for(auto it = aiList04.begin(); it != aiList04.end(); ++it)
	{
		strTbModbusAnalog& objModbusAi = *it;
		std::string dataType = objModbusAi.dataType;
		Mce::float32 rawData = getModbusAnalogValue(objModbusAi);
		Mce::float32 data = rawData * objModbusAi.rate + objModbusAi.offset;
		int regIndex = objModbusAi.transRegisterNo;
		sharedDataValue::_pInstance->setAnalogData(regIndex, dataType, data, READ_INPUT_REG);
	}
}



void 
clsDataCheck::do_modbusAnalogBit()
{
	std::vector<strTbModbusRegBit>& aiBitList03 = groupCsvFile::getInstance().diTableModbusRegBit03();    //ai 03 func
	for(auto it = aiBitList03.begin(); it != aiBitList03.end(); ++it)
	{
		strTbModbusRegBit& objModbusAiBit = *it;
		int bitNo = objModbusAiBit.transBitNo;
		int regNo = objModbusAiBit.transRegisterNo;
		bool boolValue  = static_cast<bool>(getModbusRegBitValue(objModbusAiBit));  //这个pointNo的bool
		if(boolValue)
		{
			Mce::uint16 tmpVal = (1 << bitNo);
			Mce::uint16 regValue = sharedDataValue::_pInstance->getAnaValue(regNo, READ_HOLDING_REG);
			regValue |= tmpVal;
			Mce::float32 val = static_cast<Mce::float32>(regValue);
			sharedDataValue::_pInstance->setAnalogData(regNo, "int16", val, READ_HOLDING_REG);
		}
		else
		{
			Mce::uint16 tmpVal = (1 << bitNo);
			tmpVal = ~tmpVal;
			Mce::uint16 regValue = sharedDataValue::_pInstance->getAnaValue(regNo, READ_HOLDING_REG);
			regValue &= tmpVal;
			Mce::float32 val = static_cast<Mce::float32>(regValue);
			sharedDataValue::_pInstance->setAnalogData(regNo, "int16", val, READ_HOLDING_REG);
		}	
	}

	std::vector<strTbModbusRegBit>& aiBitList04 = groupCsvFile::getInstance().diTableModbusRegBit04();    //ai 04 func
	for(auto it = aiBitList04.begin(); it != aiBitList04.end(); ++it)
	{
		strTbModbusRegBit& objModbusAiBit = *it;
		int bitNo = objModbusAiBit.transBitNo;
		int regNo = objModbusAiBit.transRegisterNo;
		bool boolValue  = static_cast<bool>(getModbusRegBitValue(objModbusAiBit));  //这个pointNo的bool
		if(boolValue)
		{
			Mce::uint16 tmpVal = (1 << bitNo);
			Mce::uint16 regValue = sharedDataValue::_pInstance->getAnaValue(regNo, READ_INPUT_REG);
			regValue |= tmpVal;
			Mce::float32 val = static_cast<Mce::float32>(regValue);
			sharedDataValue::_pInstance->setAnalogData(regNo, "int16", val, READ_INPUT_REG);
		}
		else
		{
			Mce::uint16 tmpVal = (1 << bitNo);
			tmpVal = ~tmpVal;
			Mce::uint16 regValue = sharedDataValue::_pInstance->getAnaValue(regNo, READ_INPUT_REG);
			regValue &= tmpVal;
			Mce::float32 val = static_cast<Mce::float32>(regValue);
			sharedDataValue::_pInstance->setAnalogData(regNo, "int16", val, READ_INPUT_REG);
		}	
	}
}






namespace
{
	static MceUtil::SingletonHolder<clsDataCheck> __data_change_check;
}





clsDataCheck& 
clsDataCheck::getInstance()
{
	return *__data_change_check.get();
}

