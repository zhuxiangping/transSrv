#ifndef _SHARE_DATA_H___ 
#define _SHARE_DATA_H___ 

#include <mce/Mutex.h>
#include "common.h"


class sharedDataValue
{

public:
	A_DISABLE_COPY(sharedDataValue)
	sharedDataValue();
	~sharedDataValue();

	static sharedDataValue*  _pInstance;
	void setAnalogData(const int&index, const std::string& dataType,  Mce::float32& val, const int& funcCode);
	void setDigitalData(const int&index,  Mce::uint8& val, const int& funcCode);

	Mce::uint8 getDigitalValue(const int&index, const int& funcCode);
	Mce::uint16 getAnaValue(const int&index, const int& funcCode);

	int dataToBuf(Mce::uint16* buf, const std::string& dataType, ENDIAN_MODE& mode, Mce::float32& data);

	const int boolMax() const
	{
		return kBoolMax;
	}

	const int int16Max() const
	{
		return kInt16Max;
	}

	void addConnection(const int& port);
	void deConnection(const int& port);
	int getConnection(const int& port);
	int getPortNum() const {return _num;}
	int getPortByIndex(const int& index) const {return _port[index];}
	int getOffset() const {return _offset;}
	int getMaxLink() const {return _maxLink;}

private:
	std::string intToStr(int i);

private:
	static const int                           kBoolMax = 65535;
	static const int                           kInt16Max = 65535;
	Mce::uint8                                 _digital01[kBoolMax];
	Mce::uint8                                 _digital02[kBoolMax];
	Mce::uint16                                _analog03[kInt16Max];
	Mce::uint16                                _analog04[kInt16Max];
	MceUtil::QuickMutex                        _mutex_01;
	MceUtil::QuickMutex                        _mutex_02;
	MceUtil::QuickMutex                        _mutex_03;
	MceUtil::QuickMutex                        _mutex_04;
	MceUtil::QuickMutex                        _mutexLinkCnt;

	int                                        _num;
	int                                        _port[16];
	int                                        _offset;
	int                                        _maxLink;
	ENDIAN_MODE                                _mode;

	std::map<int, int>                         _conCount;
};


#endif  //_SHARE_DATA_H___
