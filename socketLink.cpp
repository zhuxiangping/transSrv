#include <mce/Socket.h>
#include <mce/SharedLibrary.h>
#include <iostream>
#include <log/ICASCOLog.h>
#include <mce/SocketException.h>
#include <mce/NetworkException.h>
#include "socketlink.h"
#include "shareData.h"
#include "common.h"
#include <mce/RealTime.h>



clsModbusLink::clsModbusLink(const Mce_net::StreamSocket& s): 
				 Mce_net::TCPServerConnection(s)

{
	_lastTime = 0;	
	_establishTime = getNowSecond();
	_ss = socket();
	_port = _ss.address().port();
	_maxLink =  sharedDataValue::_pInstance->getMaxLink();
	
	sharedDataValue::_pInstance->addConnection(_port);
	printf("新连接, 监听信息%s,当前连接数%d\n", _ss.address().toString().c_str(), sharedDataValue::_pInstance->getConnection(_port));
}



clsModbusLink::~clsModbusLink()
{
	sharedDataValue::_pInstance->deConnection(_port);
	printf("删除连接, 监听信息%s,当前连接数%d\n", _ss.address().toString().c_str(), sharedDataValue::_pInstance->getConnection(_port));
}




//void 
//clsModbusLink::clear()
//{
//	_ss.close();
//}



void 
clsModbusLink::run()
{
	try
	{		
		try
		{		
			if(sharedDataValue::_pInstance->getConnection(_port) > _maxLink)
			{
				return;
			}
			_ss.setReceiveBufferSize(1024 * 1000);
			_ss.setReceiveTimeout(MceUtil::Time::milliSeconds(10000));

			int nRet = 0;
			MceUtil::Time timeout = MceUtil::Time::milliSeconds(1000);;
			while (1)
			{
				if(getNowSecond() - _establishTime > 30 && _lastTime == 0)
				{
					return;
				}

				if(_lastTime != 0 && getNowSecond() - _lastTime >= 10)
				{
					return;
				}
					
				if (_ss.poll(timeout, Mce_net::Socket::SELECT_READ))
				{
					nRet = onMessage();
					if (nRet < 0)
					{
						printf("RipConnection::run()---reader.onMessage() fail\n");
						char log[128] = {0};
						mce_snprintf(log, sizeof(log) -1, "transDataIom:RipConnection::run()--reader.onMessage() fail");
						ICASCOLog::error(log);
						break;
					}
				}
			}
		}
		catch (Mce_net::NetException& ex)
		{
			printf("RipConnection::run()--Mce_net::NetException& ex--RipConnection系统网络异常!\n");
			char log[256] = {0};
			mce_snprintf(log, sizeof(log) -1, "transDataIom:RipConnection::run()--Mce_net::NetException& ex--RipConnection系统网络异常!");
			ICASCOLog::error(log);
		}
		catch (Mce::Exception& exc)
		{
			printf("RipConnection::run()--Mce_net::Exception& ex--RipConnection系统网络异常!\n");
			char log[256] = {0};
			mce_snprintf(log, sizeof(log) -1, "transDataIom:RipConnection::run()--Mce_net::Exception& ex--RipConnection系统网络异常!");
			ICASCOLog::error(log);
		}
	}
	catch (Mce_net::NetException& ex)
	{
		char szLog[256] = {0};
		mce_snprintf(szLog, sizeof(szLog), "transDataIom:clsLink::run()--Mce_net::NetException& ex-%s", (char*)ex.what());
		ICASCOLog::info(szLog);
	}
	catch (Mce::Exception& exc)
	{
		char szLog[256] = {0};
		mce_snprintf(szLog, sizeof(szLog), "transDataIom:clsLink::run()--Mce::Exception& ex-%s", (char*)exc.what());
		ICASCOLog::info(szLog);
	}
}




int  
clsModbusLink::onMessage()
{
	int ret = 0;
	int nLen = 0;
	int nData = 0;
	Mce::uint8 buf[2048] = {0};
    nLen = _ss.receiveBytes(buf, kLength);

    if(nLen <= 0)
    {
	   ret = -1;
    }
   else
    {
		_lastTime = getNowSecond();
		
		while(nLen > 0)
		{
			int framePerLen = buf[4] * 256 + buf[5] + 6;
			EXECEPTION_CODE errorCode = frameValidCheck(buf + nData, framePerLen);
			if(errorCode == VALID)
			{
				processFrame(buf + nData, framePerLen);
			}
			else
			{
				processException(buf + nData, framePerLen, errorCode);
			}
			nData += framePerLen;
			nLen -= framePerLen;
		}
    }
	return ret;
}



bool 
clsModbusLink::isFuncCodeOk(const int& funcCode)
{
	bool ret = true;
	if(funcCode != READ_COIL && funcCode != READ_INPUT_DISC && funcCode != READ_HOLDING_REG  \
	   && funcCode != READ_INPUT_REG  && funcCode != WRITE_SINGLE_COIL &&  \
	   funcCode != WRITE_MULTI_COIL && funcCode != WRITE_SINGLE_REG && funcCode != WRITE_SINGLE_REG &&  \
	    funcCode != WRITE_MULTI_REG)
	{
		ret = false;
	}
	return ret;
}



