#include <iostream>
#include <fstream>
#include <sstream>  
#include <log/ICASCOLog.h>
#include "csvFileParse.h"



groupCsvFile::groupCsvFile()
{
	char *envHome = getenv("SSIP_ROOT");
	if ((envHome == NULL) || (strlen(envHome) == 0))
	{
		printf("groupCsvFile::groupCsvFile() - �Ҳ�����������\n");
		ICASCOLog::error("transDataIom: groupCsvFile::groupCsvFile() - �Ҳ�����������"); 
		exit(-1);
	}
	memset(_envName, 0, sizeof(_envName));
	strncpy_s(_envName, envHome, sizeof(_envName) - 1);
	printf("groupCsvFile::groupCsvFile() - ��ʼ��ȡ���ý���\n");
	groupFile();
    printf("groupCsvFile::groupCsvFile() - ��ȡ���ý���\n");
	ICASCOLog::info("transDataIom: groupCsvFile::groupCsvFile() - ��ȡ���ý���"); 
};



groupCsvFile::~groupCsvFile()
{

};



void 
groupCsvFile::groupFile()
{
	if(!groupModbusRegBitDigital(DI_MODBUS_REG_BIT_03_FUNC))
	{
		printf("transDataIom: groupCsvFile::groupCsvFile() -  groupModbusRegBitDigital(DI_MODBUS_REG_BIT_03_FUNC) fail\n"); 
		ICASCOLog::error("transDataIom: groupCsvFile::groupCsvFile() -  groupModbusRegBitDigital(DI_MODBUS_REG_BIT_03_FUNC) fail"); 
	}
	if(!groupModbusRegBitDigital(DI_MODBUS_REG_BIT_04_FUNC))
	{
		printf("groupCsvFile::groupCsvFile() -  groupModbusRegBitDigital(DI_MODBUS_REG_BIT_04_FUNC) fail\n"); 
		ICASCOLog::error("transDataIom: groupCsvFile::groupCsvFile() -  groupModbusRegBitDigital(DI_MODBUS_REG_BIT_04_FUNC) fail"); 
	}

	if(!groupModbusRegBitDigital(DO_MODBUS_REG_BIT_03_FUNC))
	{
		printf("groupCsvFile::groupCsvFile() -  groupModbusRegBitDigital(DO_MODBUS_REG_BIT_03_FUNC) fail\n"); 
		ICASCOLog::error("transDataIom: groupCsvFile::groupCsvFile() -  groupModbusRegBitDigital(DO_MODBUS_REG_BIT_03_FUNC) fail"); 
	}

	if(!groupModbusBitDigital(DO_MODBUS_BIT))
	{
		printf("groupCsvFile::groupCsvFile() -  groupModbusBitDigital(DO_MODBUS_BIT) fail\n"); 
		ICASCOLog::error("transDataIom: groupCsvFile::groupCsvFile() -  groupModbusBitDigital(DO_MODBUS_BIT) fail"); 
	}

	if(!groupModbusBitDigital(DI_MODBUS_01_FUNC))
	{
		printf("transDataIom: groupCsvFile::groupCsvFile() -  groupModbusBitDigital(DI_MODBUS_01_FUNC) fail\n"); 
		ICASCOLog::error("transDataIom: groupCsvFile::groupCsvFile() - groupModbusBitDigital(DI_MODBUS_01_FUNC) fail"); 
	}

	if(!groupModbusBitDigital(DI_MODBUS_02_FUNC))
	{
		printf("transDataIom: groupCsvFile::groupCsvFile() -  groupModbusBitDigital(DI_MODBUS_02_FUNC) fail\n"); 
		ICASCOLog::error("transDataIom: groupCsvFile::groupCsvFile() - groupModbusBitDigital(DI_MODBUS_02_FUNC) fail"); 
	}

	if(!groupModbusAnalog(AI_MODBUS_03_FUNC))
	{
		printf("transDataIom: groupCsvFile::groupCsvFile() -  groupModbusAnalog(AI_MODBUS_03_FUNC) fail\n"); 
		ICASCOLog::error("transDataIom: groupCsvFile::groupCsvFile() - groupModbusAnalog(AI_MODBUS_03_FUNC) fail"); 
	}
	if(!groupModbusAnalog(AI_MODBUS_04_FUNC))
	{
		printf("transDataIom: groupCsvFile::groupCsvFile() -  groupModbusAnalog(AI_MODBUS_04_FUNC) fail\n"); 
		ICASCOLog::error("transDataIom: groupCsvFile::groupCsvFile() - groupModbusAnalog(AI_MODBUS_04_FUNC) fail"); 
	}
	if(!groupModbusAnalog(AO_MODBUS))
	{
		printf("transDataIom: groupCsvFile::groupCsvFile() -  groupModbusAnalog(AO_MODBUS) fail\n"); 
		ICASCOLog::error("transDataIom: groupCsvFile::groupCsvFile() - groupModbusAnalog(AO_MODBUS) fail"); 
	}

}




