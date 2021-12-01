//---------------------------------------------------------------------------
#ifndef CKMODULE_LOGFILE_H
#define CKMODULE_LOGFILE_H
#include <windows.h>
#include <stdio.h>
//---------------------------------------------------------------------------

typedef void* (*CKLogFileDLLModule_LogFile_Open)(char* pDirectoryName, bool bAutoDelete, unsigned int iReservedDays);
typedef void* (*CKLogFileDLLModule_LogFile_Close)(void* pDLLModule);
typedef bool (*CKLogFileDLLModule_LogFile_LogData)(void* pDLLModule, char* pData);
typedef bool (*CKLogFileDLLModule_LogFile_DeleteLogFile)(void* pDLLModule, char* pDirectoryPath, unsigned int iReservedDays);
typedef bool (*CKLogFileDLLModule_LogFile_SetLogFileMaxSize)(void* pDLLModule, unsigned int iKByte);

//---------------------------------------------------------------------------
class CKModule_LogFile
{
private:
protected:
    CKLogFileDLLModule_LogFile_Open CKLogFile_Open;
    CKLogFileDLLModule_LogFile_Close CKLogFile_Close;
    CKLogFileDLLModule_LogFile_LogData CKLogFile_LogData;
	CKLogFileDLLModule_LogFile_DeleteLogFile CKLogFile_DeleteLogFile;
    CKLogFileDLLModule_LogFile_SetLogFileMaxSize CKLogFile_SetLogFileMaxSize;
    HINSTANCE hCKLogFileDLLModule;
    void* pCKLogFile;
    char* pDLLVersion;

public:
    CKModule_LogFile(char* pDirectoryName, bool bAutoDelete, unsigned int iReservedDays);
	~CKModule_LogFile();
	bool LogData(char* pData);
	bool DeleteLogFile(char* pDirectoryPath, unsigned int iReservedDays);
	bool SetLogFileMaxSize(unsigned int iKByte);
    char* GetVersion(void) { return pDLLVersion; };
};
//---------------------------------------------------------------------------
#endif