bool 
clsModbusLink::isAddrOk(Mce::uint8* buf, const int& len)
{
	bool ret = true;
	int funcCode = buf[7];
	
	if(funcCode == READ_COIL || funcCode == READ_INPUT_DISC)
	{
		int num = buf[10] * 256 + buf[11];
		int begAddr = buf[8] * 256  + buf[9];
		int boolMax = sharedDataValue::_pInstance->boolMax();
		for(int i = 0; i < num; i++)
		{
			if(begAddr + i > boolMax ||  begAddr + i <= 0)
			{
				ret = false;
				break;
			}
		}
	}
	else if(funcCode == READ_HOLDING_REG || funcCode == READ_INPUT_REG || funcCode == WRITE_MULTI_COIL || funcCode == WRITE_MULTI_REG)
	{
		int num = buf[10] * 256 + buf[11];
		int begAddr = buf[8] * 256  + buf[9];
		int int16Max = sharedDataValue::_pInstance->int16Max();
		for(int i = 0 ; i < num; i++)
		{
			if(begAddr + i > int16Max  ||  begAddr + i <= 0)
			{
				ret = false;
				break;
			}
		}
	}
	else if(funcCode == WRITE_SINGLE_COIL)
	{
		int begAddr = buf[8] * 256  + buf[9];
		int boolMax = sharedDataValue::_pInstance->boolMax();
		if(begAddr > boolMax ||  begAddr <= 0)
		{
			ret = false;
		}
	} 
	else if(funcCode == WRITE_SINGLE_REG)
	{
		int begAddr = buf[8] * 256  + buf[9];
		int int16Max = sharedDataValue::_pInstance->int16Max();
		if(begAddr > int16Max ||  begAddr <= 0)
		{
			ret = false;
		}
	}
	else
	{
		//dummy
	}
	return ret;
}




bool 
clsModbusLink::isDataOk(Mce::uint8* buf, const int& len)
{
	bool ret = true;
	int affairNo = buf[0] * 256 + buf[1];
	int protoId = buf[2] * 256 + buf[3];
	int unitId = buf[6];
	int funcCode = buf[7];
	int begAddr = buf[8] * 256  + buf[9];

	if(protoId != 0 && unitId != 0)
	{
		ret = false;
	}
	else if(funcCode == READ_COIL || funcCode == READ_INPUT_DISC)
	{
		int num = buf[10] * 256 + buf[11];
		if(num <= 0 && num > 2000)
		{
		  ret = false;
		}
	}
	else if (funcCode == READ_HOLDING_REG || funcCode != READ_INPUT_REG )
	{
		int num = buf[10] * 256 + buf[11];
		if(num <= 0 && num > 125)
		{
			ret = false;
		}
	}
	else if(funcCode == WRITE_SINGLE_COIL) 
	{
		if(!((buf[10] == 00 && buf[11] == 00) || (buf[10] == 0xff && buf[11] == 00)))
		{
			ret = false;
		}
	}
	else if(funcCode == WRITE_MULTI_COIL)
	{
		int num = buf[10] * 256 + buf[11];
		if(num <= 0 && num > 1968)
		{
			ret = false;
		}
	}
	else if(funcCode == WRITE_SINGLE_REG)
	{
		//dummy
	}
    else if(funcCode == WRITE_MULTI_REG)
	{
		int num = buf[10] * 256 + buf[11];
		if(num <= 0 && num > 123)
		{
			ret = false;
		}
	}
	return ret;
}





EXECEPTION_CODE 
clsModbusLink::frameValidCheck(Mce::uint8* buf, const int& len)
{
	EXECEPTION_CODE ret = VALID;
	int funcCode = buf[7];
	do
	{
		if (!isFuncCodeOk(funcCode))
		{
			ret = INVALID_FUNCCODE;
			break;
		}

		if(!isDataOk(buf, len))
		{
			ret = INVALID_DATA;
			break;
		}

		if (!isAddrOk(buf, len))
		{
			ret = INVALID_ADDR;
			break;
		}

		
	} while(false);

	
	if(ret == VALID)
	{
		switch (funcCode)
		{
		case READ_COIL:
		case READ_INPUT_DISC:
		case READ_HOLDING_REG:
		case READ_INPUT_REG:
			{
				modbusRead rr;
				rr.affairNo   = buf[0] * 256 + buf[1];
				rr.protoId    = buf[2] * 256 + buf[3];
				rr.unitId     = buf[6];
				rr.funcCode   = buf[7];
				rr.begAddr = buf[8] * 256  + buf[9];
				rr.num     = buf[10] * 256 + buf[11];
				_read = rr;
			}
			break;
		case WRITE_SINGLE_COIL:
		case WRITE_MULTI_COIL:
		case WRITE_SINGLE_REG:
		case WRITE_MULTI_REG:
			{
				modbusWrite  wr;
				wr.affairNo   = buf[0] * 256 + buf[1];
				wr.protoId    = buf[2] * 256 + buf[3];
				wr.unitId     = buf[6];
				wr.funcCode   = buf[7];
				wr.begAddr = buf[8] * 256  + buf[9];
				wr.num     = buf[10] * 256 + buf[11];
				_write = wr;
			}
			break;
		default:
			break;
		}
	}
	return ret;
}