bool 
groupCsvFile::groupModbusAnalog(const CSV_TABLE_TYPE type)
{
	char fileName[256] = {0};
	if(type == AI_MODBUS_03_FUNC)
	{
		mce_snprintf(fileName, sizeof(fileName) -1, "%s/config/R_03.csv", _envName); 
	}
	else if(type == AI_MODBUS_04_FUNC)
	{
		mce_snprintf(fileName, sizeof(fileName) -1, "%s/config/R_04.csv", _envName); 
	}
	else if(type == AO_MODBUS)
	{
		mce_snprintf(fileName, sizeof(fileName) -1, "%s/config/W_03.csv", _envName); 
	}

	Mce::uint16 rowIndex = 1;
	Mce::uint16 lineIndex;
	strTbModbusAnalog objPoint;

	std::ifstream csvInput(fileName);
	std::string oneLine;
	std::vector<std::string> lineOfstr; // ����һ���ַ����������ڴ��ͼƬID�ʹ������  
	while (std::getline(csvInput, oneLine))  // ���ж�ȡ�����з���\n�����֣������ļ�β��־eof��ֹ��ȡ  
	{	
		lineIndex = 1;
		memset(&objPoint, 0, sizeof(objPoint));
		std::istringstream Readstr(oneLine); // �������ַ���oneLine���뵽�ַ�����istringstream��
		std::string partOfstr;
		if (oneLine == "trans_register,from_rtuno,from_pointno,rate,offset,deadzone,Type,Desc")   // ��һ�в���ȡ
		{
			rowIndex++;
			continue;
		}
		while(std::getline(Readstr, partOfstr, ',')) // ���ַ�����_Readstr�е��ַ����뵽_partOfstr�ַ����У��Զ���Ϊ�ָ���
		{
			if(lineIndex == 1)
			{
				if(isNum(partOfstr))
				{
					objPoint.transRegisterNo = atoi((char *)partOfstr.c_str());
				}
			}
			else if(lineIndex == 2)
			{
				if(isNum(partOfstr))
				{
					objPoint.sourceContent.sourcePointInfo.sourceRtuNo = atoi((char *)partOfstr.c_str());
				}
			}
			else if(lineIndex == 3)
			{
				if(isNum(partOfstr))  
				{
					objPoint.sourceContent.sourcePointInfo.sourcePointNo = atoi((char *)partOfstr.c_str());
				}
				else
				{
					std::string::size_type position = partOfstr.find("Rtu_");  //rtu״̬
					if (position != partOfstr.npos)  //���û�ҵ�������һ���ر�ı�־c++����npos��ʾ
					{
						objPoint.pointType = 1; //rtu status
						strcpy_s(objPoint.sourceContent.pointName, partOfstr.c_str());
						lineIndex++;
						continue;
					}

				    position = partOfstr.find("Chan");  //channel״̬
					if (position != partOfstr.npos)  
					{
						objPoint.pointType = 2; //channel status
						strcpy_s(objPoint.sourceContent.pointName, partOfstr.c_str());
						lineIndex++;
						continue;
					}
					if(partOfstr.at(0) == 'p' || partOfstr.at(0) == 'P')     
					{
						objPoint.pointType = 3; //logic calc point
						strcpy_s(objPoint.sourceContent.pointName, partOfstr.c_str());
						lineIndex++;
						continue;
					}
				}
			}
			else if(lineIndex == 4)
			{
				if(isNum(partOfstr))
				{
					objPoint.rate = atof((char *)partOfstr.c_str());
				}
			}
			else if(lineIndex == 5)
			{
				if(isNum(partOfstr))
				{
					objPoint.offset = atof((char *)partOfstr.c_str());
				}
			}
			else if(lineIndex == 6)
			{
				if(isNum(partOfstr))
				{
					objPoint.deadband = atof((char *)partOfstr.c_str());
				}
			}
			else if(lineIndex == 7)
			{
				if(!partOfstr.empty())
				{
					strcpy_s(objPoint.dataType,(char *)(partOfstr.c_str()));
				}
			}
			lineIndex++;
		}

		rowIndex++;
		if(type == AI_MODBUS_03_FUNC)
		{
			//Mce::uint16 transRegisterNo = objPoint.transRegisterNo;
			_aiTableModbus03.push_back(objPoint);
		}
		else if(type == AI_MODBUS_04_FUNC)
		{
			//Mce::uint16 transRegisterNo = objPoint.transRegisterNo;
			_aiTableModbus04.push_back(objPoint);
		}
		else if(type == AO_MODBUS)
		{
			//Mce::uint16 transRegisterNo = objPoint.transRegisterNo;
			_aoTableModbus04.push_back(objPoint);
		}

	}
	csvInput.close();
	return true;
}



