#include "shareData.h"
#include <mce/IniFileConfiguration.h>
#include <sstream>

static sharedDataValue TheApp;
sharedDataValue* sharedDataValue::_pInstance = &TheApp;


sharedDataValue::sharedDataValue()
{
	char szConfgiFile[256] = {0};
	char* home = getenv("SSIP_ROOT");
	memset(szConfgiFile, 0, sizeof(szConfgiFile));
	mce_snprintf(szConfgiFile, sizeof(szConfgiFile) - 1, "%s/config/transAllData.ini", home);

	MceUtil::IniFileConfiguration file(szConfgiFile);
	file.load(szConfgiFile);
	_num = file.getInt("portInFO.num", 2);	

	for(int i = 0 ; i < _num; i++)
	{
		std::string portStr = "portInFO.port" + intToStr(i + 1);
		_port[i] = file.getInt(portStr, 502 + i);
	}

	_offset = file.getInt("startAddr.offset", 0);	
	_maxLink = file.getInt("maxLink.maxLinkPerPort", 2);	

	std::string offset = file.getString("endianMode.mode", "CDBA");	

	if(offset == "ABCD")
	{
		_mode = ABCD;
	}
	else if(offset == "CDAB")
	{
		_mode = CDAB;
	}
	else
	{
		_mode = CDAB;
	}

}


sharedDataValue::~sharedDataValue()
{
	_conCount.clear();
}


void 
sharedDataValue::setAnalogData(const int&index,  const std::string& dataType,  Mce::float32& val, const int& funcCode)
{
	
	Mce::uint16 buf[4] = {0};
	if(index > 0 && index <= kInt16Max)
	{
		int num = dataToBuf(buf, dataType, _mode, val);
		if(funcCode == 0x03)
		{
			MceUtil::QuickMutex::ScopedLock lock(_mutex_03);
			for(int i = 0; i < num; i++)
			{
				if(_mode ==  ABCD)
				{
					_analog03[index + i] = buf[i];
				}
				else if(_mode ==  CDAB)
				{
					_analog03[index + i] = buf[num - 1 - i];
				}
				
			}
		}
		else if (funcCode == 0x04)
		{
			MceUtil::QuickMutex::ScopedLock lock(_mutex_04);
			for(int i = 0; i < num; i++)
			{
				if(_mode ==  ABCD)
				{
					_analog04[index + i] = buf[i];
				}
				else if(_mode ==  CDAB)
				{
					_analog04[index + i] = buf[num - 1 - i];
				}
			}
		}
	}


}


void 
sharedDataValue::setDigitalData(const int&index,  Mce::uint8& val, const int& funcCode)
{
	
	if(index > 0 && index <= kBoolMax)
	{
		if(funcCode == 0x01)
		{
			MceUtil::QuickMutex::ScopedLock lock(_mutex_01);
			_digital01[index] = val;
		}
		else if (funcCode == 0x02)
		{
			MceUtil::QuickMutex::ScopedLock lock(_mutex_02);
			_digital02[index] = val;
		}
	}
}


Mce::uint8 
sharedDataValue::getDigitalValue(const int&index, const int& funcCode)
{
	
	Mce::uint8 ret = 0;
	if(funcCode == 0x01)
	{
		MceUtil::QuickMutex::ScopedLock lock(_mutex_01);
		ret = _digital01[index];
	}
	else if(funcCode == 0x02)
	{
		MceUtil::QuickMutex::ScopedLock lock(_mutex_02);
		ret = _digital02[index];
	}
	return ret;
}


Mce::uint16 
sharedDataValue::getAnaValue(const int&index, const int& funcCode)
{
	
	Mce::uint16 ret = 0;
	if(funcCode == 0x03)
	{
		MceUtil::QuickMutex::ScopedLock lock(_mutex_03);
		ret = _analog03[index];
	}
	else if(funcCode == 0x04)
	{
		MceUtil::QuickMutex::ScopedLock lock(_mutex_04);
		ret = _analog04[index];
	}
	return ret;
}



int 
sharedDataValue::dataToBuf(Mce::uint16* buf, const std::string& dataType, ENDIAN_MODE& mode, Mce::float32& data)
{
	int ret = 0;
	
	if(dataType == "int16")
	{
		Mce::int16 val = static_cast<Mce::int16>(data);
		memcpy(buf, &val, sizeof(Mce::int16));
		ret = 1;
	}

	else if(dataType == "uint16")
	{
		Mce::uint16 val = static_cast<Mce::uint16>(data);
		memcpy(buf, &val, sizeof(Mce::uint16));
		ret = 1;
	}

	else if(dataType == "float32")
	{
		Mce::uint16* val = reinterpret_cast<Mce::uint16*>(&data);
		memcpy(buf, val, sizeof(Mce::float32));
		ret = 2;
	}

	else if(dataType == "int32")
	{
		Mce::int32 val = static_cast<Mce::int32>(data);
		memcpy(buf, &val, sizeof(Mce::int32));
		ret = 2;
	}

	else if(dataType == "uint32")
	{
		Mce::uint32 val = static_cast<Mce::uint32>(data);
		memcpy(buf, &val, sizeof(Mce::uint32));
		ret = 2;
	}

	return ret;
}


void 
sharedDataValue::addConnection(const int& port)
{
	MceUtil::QuickMutex::ScopedLock lock(_mutexLinkCnt);
	_conCount[port]++;
}


void 
sharedDataValue::deConnection(const int& port)
{
	MceUtil::QuickMutex::ScopedLock lock(_mutexLinkCnt);
	_conCount[port]--;
}


 int 
sharedDataValue::getConnection(const int& port)
 {
	 MceUtil::QuickMutex::ScopedLock lock(_mutexLinkCnt);
	 return _conCount[port];
 }



 std::string sharedDataValue::intToStr(int i)
 {
	 std::string s;
	 std::stringstream ss;
	 ss << i;
	 ss >> s;
	 return s;
 }