void 
clsModbusLink::processFrame(Mce::uint8* buf, const int& len)
{
	Mce::uint8 sendBuf[259] = {0};
	int index = 0;
	
	int funcCode = buf[7];
	switch (funcCode)
	{
	case WRITE_SINGLE_COIL:
	case WRITE_MULTI_COIL:
	case WRITE_SINGLE_REG:
	case WRITE_MULTI_REG:
		{
			sendBuf[index++] = _write.affairNo / 256;
			sendBuf[index++] = _write.affairNo % 256;
			sendBuf[index++] = _write.protoId / 256;
			sendBuf[index++] = _write.protoId % 256;
			sendBuf[index++] = 0;
			sendBuf[index++] = 6;
			sendBuf[index++] = _write.unitId;
			sendBuf[index++] = _write.funcCode;
			sendBuf[index++] = _write.begAddr / 256;
			sendBuf[index++] = _write.begAddr % 256;
			sendBuf[index++] = _write.num / 256;
			sendBuf[index++] = _write.num % 256;
			_ss.sendBytes(sendBuf, index); 
		}
		break;

	case READ_HOLDING_REG:
	case READ_INPUT_REG:
		{
			sendBuf[index++] = _read.affairNo / 256;
			sendBuf[index++] = _read.affairNo % 256;
			sendBuf[index++] = _read.protoId / 256;
			sendBuf[index++] = _read.protoId % 256;
			sendBuf[index++] = 0;
			sendBuf[index++] = _read.num * 2 + 3;
			sendBuf[index++] = _read.unitId;
			sendBuf[index++] = _read.funcCode;
			sendBuf[index++] = _read.num * 2;

			int begAddr = _read.begAddr;
			int num = _read.num;
			for(int i = 0; i < num; i++)
			{
				int addr = begAddr + i;
				Mce::uint16 val = sharedDataValue::_pInstance->getAnaValue(addr, _read.funcCode);
				sendBuf[index++] = val /256;
				sendBuf[index++] = val % 256;
			}
			_ss.sendBytes(sendBuf, index); 
		}
		break;

	case READ_COIL:
	case READ_INPUT_DISC:
		{
			sendBuf[index++] = _read.affairNo / 256;
			sendBuf[index++] = _read.affairNo % 256;
			sendBuf[index++] = _read.protoId / 256;
			sendBuf[index++] = _read.protoId % 256;
			sendBuf[index++] = 0;
			int length = _read.num / 8 +  ((_read.num % 8 == 0) ? 0 : 1);;
			sendBuf[index++] = length + 3;
			sendBuf[index++] = _read.unitId;
			sendBuf[index++] = _read.funcCode;
			sendBuf[index++] = length;

			int begAddr = _read.begAddr;
			int num = _read.num;
			Mce::uint8 val[250] = {0};
			int byteIndex = 0;
			int bitIndex = 0;
			for(int i = 0; i < num; i++)
			{
				int addr = begAddr + i;
				bool tmpBool = static_cast<bool>(sharedDataValue::_pInstance->getDigitalValue(addr, _read.funcCode));
				byteIndex = i / 8;
				bitIndex = i % 8;
				val[byteIndex] = val[byteIndex] | (tmpBool << bitIndex);
				sendBuf[index + byteIndex] = val[byteIndex];
			}
			_ss.sendBytes(sendBuf, index + byteIndex + 1); 
		}
		break;
	default:
		break;
	}
}





void 
clsModbusLink::processException(Mce::uint8* buf, const int& len, EXECEPTION_CODE& execeptCode)
{
	Mce::uint8 sendBuf[259] = {0};
	int index = 0;

	sendBuf[index++] = buf[0];     //affair no hi
	sendBuf[index++] = buf[1];     //affair no low
	sendBuf[index++] = buf[2];     //protocol hi
	sendBuf[index++] = buf[3];     //protocol low
	sendBuf[index++] = buf[4];     //unit id
	sendBuf[index++] = 3;          //len

	sendBuf[index++] = buf[6];             //unit id
	sendBuf[index++] = buf[7] + 0x80;     //funccode
	sendBuf[index++] = static_cast<Mce::uint8>(execeptCode);            //unit id

	_ss.sendBytes(sendBuf,index); 
}


time_t
clsModbusLink::getNowSecond()
{
	STANDARDTIME TmpEventTime;
	MceUtil::RealTime SelTime;
	Mce::ushort msec = 0;
	SelTime.getNow(TmpEventTime, msec);
	return TmpEventTime;
}
