//---------------------------------------------------------------------------
#pragma hdrstop
//for VC++
//===========================================================================
/*
#include "stdafx.h"
#pragma comment(lib, "Version.lib")
*/
//===========================================================================
#include "CKModule_LogFile.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

CKModule_LogFile::CKModule_LogFile(char* pDirectoryName, bool bAutoDelete, unsigned int iReservedDays)
{
    hCKLogFileDLLModule = NULL;
	pCKLogFile = NULL;
	pDLLVersion = NULL;
	char cExeFilePath[MAX_PATH] = { '\0' };
	unsigned long iResult = ::GetModuleFileName(NULL, cExeFilePath, sizeof(cExeFilePath)); //Retrieve the path of the executable file of the current process
	if (iResult != 0 && ::GetLastError() != ERROR_INSUFFICIENT_BUFFER) //The buffer used to save full path is insufficient 
	{
		char* pLastChar = ::strrchr(cExeFilePath, '\\');
		char cDLLFileName[MAX_PATH] = { '\0' };
		int iLength = pLastChar - cExeFilePath + 1;
		//for (int i = 0; i < iLength; i++)
		//	cDLLFileName[i] = cExeFilePath[i];
		::strcat(cDLLFileName, "CKLogFileDLLModule.dll");
		hCKLogFileDLLModule = ::LoadLibrary(cDLLFileName);
		if(hCKLogFileDLLModule != NULL)
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
			
			CKLogFile_Open = (CKLogFileDLLModule_LogFile_Open)::GetProcAddress(hCKLogFileDLLModule, "CKLogFileDLLModule_LogFile_Open");
			CKLogFile_Close = (CKLogFileDLLModule_LogFile_Close)::GetProcAddress(hCKLogFileDLLModule, "CKLogFileDLLModule_LogFile_Close");
			CKLogFile_LogData = (CKLogFileDLLModule_LogFile_LogData)::GetProcAddress(hCKLogFileDLLModule, "CKLogFileDLLModule_LogFile_LogData");
			CKLogFile_DeleteLogFile = (CKLogFileDLLModule_LogFile_DeleteLogFile)::GetProcAddress(hCKLogFileDLLModule, "CKLogFileDLLModule_LogFile_DeleteLogFile");
			CKLogFile_SetLogFileMaxSize = (CKLogFileDLLModule_LogFile_SetLogFileMaxSize)::GetProcAddress(hCKLogFileDLLModule, "CKLogFileDLLModule_LogFile_SetLogFileMaxSize");

			pCKLogFile = CKLogFile_Open(pDirectoryName, bAutoDelete, iReservedDays);
		}
		else
		{
			DWORD iResult = ::GetLastError();
			if (iResult == 193)
				::MessageBox(NULL, "CKLogFileDLLModule Is Not Compatible To Platform", "Error", MB_ICONERROR | MB_SYSTEMMODAL);
			else
				::MessageBox(NULL, "Failed to Load CKLogFileDLLModule", "Error", MB_ICONERROR | MB_SYSTEMMODAL);
		}
	}
	else
		::MessageBox(NULL, "CKLogFileDLLModule Not Found", "Error", MB_ICONERROR | MB_SYSTEMMODAL);
}
//---------------------------------------------------------------------------
CKModule_LogFile::~CKModule_LogFile()
{
    if(pCKLogFile != NULL)
    {
        pCKLogFile = CKLogFile_Close(pCKLogFile);
        pCKLogFile = NULL;
    }
    if(hCKLogFileDLLModule != NULL)
    {
        ::FreeLibrary(hCKLogFileDLLModule);
        hCKLogFileDLLModule = NULL;
    }
    if(pDLLVersion != NULL)
	{
        delete [] pDLLVersion;
		pDLLVersion = NULL;
	}
}
//---------------------------------------------------------------------------
bool CKModule_LogFile::LogData(char* pData)
{
    if(hCKLogFileDLLModule == NULL) return false;
    return CKLogFile_LogData(pCKLogFile, pData);
}
//---------------------------------------------------------------------------
bool CKModule_LogFile::DeleteLogFile(char* pDirectoryPath, unsigned int iReservedDays)
{
    if(hCKLogFileDLLModule == NULL) return false;
    return CKLogFile_DeleteLogFile(pCKLogFile, pDirectoryPath, iReservedDays);
}
//---------------------------------------------------------------------------
bool CKModule_LogFile::SetLogFileMaxSize(unsigned int iKByte)
{
    if(hCKLogFileDLLModule == NULL) return false;
    return CKLogFile_SetLogFileMaxSize(pCKLogFile, iKByte);
}
//---------------------------------------------------------------------------

