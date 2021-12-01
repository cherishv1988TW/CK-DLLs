//---------------------------------------------------------------------------
#pragma hdrstop
//for VC++
//===========================================================================

// #include "stdafx.h"
// #pragma comment(lib, "Version.lib")

//===========================================================================
#include "CKModule_IniFile.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

//--------------------------------Separation---------------------------------
CKModule_IniFile::CKModule_IniFile(const char* pFileName)
{
    hCKIniFileDLLModule = NULL;
	pCKIniFile = NULL;
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
		::strcat(cDLLFileName, "CKIniFileDLLModule.dll");
		hCKIniFileDLLModule = ::LoadLibrary(cDLLFileName);
		if(hCKIniFileDLLModule != NULL)
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
			
			CKIniFile_Open = (CKIniFileDLLModule_Open)::GetProcAddress(hCKIniFileDLLModule, "CKIniFileDLLModule_Open");
			CKIniFile_Close = (CKIniFileDLLModule_Close)::GetProcAddress(hCKIniFileDLLModule, "CKIniFileDLLModule_Close");
			CKIniFile_SaveFile = (CKIniFileDLLModule_SaveFile)::GetProcAddress(hCKIniFileDLLModule, "CKIniFileDLLModule_SaveFile");
			CKIniFile_SaveFileAs = (CKIniFileDLLModule_SaveFileAs)::GetProcAddress(hCKIniFileDLLModule, "CKIniFileDLLModule_SaveFileAs");
			CKIniFile_ReadInteger = (CKIniFileDLLModule_ReadInteger)::GetProcAddress(hCKIniFileDLLModule, "CKIniFileDLLModule_ReadInteger");
			CKIniFile_ReadDouble = (CKIniFileDLLModule_ReadDouble)::GetProcAddress(hCKIniFileDLLModule, "CKIniFileDLLModule_ReadDouble");
			CKIniFile_ReadBoolean = (CKIniFileDLLModule_ReadBoolean)::GetProcAddress(hCKIniFileDLLModule, "CKIniFileDLLModule_ReadBoolean");
			CKIniFile_ReadString = (CKIniFileDLLModule_ReadString)::GetProcAddress(hCKIniFileDLLModule, "CKIniFileDLLModule_ReadString");
			CKIniFile_WriteInteger = (CKIniFileDLLModule_WriteInteger)::GetProcAddress(hCKIniFileDLLModule, "CKIniFileDLLModule_WriteInteger");
			CKIniFile_WriteDouble = (CKIniFileDLLModule_WriteDouble)::GetProcAddress(hCKIniFileDLLModule, "CKIniFileDLLModule_WriteDouble");
			CKIniFile_WriteBoolean = (CKIniFileDLLModule_WriteBoolean)::GetProcAddress(hCKIniFileDLLModule, "CKIniFileDLLModule_WriteBoolean");
			CKIniFile_WriteString = (CKIniFileDLLModule_WriteString)::GetProcAddress(hCKIniFileDLLModule, "CKIniFileDLLModule_WriteString");
			
			pCKIniFile = CKIniFile_Open(pFileName);
		}
		else
		{
			DWORD iResult = ::GetLastError();
			if(iResult == 193)
				::MessageBox(NULL, "CKIniFileDLLModule Is Not Compatible To Platform", "Error", MB_ICONERROR | MB_SYSTEMMODAL);
			else
				::MessageBox(NULL, "Failed to Load CKIniFileDLLModule", "Error", MB_ICONERROR | MB_SYSTEMMODAL);
		}
	}
	else
		::MessageBox(NULL, "CKIniFileDLLModule Not Found", "Error", MB_ICONERROR | MB_SYSTEMMODAL);
}
//---------------------------------------------------------------------------
CKModule_IniFile::~CKModule_IniFile()
{
    if(pCKIniFile != NULL)
    {
        CKIniFile_Close(pCKIniFile);
        pCKIniFile = NULL;
    }
    if(hCKIniFileDLLModule != NULL)
    {
        ::FreeLibrary(hCKIniFileDLLModule);
        hCKIniFileDLLModule = NULL;
    }
	if (pDLLVersion != NULL)
	{
		delete[] pDLLVersion;
		pDLLVersion = NULL;
	}
}
//---------------------------------------------------------------------------
bool CKModule_IniFile::SaveFile()
{
	if(hCKIniFileDLLModule == NULL) return false;
	return CKIniFile_SaveFile(pCKIniFile);
}
//---------------------------------------------------------------------------
bool CKModule_IniFile::SaveFileAs(const char* pFileName)
{
	if(hCKIniFileDLLModule == NULL) return false;
	return CKIniFile_SaveFileAs(pCKIniFile, pFileName);
}
//---------------------------------------------------------------------------
int CKModule_IniFile::ReadInteger(char* pSectionName, char* pKeyName, int iDefaultValue)
{
	if(hCKIniFileDLLModule == NULL) return 0;
	return CKIniFile_ReadInteger(pCKIniFile, pSectionName, pKeyName, iDefaultValue);
}
//---------------------------------------------------------------------------
double CKModule_IniFile::ReadDouble(char* pSectionName, char* pKeyName, double fDefaultValue)
{
	if(hCKIniFileDLLModule == NULL) return 0.0;
	return CKIniFile_ReadDouble(pCKIniFile, pSectionName, pKeyName, fDefaultValue);
}
//---------------------------------------------------------------------------
bool CKModule_IniFile::ReadBoolean(char* pSectionName, char* pKeyName, bool bDefaultValue)
{
	if(hCKIniFileDLLModule == NULL) return false;
	return CKIniFile_ReadBoolean(pCKIniFile, pSectionName, pKeyName, bDefaultValue);
}
//---------------------------------------------------------------------------
void CKModule_IniFile::ReadString(char* pSectionName, char* pKeyName, char* pDefaultString, char* pReturnedString, unsigned long iReturnedStringSize)
{
	if(hCKIniFileDLLModule == NULL) return;
	return CKIniFile_ReadString(pCKIniFile, pSectionName, pKeyName, pDefaultString, pReturnedString, iReturnedStringSize);
}
//---------------------------------------------------------------------------
bool CKModule_IniFile::WriteInteger(char* pSectionName, char* pKeyName, int iKeyValue)
{
	if(hCKIniFileDLLModule == NULL) return false;
	return CKIniFile_WriteInteger(pCKIniFile, pSectionName, pKeyName, iKeyValue);
}
//---------------------------------------------------------------------------
bool CKModule_IniFile::WriteDouble(char* pSectionName, char* pKeyName, double fKeyValue)
{
	if(hCKIniFileDLLModule == NULL) return false;
	return CKIniFile_WriteDouble(pCKIniFile, pSectionName, pKeyName, fKeyValue);
}
//---------------------------------------------------------------------------
bool CKModule_IniFile::WriteBoolean(char* pSectionName, char* pKeyName, bool bKeyValue)
{
	if(hCKIniFileDLLModule == NULL) return false;
	return CKIniFile_WriteBoolean(pCKIniFile, pSectionName, pKeyName, bKeyValue);
}
//---------------------------------------------------------------------------
bool CKModule_IniFile::WriteString(char* pSectionName, char* pKeyName, char* pKeyString)
{
	if(hCKIniFileDLLModule == NULL) return false;
	return CKIniFile_WriteString(pCKIniFile, pSectionName, pKeyName, pKeyString);
}
//---------------------------------------------------------------------------















