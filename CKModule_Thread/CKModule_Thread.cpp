//---------------------------------------------------------------------------
#pragma hdrstop
//for VC++
//===========================================================================

// #include "stdafx.h"
// #pragma comment(lib, "Version.lib")

//===========================================================================
#include "CKModule_Thread.h"
//---------------------------------------------------------------------------

//////////////////////////// UI THREAD FUNCTIONS ////////////////////////////
//--------------------------------Separation---------------------------------
CKModule_UIThread::CKModule_UIThread(void* pOwnObject, CKTHREADDLLMODULE_ONINITIALIZATION pOnInitialization, CKTHREADDLLMODULE_ONTERMINATION pOnTermination, CKTHREADDLLMODULE_ONEVENT pOnEvent, HWND hParentWnd)
{
	hCKThreadDLLModule = NULL;
	pCKUIThread = NULL;
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
		::strcat(cDLLFileName, "CKThreadDLLModule.dll");
		hCKThreadDLLModule = ::LoadLibrary(cDLLFileName);
		if(hCKThreadDLLModule != NULL)
		{
			//Get DLL version
			//===================================================================
			char cDLLRevisionNumber[64] = {'\0'};
			DWORD iFileVersionInfoSize = ::GetFileVersionInfoSize(cDLLFileName, NULL);
			unsigned char* pFileVersionInfoBuffer = new unsigned char[iFileVersionInfoSize];
			bool bResult = ::GetFileVersionInfo(cDLLFileName, 0, iFileVersionInfoSize, pFileVersionInfoBuffer);
			if(bResult)
			{
				VS_FIXEDFILEINFO* pFixedFileInfo;
				UINT iFixedFileInfoSize = 0;
				bResult = ::VerQueryValue(pFileVersionInfoBuffer, "\\", (void**)&pFixedFileInfo, &iFixedFileInfoSize);
				if(bResult)
				{
					::sprintf(cDLLRevisionNumber, "%d.%d.%d.%d", ((pFixedFileInfo->dwFileVersionMS>>16)&0xffff), ((pFixedFileInfo->dwFileVersionMS>>0)&0xffff)
															   , ((pFixedFileInfo->dwFileVersionLS>>16)&0xffff), ((pFixedFileInfo->dwFileVersionLS>>0)&0xffff));
					pDLLVersion = new char[::strlen(cDLLRevisionNumber) + 1];
					::strcpy(pDLLVersion, cDLLRevisionNumber);
				}
			}
			delete [] pFileVersionInfoBuffer;
			//===================================================================
			CKUIThread_Open = (CKThreadDLLModule_UIThread_Open)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_UIThread_Open");
			CKUIThread_Close = (CKThreadDLLModule_UIThread_Close)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_UIThread_Close");
			CKUIThread_TriggerUIEvent = (CKThreadDLLModule_UIThread_TriggerUIEvent)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_UIThread_TriggerUIEvent");
			CKUIThread_UITriggerOtherWorkerThreadEvent = (CKThreadDLLModule_UIThread_UITriggerOtherWorkerThreadEvent)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_UIThread_UITriggerOtherWorkerThreadEvent");
			CKUIThread_GetThreadID = (CKThreadDLLModule_UIThread_GetThreadID)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_UIThread_GetThreadID");
			CKUIThread_SetThreadName = (CKThreadDLLModule_UIThread_SetThreadName)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_UIThread_SetThreadName");

			if(pOwnObject == NULL)
				pOwnObject = (void*)this;
			pCKUIThread = CKUIThread_Open(pOwnObject, pOnInitialization, pOnTermination, pOnEvent, hParentWnd);
		}
		else
		{
			DWORD iResult = ::GetLastError();
			if(iResult == 193)
				::MessageBox(NULL, "CKThreadDLLModule Is Not Compatible To Platform", "Error", MB_ICONERROR | MB_SYSTEMMODAL);
			else
				::MessageBox(NULL, "Failed to Load CKThreadDLLModule", "Error", MB_ICONERROR | MB_SYSTEMMODAL);
		}
	}
	else
		::MessageBox(NULL, "CKThreadDLLModule Not Found", "Error", MB_ICONERROR | MB_SYSTEMMODAL);
}
//---------------------------------------------------------------------------
CKModule_UIThread::~CKModule_UIThread()
{
    if(pCKUIThread != NULL)
    {
        pCKUIThread = CKUIThread_Close(pCKUIThread);
        pCKUIThread = NULL;
    }
	if(hCKThreadDLLModule != NULL)
	{
		::FreeLibrary(hCKThreadDLLModule);
		hCKThreadDLLModule = NULL;
	}
    if(pDLLVersion != NULL)
	{
        delete [] pDLLVersion;
		pDLLVersion = NULL;
	}
}
//---------------------------------------------------------------------------
bool CKModule_UIThread::TriggerUIEvent(unsigned long iEvent, void* pParameter, bool bWaitToReturn)
{
    if(hCKThreadDLLModule == NULL) return false;
    return CKUIThread_TriggerUIEvent(pCKUIThread, iEvent, pParameter, bWaitToReturn);
}
//---------------------------------------------------------------------------
bool CKModule_UIThread::UITriggerOtherWorkerThreadEvent(void* pTargetWorkerThread, unsigned long iEvent, void* pParameter, bool bWaitToReturn)
{
    if(hCKThreadDLLModule == NULL) return false;
    CKModule_WorkerThread* pTargetThread = (CKModule_WorkerThread*)pTargetWorkerThread;
    return CKUIThread_UITriggerOtherWorkerThreadEvent(pCKUIThread, pTargetThread->pCKWorkerThread, iEvent, pParameter, bWaitToReturn);
}
//---------------------------------------------------------------------------
unsigned long CKModule_UIThread::GetThreadID(void)
{
    if(hCKThreadDLLModule == NULL) return 0;
    return CKUIThread_GetThreadID(pCKUIThread);
}
//---------------------------------------------------------------------------
void CKModule_UIThread::SetThreadName(char* pName)
{
	if(hCKThreadDLLModule == NULL) return;
	return CKUIThread_SetThreadName(pCKUIThread, pName); 	
}
//---------------------------------------------------------------------------
//////////////////////////// UI THREAD FUNCTIONS ////////////////////////////

