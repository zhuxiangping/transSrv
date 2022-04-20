#include <log/ICASCOLog.h>
#include "dataTrMngr.h"
#include "socketlink.h"
#include "dataCheck.h"
#include "shareData.h"



clsDataTrans::clsDataTrans():_runningFlag(false)
{

}


clsDataTrans::~clsDataTrans()
{
	
}



void 
clsDataTrans::start()
{
	_runningFlag = true;
	const int& portNum = sharedDataValue::_pInstance->getPortNum();

	Mce_net::TCPServerConnectionFactory::Ptr pFactory = NULL;
	pFactory = new Mce_net::TCPServerConnectionFactoryImpl<clsModbusLink>();
	for(int i = 0; i < portNum; i++)
	{
		if(NULL != pFactory)
		{
			const int& port = sharedDataValue::_pInstance->getPortByIndex(i);
			_TcpServer[i] =  new Mce_net::TCPServer(pFactory, port);
			if(NULL != _TcpServer[i])
			{
				_TcpServer[i]->start();
			}
		}
	}

	clsDataCheck& dc = clsDataCheck::getInstance();
	dc.start();
}


void 
clsDataTrans::stop()
{
	_runningFlag = false;
	
	const int& portNum = sharedDataValue::_pInstance->getPortNum();
	
	for(int i = 0; i < portNum; i++)
	{
		_TcpServer[i]->stop();
	}
	
	clsDataCheck& dc = clsDataCheck::getInstance();
	dc.stop();
}





