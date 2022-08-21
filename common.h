#ifndef __COMMON_H_
#define __COMMON_H_


enum CSV_TABLE_TYPE
{
	DI_MODBUS_01_FUNC ,
	DI_MODBUS_02_FUNC ,
	DI_MODBUS_REG_BIT_03_FUNC,
	DI_MODBUS_REG_BIT_04_FUNC,
	AI_MODBUS_03_FUNC ,
	AI_MODBUS_04_FUNC,
	DO_MODBUS_BIT,
	DO_MODBUS_REG_BIT_03_FUNC,
	AO_MODBUS
};



enum REG_ENDIAN_MODE
{
	LITTLE_ENDIAN =  1,
	BID_ENDIN = 2
};



enum ENDIAN_MODE
{
	ABCD =  1,
	CDAB =  2,
	BADC =  3,
	DCBA =  4
};



enum EXECEPTION_CODE
{
	VALID = 0,
	INVALID_FUNCCODE = 1,
	INVALID_ADDR = 2,
	INVALID_DATA = 3
};



enum FUNC_CODE
{
	READ_COIL = 0x01,
	READ_INPUT_DISC = 0x02,
	READ_HOLDING_REG = 0x03,
	READ_INPUT_REG = 0x04,
	WRITE_SINGLE_COIL = 0x05,
	WRITE_SINGLE_REG = 0x06,
	WRITE_MULTI_COIL = 0x0f,
	WRITE_MULTI_REG = 0x10
};



struct strTbModbusDigital
{
	Mce::uint8   pointType;  //yx type 0; calc tpe 3
	Mce::uint16 transBitNo;
	union tag_sourceContent
	{
		char  pointName[16];
		struct tag_sourcePointInfo
		{
			Mce::uint16  sourceRtuNo;
			Mce::uint32  sourcePointNo;
		}sourcePointInfo;
	}sourceContent;
	bool        revert;    
};


// CSV trans modbus reg bit table struct define
struct strTbModbusRegBit
{
	Mce::uint8   pointType;  //yx type 0; calc tpe 3
	Mce::uint32 transRegisterNo;
	Mce::uint16 transBitNo;
	union tag_sourceContent
	{
		char  pointName[16];
		struct tag_sourcePointInfo
		{
			Mce::uint16  sourceRtuNo;
			Mce::uint32  sourcePointNo;
		}sourcePointInfo;
	}sourceContent;
	bool        revert;     
};



// CSV trans modbus analog table struct define
struct strTbModbusAnalog
{
	Mce::uint8   pointType; //yc point == 0  rtu status == 1 channel status == 2, or logic calc point == 3
	Mce::uint32  transRegisterNo;
	union tag_sourceContent
	{
		char  pointName[16];
		struct tag_sourcePointInfo
		{
			Mce::uint16  sourceRtuNo;
			Mce::uint32  sourcePointNo;
		}sourcePointInfo;
	}sourceContent;
	Mce::float32 rate;
	Mce::float32 offset;
	Mce::float32 deadband;
	char		 dataType[12];
};




//Modbus  read  Struct
struct modbusRead
{
	Mce::uint16 affairNo;
	Mce::uint16 protoId;
	Mce::uint8  unitId;
	Mce::uint8  funcCode;
	Mce::uint32 begAddr;
	Mce::uint32 num;
};



//Modbus  Reg control struct
struct modbusWrite
{
	Mce::uint16 affairNo;
	Mce::uint16 protoId;
	Mce::uint8  unitId;
	Mce::uint8  funcCode;
	Mce::uint32 begAddr;
	Mce::uint32 num;
	//Mce::uint16	retCode;
	//Mce::uint8  regVal[256];   //modbus protocol: 1 frame can write 123 reg at most
} ;


//Modbus  coil control struct
struct AE_MB_DISC_CTL
{
	Mce::uint16 affairNo;
	Mce::uint16 protoId;
	Mce::uint8  unitId;
	Mce::uint8  funcCode;
	Mce::uint32 begAddr;
	Mce::uint32 num;
	Mce::uint16	retCode;
	bool        coilVal[1968]; //modbus protocol: 1 frame can write 1968 disc at most
} ;


//rtu info
struct strRtuInfo
{
	Mce::uint16 rtuNo;
	Mce::uint8  addr;
	Mce::uint32 port;
	char		Protocol[64]; 
	char		libName[64];
	int     	needOffset;   //only for modbus
	char		endianMode[16];	      //only for modbus
};



// CSV trans 104Digital table struct define
struct strTb104Digital
{
	Mce::uint8   pointType;  //yx type 0; calc tpe 1
	Mce::uint16 transRtuNo;
	Mce::uint32 transPointNo;
	union tag_sourceContent
	{
		char  pointName[16];
		struct tag_sourcePointInfo
		{
			Mce::uint16  sourceRtuNo;
			Mce::uint32  sourcePointNo;
		}sourcePointInfo;
	}sourceContent;
	bool        revert;     
};


// CSV trans 104Analog table struct define
struct strTb104Analog
{
	Mce::uint8   pointType;  //yc point == 0  rtu status == 1 channel status == 2, or logic calc point == 3
	Mce::uint16  transRtuNo;
	Mce::uint32  transPointNo;
	union  tag_sourceContent
	{
		char  pointName[16];
		struct tag_sourcePointInfo
		{
			Mce::uint16  sourceRtuNo;
			Mce::uint32  sourcePointNo;
		}sourcePointInfo;
	}sourceContent;
	Mce::float32 rate;
	Mce::float32 offset;
	Mce::float32 deadband;
};

#endif // __COMMON_H_

//test2