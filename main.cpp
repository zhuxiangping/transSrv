#include <mce/Application.h>
#include <mce/Thread.h>
#include <aefc/processmgr.h>
#include <daq/daqrawdriver.h>
#include <log/ICASCOLog.h>
#include "dataCheck.h"
#include "dataTrMngr.h"



#define CSI_K_OBJECT        "transAllData"
#define CSI_K_OBJECT_DESC   "全数据转发"
#define CSI_K_OBJECT_USER   "transAllData"


//#ifdef WIN32
//#pragma comment(linker, "/subsystem:windows /entry:mainCRTStartup")
//#endif


MSG_ID_T 
monitorParent()
{
	CProcessManager	ProcSrv;
	ProcSrv.StartProcess(CSI_K_OBJECT, CSI_K_OBJECT_DESC, CSI_K_OBJECT_USER, NULL);	
	while (1)
	{		
		if (!ProcSrv.ISeeFather())
		{
			break;		    		
		}	
		Mce_os::sleep(1000);
	}  
	ProcSrv.EndProcess();    		
	return 0;
}




int main(int argc, char *argv[])
{
#ifdef A_OS_WIN
	HINSTANCE handle = LoadLibrary("blackbox.dll");
	
#endif
	char *szHome = getenv("SSIP_ROOT");
	if ((szHome == NULL) || (strlen(szHome) == 0))
	{
		printf("请设置SSIP_ROOT环境变量\n");
		ICASCOLog::info("transDataIom:main:stop()----stop is completed");

		return -1;
	}

	MceUtil::Application& app = MceUtil::Application::getInstance();
	app.init("transDataIom_mutex");
	if (app.isAlreadyExists())
	{
		return 0;
	}

	clsDataTrans dt;
	dt.start();
	monitorParent();

	dt.stop();

#ifdef A_OS_WIN
	if (handle)
	{
		BOOL bRet = FreeLibrary(handle);
	}
#endif
	return 0;
}
