#include "sdk_demo_pro.h"
#include <QtWidgets/QApplication>
#include <windows.h>
#include <qmessagebox.h>
HANDLE m_hMutex = NULL;
bool checkOnly()
{
 	 m_hMutex = CreateMutex(NULL, FALSE, L"sn3d_sdk_demo");
 	if (GetLastError() == ERROR_ALREADY_EXISTS)  {
 		CloseHandle(m_hMutex);
		m_hMutex = NULL;
 		return  false;
	}

	return true;
}
//#include <vld.h>

int main(int argc, char *argv[])
{
	QApplication::addLibraryPath("./plugins");
	/*VLDDisable();*/
	QApplication a(argc, argv);

	if (!checkOnly())
	{
		QMessageBox::information(NULL, "information", "Software is unable to repeat opening while running.", "OK");
		return 0;
	}
	sdk_demo_pro w;
	w.show();
	int ret= a.exec();
	if (m_hMutex != NULL)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
	return ret;
}
