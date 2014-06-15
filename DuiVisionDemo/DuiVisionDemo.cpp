// DuiVisionDemo.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "DuiVisionDemo.h"
#include "DuiHandlerMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDuiVisionDemoApp

BEGIN_MESSAGE_MAP(CDuiVisionDemoApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDuiVisionDemoApp 构造

CDuiVisionDemoApp::CDuiVisionDemoApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CDuiVisionDemoApp 对象

CDuiVisionDemoApp theApp;


// CDuiVisionDemoApp 初始化

BOOL CDuiVisionDemoApp::InitInstance()
{
	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("Blueant"));

	// 初始化DuiVision界面库,可以指定语言,dwLangID为0表示自动判断当前语言
	// 1116是应用程序ID，每个DUI应用程序应该使用不同的ID
	DWORD dwLangID = 0;
	new DuiSystem(m_hInstance, dwLangID, _T(""), 1116, IDD_DUIVISIONDEMO_DIALOG, "");

	// 检查是否已经有进程在运行
	CString strAppMutex = DuiSystem::Instance()->GetConfig("appMutex");	// 从配置文件中获取互斥量名字
	if(!strAppMutex.IsEmpty())
	{
		::CreateMutex(NULL,TRUE, _T("Global\\") + strAppMutex);
		if(ERROR_ALREADY_EXISTS == GetLastError() || ERROR_ACCESS_DENIED == GetLastError())
		{
			// 读取命令行参数,如果不需要处理命令行可以直接退出
			CString strCmd = L"";
			if(__argc > 0)
			{
				strCmd = __targv[0];
				DuiSystem::LogEvent(LOG_LEVEL_DEBUG, L"Command line:%s", strCmd);
			}

			// 发送进程间消息(lParam为1表示不显示界面,信息参数传递命令行参数)
			DuiSystem::Instance()->SendInterprocessMessage(0, DuiSystem::Instance()->GetAppID(), 1, strCmd);

			return FALSE; // Here we quit this application
		}
	}

	// 创建主窗口
	CDlgBase* pMainDlg = DuiSystem::CreateDuiDialog(_T("dlg_main"), NULL, _T(""), TRUE);
	// 给主窗口注册事件处理对象
	CDuiHandlerMain* pHandler = new CDuiHandlerMain();
	pHandler->SetDialog(pMainDlg);
	DuiSystem::RegisterHandler(pMainDlg, pHandler);

	// 初始化提示信息窗口
	DuiSystem::Instance()->CreateNotifyMsgBox(_T("dlg_notifymsg"));

	// 按照非模式对话框创建主窗口,并默认隐藏
	pMainDlg->Create(pMainDlg->GetIDTemplate(), NULL);
	//pMainDlg->ShowWindow(SW_HIDE);
	INT_PTR nResponse = pMainDlg->RunModalLoop();

	// 如果是按照模式对话框运行主窗口,只要改为如下代码就可以
	//INT_PTR nResponse = pMainDlg->DoModal();

	// 释放DuiVision界面库的资源
	DuiSystem::Release();

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