bool 
groupCsvFile::groupModbusRegBitDigital(const CSV_TABLE_TYPE type)
{
	char fileName[256] = {0};
	if(type == DI_MODBUS_REG_BIT_03_FUNC)
	{
		mce_snprintf(fileName, sizeof(fileName) -1, "%s/config/R_03Bit.csv", _envName); 
	}
	else if(type == DI_MODBUS_REG_BIT_04_FUNC)
	{
		mce_snprintf(fileName, sizeof(fileName) -1, "%s/config/R_04Bit.csv", _envName); 
	}
	else if(type == DO_MODBUS_REG_BIT_03_FUNC)
	{
		mce_snprintf(fileName, sizeof(fileName) -1, "%s/config/W_03Bit.csv", _envName); 
	}

	Mce::uint32 rowIndex = 1;
	Mce::uint16 lineIndex;
	strTbModbusRegBit objPoint;
	std::vector<strTbModbusRegBit> mbList;

	std::ifstream csvInput(fileName);
	std::string oneLine;
	std::vector<std::string> lineOfstr; // ����һ���ַ����������ڴ��ͼƬID�ʹ������  
	while (std::getline(csvInput, oneLine))  // ���ж�ȡ�����з���\n�����֣������ļ�β��־eof��ֹ��ȡ  
	{	
		lineIndex = 1;
		memset(&objPoint, 0, sizeof(objPoint));
		std::istringstream Readstr(oneLine); // �������ַ���oneLine���뵽�ַ�����istringstream��
		std::string partOfstr;
		if (oneLine == "trans_register,register_bit,from_rtuno,from_pointno,revert,Desc")// ��һ�в���ȡ
		{
			rowIndex++;
			continue;
		}
		while(std::getline(Readstr, partOfstr, ',')) // ���ַ�����_Readstr�е��ַ����뵽_partOfstr�ַ����У��Զ���Ϊ�ָ���
		{
			if(lineIndex == 1)    // trans_register
			{
				if(isNum(partOfstr))
				{
					objPoint.transRegisterNo = atoi((char *)partOfstr.c_str());
				}
			}
			else if(lineIndex == 2)   //register_bit
			{
				if(isNum(partOfstr))
				{
					objPoint.transBitNo = atoi((char *)partOfstr.c_str());
				}
			}
			else if(lineIndex == 3)     //from_rtuno
			{
				if(isNum(partOfstr))
				{
					objPoint.sourceContent.sourcePointInfo.sourceRtuNo = atoi((char *)partOfstr.c_str());
				}

			}
			else if(lineIndex == 4)    //from_pointno
			{
				if(isNum(partOfstr))
				{
					objPoint.sourceContent.sourcePointInfo.sourcePointNo = atoi((char *)partOfstr.c_str());
				}
			}
			else if(lineIndex == 5)     //revert
			{
				if(isNum(partOfstr))
				{
					objPoint.revert = ((partOfstr == "1") ? true : false);
				}
			}
			lineIndex++;
		}	
		rowIndex++;

		

		if(type == DI_MODBUS_REG_BIT_03_FUNC)
		{
			_diTableModbusRegBit03.push_back(objPoint);

		}
		else if(type == DI_MODBUS_REG_BIT_04_FUNC)
		{
			_diTableModbusRegBit04.push_back(objPoint);

		}
		else if(type == DO_MODBUS_REG_BIT_03_FUNC)
		{
			_doTableModbusRegBit03.push_back(objPoint);
		}
	}
	csvInput.close();
	return true;
}








