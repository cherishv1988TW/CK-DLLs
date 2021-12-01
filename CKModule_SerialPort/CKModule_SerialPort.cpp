//---------------------------------------------------------------------------
#pragma hdrstop
//for VC++
//===========================================================================

// #include "stdafx.h"
// #pragma comment(lib, "Version.lib")

//===========================================================================
#include "CKModule_SerialPort.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

//--------------------------------Separation---------------------------------
CKModule_SerialPort::CKModule_SerialPort(void* pCallbackObject, CKMODULE_SERIALPORT_ONNOTIFY pCallback_OnNotify)
{
    hCKSerialPortDLLModule = NULL;
	pCKSerialPort = NULL;
	pDLLVersion = NULL;
	char cExeFilePath[MAX_PATH] = {'\0'};
	unsigned long iResult = ::GetModuleFileName(NULL, cExeFilePath, sizeof(cExeFilePath)); //Retrieve the path of the executable file of the current process
	if(iResult != 0 && ::GetLastError() != ERROR_INSUFFICIENT_BUFFER) //The buffer used to save full path is insufficient 
	{
		char* pLastChar = ::strrchr(cExeFilePath, '\\');
		char cDLLFileName[MAX_PATH] = {'\0'};
		int iLength = pLastChar - cExeFilePath + 1;
		//for(int i = 0; i < iLength; i++)
		//	cDLLFileName[i] = cExeFilePath[i];
		::strcat(cDLLFileName, "CKSerialPortDLLModule.dll");
		hCKSerialPortDLLModule = ::LoadLibrary(cDLLFileName);
		if(hCKSerialPortDLLModule != NULL)
		{
			//Get DLL version
			//===================================================================
			char cDLLRevisionNumber[64] = { '\0' };
			DWORD iFileVersionInfoSize = ::GetFileVersionInfoSize(cDLLFileName, NULL);
			unsigned char* pFileVersionInfoBuffer = new unsigned char[iFileVersionInfoSize];
			bool bResult = ::GetFileVersionInfo(cDLLFileName, 0, iFileVersionInfoSize, pFileVersionInfoBuffer);
			if (bResult)
			{
				VS_FIXEDFILEINFO* pFixedFileInfo;
				UINT iFixedFileInfoSize = 0;
				bResult = ::VerQueryValue(pFileVersionInfoBuffer, "\\", (void**)&pFixedFileInfo, &iFixedFileInfoSize);
				if (bResult)
				{
					::sprintf(cDLLRevisionNumber, "%d.%d.%d.%d", ((pFixedFileInfo->dwFileVersionMS >> 16) & 0xffff), ((pFixedFileInfo->dwFileVersionMS >> 0) & 0xffff)
						, ((pFixedFileInfo->dwFileVersionLS >> 16) & 0xffff), ((pFixedFileInfo->dwFileVersionLS >> 0) & 0xffff));
					pDLLVersion = new char[::strlen(cDLLRevisionNumber) + 1];
					::strcpy(pDLLVersion, cDLLRevisionNumber);
				}
			}
			delete[] pFileVersionInfoBuffer;
			//===================================================================
			
			CKSerialPort_Create = (CKSerialPortDLLModule_Create)::GetProcAddress(hCKSerialPortDLLModule, "CKSerialPortDLLModule_Create");
			CKSerialPort_Close = (CKSerialPortDLLModule_Close)::GetProcAddress(hCKSerialPortDLLModule, "CKSerialPortDLLModule_Close");
			CKSerialPort_OpenPort = (CKSerialPortDLLModule_OpenPort)::GetProcAddress(hCKSerialPortDLLModule, "CKSerialPortDLLModule_OpenPort");
			CKSerialPort_ClosePort = (CKSerialPortDLLModule_ClosePort)::GetProcAddress(hCKSerialPortDLLModule, "CKSerialPortDLLModule_ClosePort");
			CKSerialPort_SendData = (CKSerialPortDLLModule_SendData)::GetProcAddress(hCKSerialPortDLLModule, "CKSerialPortDLLModule_SendData");
			CKSerialPort_GetDataSize = (CKSerialPortDLLModule_GetDataSize)::GetProcAddress(hCKSerialPortDLLModule, "CKSerialPortDLLModule_GetDataSize");
			CKSerialPort_GetData = (CKSerialPortDLLModule_GetData)::GetProcAddress(hCKSerialPortDLLModule, "CKSerialPortDLLModule_GetData");
			pCKSerialPort = CKSerialPort_Create((pCallbackObject == NULL ? this : pCallbackObject), pCallback_OnNotify);
		}
		else
		{
			DWORD iResult = ::GetLastError();
			if(iResult == 193)
				::MessageBox(NULL, "CKSerialPortDLLModule Is Not Compatible To Platform", "Error", MB_ICONERROR | MB_SYSTEMMODAL);
			else
				::MessageBox(NULL, "Failed to Load CKSerialPortDLLModule", "Error", MB_ICONERROR | MB_SYSTEMMODAL);
		}
	}
	else
		::MessageBox(NULL, "CKSerialPortDLLModule Not Found", "Error", MB_ICONERROR | MB_SYSTEMMODAL);
}
//---------------------------------------------------------------------------
CKModule_SerialPort::~CKModule_SerialPort()
{
    if(pCKSerialPort != NULL)
    {
        CKSerialPort_Close(pCKSerialPort);
        pCKSerialPort = NULL;
    }
    if(hCKSerialPortDLLModule != NULL)
    {
        ::FreeLibrary(hCKSerialPortDLLModule);
        hCKSerialPortDLLModule = NULL;
    }
	if (pDLLVersion != NULL)
	{
		delete[] pDLLVersion;
		pDLLVersion = NULL;
	}
}
//---------------------------------------------------------------------------
bool CKModule_SerialPort::OpenPort(char* pPortName, unsigned long iBaudRate, unsigned char iDataBits, unsigned char iParity, unsigned char iStopBits)
{
    if(hCKSerialPortDLLModule == NULL) return false;
    return CKSerialPort_OpenPort(pCKSerialPort, pPortName, iBaudRate, iDataBits, iParity, iStopBits);
}
//---------------------------------------------------------------------------
bool CKModule_SerialPort::ClosePort(void)
{
    if(hCKSerialPortDLLModule == NULL) return false;
    return CKSerialPort_ClosePort(pCKSerialPort);
}
//---------------------------------------------------------------------------
bool CKModule_SerialPort::SendData(unsigned char* pData, unsigned long iDataSize, bool bSynchronized)
{
    if(hCKSerialPortDLLModule == NULL) return false;
    return CKSerialPort_SendData(pCKSerialPort, pData, iDataSize, bSynchronized);
}
//---------------------------------------------------------------------------
unsigned long CKModule_SerialPort::GetDataSize(void)
{
    if(hCKSerialPortDLLModule == NULL) return 0;
    return CKSerialPort_GetDataSize(pCKSerialPort);
}
//---------------------------------------------------------------------------
unsigned long CKModule_SerialPort::GetData(unsigned char* pData, unsigned long iDataSize)
{
    if(hCKSerialPortDLLModule == NULL) return 0;
    return CKSerialPort_GetData(pCKSerialPort, pData, iDataSize);
}
//---------------------------------------------------------------------------








