//---------------------------------------------------------------------------
#ifndef CKMODULE_INIFILE_H
#define CKMODULE_INIFILE_H
#include <windows.h>
#include <stdio.h>
//---------------------------------------------------------------------------

//Note: Both semicolon or the comment characters of C program will be regarded as comment in INI file

//---------------------------------------------------------------------------
typedef void* (*CKIniFileDLLModule_Open)(const char* pFileName);
typedef void* (*CKIniFileDLLModule_Close)(void* pDLLModule);
typedef bool (*CKIniFileDLLModule_SaveFile)(void* pDLLModule);
typedef bool (*CKIniFileDLLModule_SaveFileAs)(void* pDLLModule, const char* pFileName);
typedef int (*CKIniFileDLLModule_ReadInteger)(void* pDLLModule, char* pSectionName, char* pKeyName, int iDefaultValue);
typedef double (*CKIniFileDLLModule_ReadDouble)(void* pDLLModule, char* pSectionName, char* pKeyName, double fDefaultValue);
typedef bool (*CKIniFileDLLModule_ReadBoolean)(void* pDLLModule, char* pSectionName, char* pKeyName, bool bDefaultValue);
typedef void (*CKIniFileDLLModule_ReadString)(void* pDLLModule, char* pSectionName, char* pKeyName, char* pDefaultString, char* pReturnedString, unsigned long iReturnedStringSize);
typedef bool (*CKIniFileDLLModule_WriteInteger)(void* pDLLModule, char* pSectionName, char* pKeyName, int iKeyValue);
typedef bool (*CKIniFileDLLModule_WriteDouble)(void* pDLLModule, char* pSectionName, char* pKeyName, double fKeyValue);
typedef bool (*CKIniFileDLLModule_WriteBoolean)(void* pDLLModule, char* pSectionName, char* pKeyName, bool bKeyValue);
typedef bool (*CKIniFileDLLModule_WriteString)(void* pDLLModule, char* pSectionName, char* pKeyName, char* pKeyString);

//---------------------------------------------------------------------------
class CKModule_IniFile
{
private:
protected:
	CKIniFileDLLModule_Open CKIniFile_Open;
	CKIniFileDLLModule_Close CKIniFile_Close;
	CKIniFileDLLModule_SaveFile CKIniFile_SaveFile;
	CKIniFileDLLModule_SaveFileAs CKIniFile_SaveFileAs;
	CKIniFileDLLModule_ReadInteger CKIniFile_ReadInteger;
	CKIniFileDLLModule_ReadDouble CKIniFile_ReadDouble;
	CKIniFileDLLModule_ReadBoolean CKIniFile_ReadBoolean;
	CKIniFileDLLModule_ReadString CKIniFile_ReadString;
	CKIniFileDLLModule_WriteInteger CKIniFile_WriteInteger;
	CKIniFileDLLModule_WriteDouble CKIniFile_WriteDouble;
	CKIniFileDLLModule_WriteBoolean CKIniFile_WriteBoolean; 
	CKIniFileDLLModule_WriteString CKIniFile_WriteString; 
	HINSTANCE hCKIniFileDLLModule;
    void* pCKIniFile;
	char* pDLLVersion;

public:
	CKModule_IniFile(const char* pFileName);
	~CKModule_IniFile(void);
	bool Open(const char* pFileName);
	bool Close();
	bool SaveFile(); //Save data to the destination whcih specified in its constructor
	bool SaveFileAs(const char* pFileName);
	int ReadInteger(char* pSectionName, char* pKeyName, int iDefaultValue);
	double ReadDouble(char* pSectionName, char* pKeyName, double fDefaultValue);
	bool ReadBoolean(char* pSectionName, char* pKeyName, bool bDefaultValue);
	void ReadString(char* pSectionName, char* pKeyName, char* pDefaultString, char* pReturnedString, unsigned long iReturnedStringSize);
	bool WriteInteger(char* pSectionName, char* pKeyName, int iKeyValue);
	bool WriteDouble(char* pSectionName, char* pKeyName, double fKeyValue);
	bool WriteBoolean(char* pSectionName, char* pKeyName, bool bKeyValue);
	bool WriteString(char* pSectionName, char* pKeyName, char* pKeyString);
};
//---------------------------------------------------------------------------
#endif
