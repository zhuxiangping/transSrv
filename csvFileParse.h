#ifndef _READ_CSV_FILE_TABLE_H
#define _READ_CSV_FILE_TABLE_H 


#include <mce/SingletonHolder.h>
#include "common.h"


class  groupCsvFile
{
private:
	friend class MceUtil::SingletonHolder<groupCsvFile>;
	groupCsvFile();
	~groupCsvFile();

public:
	static groupCsvFile& getInstance();

	std::vector<strTbModbusAnalog>& aiTableModbus03()    //ai 03 func
	{
		return _aiTableModbus03;
	}

	std::vector<strTbModbusAnalog>& aiTableModbus04()    //ai 04 func
	{
		return _aiTableModbus04;
	}


	std::vector<strTbModbusAnalog>& aoTableModbus()       //ao 06 or 16 func
	{
		return  _aoTableModbus04;
	}

	std::vector<strTbModbusDigital>& diTableModbus01()     //di 01 func   
	{
		return _diTableModbus01;

	}

	std::vector<strTbModbusDigital>&  diTableModbus02()    //di 02 func      ;
	{
		return _diTableModbus02;
	}

	std::vector<strTbModbusRegBit>& diTableModbusRegBit03() // di 03 func bit
	{
		return _diTableModbusRegBit03;
	}

	std::vector<strTbModbusRegBit>& diTableModbusRegBit04()  // di 04 func bit
	{
		return _diTableModbusRegBit04;
	}

	std::vector<strTbModbusRegBit>& doTableModbusRegBit03()  // d0 03 func bit
	{
		return _doTableModbusRegBit03;
	}


	void groupFile();

private:
	bool groupModbusAnalog(const CSV_TABLE_TYPE type);
	bool groupModbusRegBitDigital(const CSV_TABLE_TYPE type);
	bool groupModbusBitDigital(const CSV_TABLE_TYPE type);

	bool isNum(std::string str);

public:

	std::vector<strTbModbusAnalog>                        _aiTableModbus03;
	std::vector<strTbModbusAnalog>                        _aiTableModbus04;
	std::vector<strTbModbusAnalog>                        _aoTableModbus04;

	std::vector<strTbModbusDigital>                       _diTableModbus01;
	std::vector<strTbModbusDigital>                       _diTableModbus02;
	std::vector<strTbModbusDigital>                       _doTableModbus01;

	std::vector<strTbModbusRegBit>                        _diTableModbusRegBit03;
	std::vector<strTbModbusRegBit>                        _diTableModbusRegBit04;
	std::vector<strTbModbusRegBit>                        _doTableModbusRegBit03;

	char    _envName[128];
};



#endif  //_READ_CSV_FILE_TABLE_H