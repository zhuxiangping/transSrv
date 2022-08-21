#ifndef _DATA_TR_MMGR_H___ 
#define _DATA_TR_MMGR_H___ 


#include <mce/TCPServer.h> 
#include "common.h"


//data 1



class clsDataTrans
{
public:
	A_DISABLE_COPY(clsDataTrans)
	clsDataTrans();
	~clsDataTrans();

public:
	void start();
	void stop();

	

private:
	bool                                    _runningFlag;
	Mce_net::TCPServer*                     _TcpServer[16];
};




#endif  //_DATA_TR_MMGR_H___