////////////////////////// WORKER THREAD FUNCTIONS //////////////////////////
//--------------------------------Separation---------------------------------
CKModule_WorkerThread::CKModule_WorkerThread(void* pOwnObject, unsigned long iPollingCycle, CKTHREADDLLMODULE_ONINITIALIZATION pOnInitialization, CKTHREADDLLMODULE_ONTERMINATION pOnTermination, CKTHREADDLLMODULE_ONEVENT pOnEvent)
{
    hCKThreadDLLModule = NULL;
	pCKWorkerThread = NULL;
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
		::strcat(cDLLFileName, "CKThreadDLLModule.dll");
		hCKThreadDLLModule = ::LoadLibrary(cDLLFileName);
		if(hCKThreadDLLModule != NULL)
		{
			//Get DLL version
			//===================================================================
			char cDLLRevisionNumber[64] = {'\0'};
			DWORD iFileVersionInfoSize = ::GetFileVersionInfoSize(cDLLFileName, NULL);
			unsigned char* pFileVersionInfoBuffer = new unsigned char[iFileVersionInfoSize];
			bool bResult = ::GetFileVersionInfo(cDLLFileName, 0, iFileVersionInfoSize, pFileVersionInfoBuffer);
			if(bResult)
			{
				VS_FIXEDFILEINFO* pFixedFileInfo;
				UINT iFixedFileInfoSize = 0;
				bResult = ::VerQueryValue(pFileVersionInfoBuffer, "\\", (void**)&pFixedFileInfo, &iFixedFileInfoSize);
				if(bResult)
				{
					::sprintf(cDLLRevisionNumber, "%d.%d.%d.%d", ((pFixedFileInfo->dwFileVersionMS>>16)&0xffff), ((pFixedFileInfo->dwFileVersionMS>>0)&0xffff)
															   , ((pFixedFileInfo->dwFileVersionLS>>16)&0xffff), ((pFixedFileInfo->dwFileVersionLS>>0)&0xffff));
					pDLLVersion = new char[::strlen(cDLLRevisionNumber) + 1];
					::strcpy(pDLLVersion, cDLLRevisionNumber);
				}
			}
			delete [] pFileVersionInfoBuffer;
			//===================================================================
			CKWorkerThread_Open = (CKThreadDLLModule_WorkerThread_Open)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_WorkerThread_Open");
			CKWorkerThread_Close = (CKThreadDLLModule_WorkerThread_Close)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_WorkerThread_Close");
			CKWorkerThread_TriggerEvent = (CKThreadDLLModule_WorkerThread_TriggerEvent)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_WorkerThread_TriggerEvent");
			CKWorkerThread_TriggerOtherWorkerThreadEvent = (CKThreadDLLModule_WorkerThread_TriggerOtherWorkerThreadEvent)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_WorkerThread_TriggerOtherWorkerThreadEvent");
			CKWorkerThread_PeekEvent = (CKThreadDLLModule_WorkerThread_PeekEvent)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_WorkerThread_PeekEvent");
			CKWorkerThread_QuitEventWait = (CKThreadDLLModule_WorkerThread_QuitEventWait)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_WorkerThread_QuitEventWait");
			CKWorkerThread_SetThreadPriority = (CKThreadDLLModule_WorkerThread_SetThreadPriority)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_WorkerThread_SetThreadPriority");
			CKWorkerThread_GetThreadID = (CKThreadDLLModule_WorkerThread_GetThreadID)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_WorkerThread_GetThreadID");
			CKWorkerThread_SetThreadName = (CKThreadDLLModule_WorkerThread_SetThreadName)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_WorkerThread_SetThreadName");
			CKWorkerThread_ResumeThread = (CKThreadDLLModule_WorkerThread_ResumeThread)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_WorkerThread_ResumeThread");
			CKWorkerThread_SuspendThread = (CKThreadDLLModule_WorkerThread_SuspendThread)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_WorkerThread_SuspendThread");
			CKWorkerThread_EnableTimer = (CKThreadDLLModule_WorkerThread_EnableTimer)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_WorkerThread_EnableTimer");
			CKWorkerThread_DisableTimer = (CKThreadDLLModule_WorkerThread_DisableTimer)::GetProcAddress(hCKThreadDLLModule, "CKThreadDLLModule_WorkerThread_DisableTimer");

			if(pOwnObject == NULL)
				pOwnObject = (void*)this;
			pCKWorkerThread = CKWorkerThread_Open(pOwnObject, iPollingCycle, pOnInitialization, pOnTermination, pOnEvent);
		}
		else
		{
			DWORD iResult = ::GetLastError();
			if(iResult == 193)
				::MessageBox(NULL, "CKThreadDLLModule Is Not Compatible To Platform", "Error", MB_ICONERROR | MB_SYSTEMMODAL);
			else
				::MessageBox(NULL, "Failed to Load CKThreadDLLModule", "Error", MB_ICONERROR | MB_SYSTEMMODAL);
		}
    }
    else
        ::MessageBox(NULL, "Failed to Load CKThreadDLLModule", "Error", MB_ICONERROR | MB_SYSTEMMODAL);
}
//---------------------------------------------------------------------------
CKModule_WorkerThread::~CKModule_WorkerThread()
{
    if(pCKWorkerThread != NULL)
    {
        pCKWorkerThread = CKWorkerThread_Close(pCKWorkerThread);
        pCKWorkerThread = NULL;
    }
    if(hCKThreadDLLModule != NULL)
    {
        ::FreeLibrary(hCKThreadDLLModule);
        hCKThreadDLLModule = NULL;
    }
    if(pDLLVersion != NULL)
	{
        delete [] pDLLVersion;
		pDLLVersion = NULL;
	}
}
//---------------------------------------------------------------------------
bool CKModule_WorkerThread::TriggerEvent(unsigned long iEvent, void* pParameter, bool bWaitToReturn)
{
    if(hCKThreadDLLModule == NULL) return false;
    return CKWorkerThread_TriggerEvent(pCKWorkerThread, iEvent, pParameter, bWaitToReturn);
}
//---------------------------------------------------------------------------
bool CKModule_WorkerThread::TriggerOtherWorkerThreadEvent(void* pTargetWorkerThread, unsigned long iEvent, void* pParameter, bool bWaitToReturn)
{
    if(hCKThreadDLLModule == NULL) return false;
    CKModule_WorkerThread* pTargetThread = (CKModule_WorkerThread*)pTargetWorkerThread;
    return CKWorkerThread_TriggerOtherWorkerThreadEvent(pCKWorkerThread, pTargetThread->pCKWorkerThread, iEvent, pParameter, bWaitToReturn);
}
//---------------------------------------------------------------------------
bool CKModule_WorkerThread::PeekEvent(unsigned long iFirstEvent, unsigned long iLastEvent, bool bRemoveEvent, stCKThreadDLLModule_PeekEventQueueData* pPeekEventQueueData)
{
    if(hCKThreadDLLModule == NULL) return false;
    return CKWorkerThread_PeekEvent(pCKWorkerThread, iFirstEvent, iLastEvent, bRemoveEvent, pPeekEventQueueData);
}
//---------------------------------------------------------------------------
bool CKModule_WorkerThread::QuitEventWait(void)
{
    if(hCKThreadDLLModule == NULL) return false;
    return CKWorkerThread_QuitEventWait(pCKWorkerThread);
}
//---------------------------------------------------------------------------
bool CKModule_WorkerThread::SetThreadPriority(int iPriority)
{
    if(hCKThreadDLLModule == NULL) return false;
    return CKWorkerThread_SetThreadPriority(pCKWorkerThread, iPriority);
}
//---------------------------------------------------------------------------
unsigned long CKModule_WorkerThread::GetThreadID(void)
{
    if(hCKThreadDLLModule == NULL) return 0;
    return CKWorkerThread_GetThreadID(pCKWorkerThread);
}
//---------------------------------------------------------------------------
void CKModule_WorkerThread::SetThreadName(char* pName)
{
	if(hCKThreadDLLModule == NULL) return;
	return CKWorkerThread_SetThreadName(pCKWorkerThread, pName);
}
//---------------------------------------------------------------------------
unsigned long CKModule_WorkerThread::ResumeThread(void)
{
    if(hCKThreadDLLModule == NULL) return -1;
    return CKWorkerThread_ResumeThread(pCKWorkerThread);
}
//---------------------------------------------------------------------------
unsigned long CKModule_WorkerThread::SuspendThread(void)
{
    if(hCKThreadDLLModule == NULL) return -1;
    return CKWorkerThread_SuspendThread(pCKWorkerThread);
}
//---------------------------------------------------------------------------
UINT_PTR CKModule_WorkerThread::EnableTimer(unsigned long iPollingCycle, CKTHREADDLLMODULE_ONTIMER pTimerProc)
{
	if(hCKThreadDLLModule == NULL) return false;
	return CKWorkerThread_EnableTimer(pCKWorkerThread, iPollingCycle, pTimerProc);
}
//---------------------------------------------------------------------------
bool CKModule_WorkerThread::DisableTimer(UINT_PTR iTimerID)
{
	if(hCKThreadDLLModule == NULL) return false;
	return CKWorkerThread_DisableTimer(pCKWorkerThread, iTimerID);
}
//---------------------------------------------------------------------------
////////////////////////// WORKER THREAD FUNCTIONS //////////////////////////