bool 
groupCsvFile::isNum(std::string str)  
{  
	std::stringstream sin(str);  
	double d;  
	char c;  
	if(!(sin >> d))  
		return false;  
	if (sin >> c)  
		return false;  
	return true;  
}  





bool 
groupCsvFile::groupModbusBitDigital(const CSV_TABLE_TYPE type)
{
	char fileName[256] = {0};

	if(type == DI_MODBUS_01_FUNC)
	{
		mce_snprintf(fileName, sizeof(fileName) -1, "%s/config/R_01.csv", _envName); 
	}
	else if(type == DI_MODBUS_02_FUNC)
	{
		mce_snprintf(fileName, sizeof(fileName) -1, "%s/config/R_02.csv", _envName); 
	}
	else if(type == DO_MODBUS_BIT)
	{
		mce_snprintf(fileName, sizeof(fileName) -1, "%s/config/W_01.csv", _envName); 
	}
	else
	{
		return false;
	}

	Mce::uint32 rowIndex = 1;
	Mce::uint16 lineIndex;
	strTbModbusDigital objPoint;
	std::vector<strTbModbusDigital> mbList;

	std::ifstream csvInput(fileName);
	std::string oneLine; 
	while (std::getline(csvInput, oneLine))  // ���ж�ȡ�����з���\n�����֣������ļ�β��־eof��ֹ��ȡ  
	{	
		lineIndex = 1;
		memset(&objPoint, 0, sizeof(objPoint));
		std::istringstream Readstr(oneLine); // �������ַ���oneLine���뵽�ַ�����istringstream��
		std::string partOfstr;
		if (oneLine == "trans_Bit,from_rtuno,from_pointno,revert,Desc")   // ��һ�в���ȡ
		{
			rowIndex++;
			continue;
		}
		while(std::getline(Readstr, partOfstr, ',')) // ���ַ�����_Readstr�е��ַ����뵽_partOfstr�ַ����У��Զ���Ϊ�ָ���
		{
			if(lineIndex == 1)
			{
				if(isNum(partOfstr))
				{
					objPoint.transBitNo = atoi((char *)partOfstr.c_str());
				}
			}
			else if(lineIndex == 2)
			{
				if(isNum(partOfstr))
				{
					objPoint.sourceContent.sourcePointInfo.sourceRtuNo = atoi((char *)partOfstr.c_str());
				}
			}
			else if(lineIndex == 3)
			{
				if(isNum(partOfstr) && objPoint.pointType == 0)
				{
					objPoint.sourceContent.sourcePointInfo.sourcePointNo = atoi((char *)partOfstr.c_str());
				}
			}
			else if(lineIndex == 5)
			{
				if(isNum(partOfstr))
				{
					objPoint.revert = atoi((char *)partOfstr.c_str());
				}
			}
			lineIndex++;
		}
		rowIndex++;

		if(type == DI_MODBUS_01_FUNC)
		{
			//Mce::uint16 bit_No = objPoint.transBitNo;
			_diTableModbus01.push_back (objPoint);
		}
		else if(type == DI_MODBUS_02_FUNC)
		{
			//Mce::uint16 bit_No = objPoint.transBitNo;
			_diTableModbus02.push_back (objPoint);
		}
		else if(type == DO_MODBUS_BIT)
		{
			//Mce::uint16 bit_No = objPoint.transBitNo;t;
			_doTableModbus01.push_back (objPoint);
		}
	}
	csvInput.close();
	return true;
}





namespace
{
	static MceUtil::SingletonHolder<groupCsvFile> __CSV_FILE;
}



groupCsvFile& 
groupCsvFile::getInstance()
{
	return *__CSV_FILE.get();
}
