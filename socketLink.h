#ifndef  __SOCKET_LINK_H
#define	 __SOCKET_LINK_H

#include <mce/TCPServerConnection.h>
#include "common.h"

//link1

struct STUTIME
{
	time_t   timeSecond;
	Mce::ushort   timeMsec;
};



class clsModbusLink : public Mce_net::TCPServerConnection
{
	A_DISABLE_COPY(clsModbusLink)

public:	
	explicit clsModbusLink(const Mce_net::StreamSocket& s);
	~clsModbusLink();

private:
	//void clear();
	int  onMessage();
	EXECEPTION_CODE frameValidCheck(Mce::uint8* buf, const int& len);
	void processFrame(Mce::uint8* buf, const int& len);
	void processException(Mce::uint8* buf, const int& len, EXECEPTION_CODE& execeptCode);

	bool isAddrOk(Mce::uint8* buf, const int& len);
	bool isFuncCodeOk(const int& funcCode);
	bool isDataOk(Mce::uint8* buf, const int& len);
	time_t   getNowSecond();


private:
	static const int kLength = 1500;
	void run();
	Mce_net::StreamSocket      _ss;

	modbusRead                 _read;                     
	modbusWrite                _write;    
	int                        _port;
	int                        _maxLink;

	time_t                     _lastTime;
	time_t                     _establishTime;
};


#endif   //__SOCKET_LINK_H
