//---------------------------------------------------------------------------
#ifndef CKMODULE_THREAD_H
#define CKMODULE_THREAD_H
#include <windows.h>
#include <stdio.h>
#include <time.h>
#pragma comment(lib, "User32.lib") //Prevent an error happening in case the solution doesn't include the standard window library 
//---------------------------------------------------------------------------

#ifdef _DEBUG
	#ifndef DEBUG_NEW //MFC default memory leak detection mechanism
        #ifndef DEBUG_CLIENTBLOCK //for C++ memory leak detection
		    #define DEBUG_CLIENTBLOCK new (_CLIENT_BLOCK, __FILE__, __LINE__)
        #endif  
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
    #define new DEBUG_CLIENTBLOCK
    #endif
#endif

//Put the following function at the entry point of the program if the problem of memory leaks of a non-MFC program would be examined
//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

//_CrtSetBreakAlloc(long lBreakAlloc) //Sets a breakpoint on a specified object allocation order number (debug version only)

//Thread private data structure (for PeekEvent function)
//---------------------------------------------------------------------------
typedef struct STCKTHREADDLLMODULE_PEEKEVENTQUEUEDATA
{
	unsigned long iEvent;
	void* pParameter;
}stCKThreadDLLModule_PeekEventQueueData;

//Thread callback functions
//---------------------------------------------------------------------------
typedef void (*CKTHREADDLLMODULE_ONINITIALIZATION)(void* pOwnObject);
typedef void (*CKTHREADDLLMODULE_ONTERMINATION)(void* pOwnObject);
typedef void (*CKTHREADDLLMODULE_ONEVENT)(void* pOwnObject, unsigned long iEvent, void* pParameter);
typedef void (*CKTHREADDLLMODULE_ONTIMER)(void* pOwnObject, UINT_PTR iTimerID);

//////////////////////////// UI THREAD FUNCTIONS ////////////////////////////
//---------------------------------------------------------------------------
typedef void* (*CKThreadDLLModule_UIThread_Open)(void* pOwnObject, CKTHREADDLLMODULE_ONINITIALIZATION pOnInitialization, CKTHREADDLLMODULE_ONTERMINATION pOnTermination, CKTHREADDLLMODULE_ONEVENT pOnEvent, HWND hParentWnd);
typedef void* (*CKThreadDLLModule_UIThread_Close)(void* pDLLModule);
typedef bool (*CKThreadDLLModule_UIThread_TriggerUIEvent)(void* pDLLModule, unsigned long iEvent, void* pParameter, bool bWaitToReturn);
typedef bool (*CKThreadDLLModule_UIThread_UITriggerOtherWorkerThreadEvent)(void* pDLLModule, void* pTargetWorkerThread, unsigned long iEvent, void* pParameter, bool bWaitToReturn);
typedef unsigned long (*CKThreadDLLModule_UIThread_GetThreadID)(void* pDLLModule);
typedef void (*CKThreadDLLModule_UIThread_SetThreadName)(void* pDLLModule, char* pName); 

//---------------------------------------------------------------------------
class CKModule_UIThread
{
private:
protected:
    HINSTANCE hCKThreadDLLModule;
    CKThreadDLLModule_UIThread_Open CKUIThread_Open;
    CKThreadDLLModule_UIThread_Close CKUIThread_Close;
    CKThreadDLLModule_UIThread_TriggerUIEvent CKUIThread_TriggerUIEvent;
    CKThreadDLLModule_UIThread_UITriggerOtherWorkerThreadEvent CKUIThread_UITriggerOtherWorkerThreadEvent;
    CKThreadDLLModule_UIThread_GetThreadID CKUIThread_GetThreadID;
    CKThreadDLLModule_UIThread_SetThreadName CKUIThread_SetThreadName;
    void* pCKUIThread;
    char* pDLLVersion;

public:
    CKModule_UIThread(void* pOwnObject, CKTHREADDLLMODULE_ONINITIALIZATION pOnInitialization, CKTHREADDLLMODULE_ONTERMINATION pOnTermination, CKTHREADDLLMODULE_ONEVENT pOnEvent, HWND hParentWnd = NULL);
    ~CKModule_UIThread();
    bool TriggerUIEvent(unsigned long iEvent, void* pParameter, bool bWaitToReturn);
    bool UITriggerOtherWorkerThreadEvent(void* pTargetWorkerThread, unsigned long iEvent, void* pParameter, bool bWaitToReturn);
    unsigned long GetThreadID(void);
    void SetThreadName(char* pName);
    char* GetVersion(void) { return pDLLVersion; };
};
//---------------------------------------------------------------------------
//////////////////////////// UI THREAD FUNCTIONS ////////////////////////////

////////////////////////// WORKER THREAD FUNCTIONS //////////////////////////
//---------------------------------------------------------------------------
typedef void* (*CKThreadDLLModule_WorkerThread_Open)(void* pOwnObject, unsigned long iPollingCycle, CKTHREADDLLMODULE_ONINITIALIZATION pOnInitialization, CKTHREADDLLMODULE_ONTERMINATION pOnTermination, CKTHREADDLLMODULE_ONEVENT pOnEvent);
typedef void* (*CKThreadDLLModule_WorkerThread_Close)(void* pDLLModule);
typedef bool (*CKThreadDLLModule_WorkerThread_TriggerEvent)(void* pDLLModule, unsigned long iEvent, void* pParameter, bool bWaitToReturn);
typedef bool (*CKThreadDLLModule_WorkerThread_TriggerOtherWorkerThreadEvent)(void* pDLLModule, void* pTargetWorkerThread, unsigned long iEvent, void* pParameter, bool bWaitToReturn);
typedef bool (*CKThreadDLLModule_WorkerThread_PeekEvent)(void* pDLLModule, unsigned long iFirstEvent, unsigned long iLastEvent, bool bRemoveEvent, stCKThreadDLLModule_PeekEventQueueData* pPeekEventQueueData);
typedef bool (*CKThreadDLLModule_WorkerThread_QuitEventWait)(void* pDLLModule);
typedef bool (*CKThreadDLLModule_WorkerThread_SetThreadPriority)(void* pDLLModule, int iPriority);
typedef unsigned long (*CKThreadDLLModule_WorkerThread_GetThreadID)(void* pDLLModule);
typedef void (*CKThreadDLLModule_WorkerThread_SetThreadName)(void* pDLLModule, char* pName);
typedef unsigned long (*CKThreadDLLModule_WorkerThread_ResumeThread)(void* pDLLModule);
typedef unsigned long (*CKThreadDLLModule_WorkerThread_SuspendThread)(void* pDLLModule);
typedef UINT_PTR (*CKThreadDLLModule_WorkerThread_EnableTimer)(void* pDLLModule, unsigned long iPollingCycle, CKTHREADDLLMODULE_ONTIMER pTimerProc);
typedef bool (*CKThreadDLLModule_WorkerThread_DisableTimer)(void* pDLLModule, UINT_PTR iTimerID);

//---------------------------------------------------------------------------
class CKModule_WorkerThread
{
private:
protected:
    HINSTANCE hCKThreadDLLModule;
    CKThreadDLLModule_WorkerThread_Open CKWorkerThread_Open;
    CKThreadDLLModule_WorkerThread_Close CKWorkerThread_Close;
    CKThreadDLLModule_WorkerThread_TriggerEvent CKWorkerThread_TriggerEvent;
    CKThreadDLLModule_WorkerThread_TriggerOtherWorkerThreadEvent CKWorkerThread_TriggerOtherWorkerThreadEvent;
    CKThreadDLLModule_WorkerThread_PeekEvent CKWorkerThread_PeekEvent;
    CKThreadDLLModule_WorkerThread_QuitEventWait CKWorkerThread_QuitEventWait;
    CKThreadDLLModule_WorkerThread_SetThreadPriority CKWorkerThread_SetThreadPriority;
    CKThreadDLLModule_WorkerThread_GetThreadID CKWorkerThread_GetThreadID;
    CKThreadDLLModule_WorkerThread_SetThreadName CKWorkerThread_SetThreadName;
    CKThreadDLLModule_WorkerThread_ResumeThread CKWorkerThread_ResumeThread;
    CKThreadDLLModule_WorkerThread_SuspendThread CKWorkerThread_SuspendThread;
	CKThreadDLLModule_WorkerThread_EnableTimer CKWorkerThread_EnableTimer;
	CKThreadDLLModule_WorkerThread_DisableTimer CKWorkerThread_DisableTimer;
    friend class CKModule_UIThread;
    void* pCKWorkerThread;
    char* pDLLVersion;

public:
    CKModule_WorkerThread(void* pOwnObject, unsigned long iPollingCycle, CKTHREADDLLMODULE_ONINITIALIZATION pOnInitialization, CKTHREADDLLMODULE_ONTERMINATION pOnTermination, CKTHREADDLLMODULE_ONEVENT pOnEvent);
    ~CKModule_WorkerThread();
    bool TriggerEvent(unsigned long iEvent, void* pParameter, bool bWaitToReturn);
    bool TriggerOtherWorkerThreadEvent(void* pTargetWorkerThread, unsigned long iEvent, void* pParameter, bool bWaitToReturn);
    bool PeekEvent(unsigned long iFirstEvent, unsigned long iLastEvent, bool bRemoveEvent, stCKThreadDLLModule_PeekEventQueueData* pPeekEventQueueData);
    bool QuitEventWait(void);
    /*
	THREAD_PRIORITY_HIGHEST			: 2
	THREAD_PRIORITY_ABOVE_NORMAL	: 1
	THREAD_PRIORITY_NORMAL			: 0
	THREAD_PRIORITY_BELOW_NORMAL	:-1	
	THREAD_PRIORITY_LOWEST			:-2
	*/
    bool SetThreadPriority(int iPriority = 0);
    unsigned long GetThreadID(void);
	void SetThreadName(char* pName);
	unsigned long ResumeThread(void);
	unsigned long SuspendThread(void);
    UINT_PTR EnableTimer(unsigned long iPollingCycle, CKTHREADDLLMODULE_ONTIMER pTimerProc);
    bool DisableTimer(UINT_PTR iTimerID);
	char* GetVersion(void) { return pDLLVersion; };
};
//---------------------------------------------------------------------------
////////////////////////// WORKER THREAD FUNCTIONS //////////////////////////

//---------------------------------------------------------------------------
class CKObject_Locker
{
private:
    CRITICAL_SECTION csLock;
protected:
public:
    CKObject_Locker()
    {
        ::memset(&csLock, 0, sizeof(CRITICAL_SECTION));
        ::InitializeCriticalSection(&csLock);
    }
    ~CKObject_Locker()
    {
        Unlock();
        ::DeleteCriticalSection(&csLock);
    }
    void Lock()
    {
        ::EnterCriticalSection(&csLock);
    }
    void Unlock()
    {
        ::LeaveCriticalSection(&csLock);
    }
};
//---------------------------------------------------------------------------
class CKObject_StopWatch
{
private:
    CKObject_Locker* pCKObject_Locker; //Use to prevent multi-thread accessing simultaneously
    bool bIsRunning;
    clock_t iElapsedTime_Start;
    clock_t iElapsedTime_End;
    clock_t iElapsedTime_Total;
    double fElapsedTime_Milliseconds; 
protected:
public:
    CKObject_StopWatch()
    {
        pCKObject_Locker = new CKObject_Locker();
        bIsRunning = false;
        iElapsedTime_Start = 0;
        iElapsedTime_End = 0;
        iElapsedTime_Total = 0;
        fElapsedTime_Milliseconds = 0.0;
    };
    ~CKObject_StopWatch()
    {
        delete pCKObject_Locker;
        pCKObject_Locker = NULL;
    }
    void Start(void)
    {
        pCKObject_Locker->Lock();
        if (!bIsRunning)
        {
            iElapsedTime_Start = ::clock();
            bIsRunning = true;
        }
        pCKObject_Locker->Unlock();
    }
    void Stop(void)
    {
        pCKObject_Locker->Lock();
        if (bIsRunning)
        {
            iElapsedTime_End = ::clock();
            iElapsedTime_Total = iElapsedTime_End - iElapsedTime_Start;
            bIsRunning = false;
            if (iElapsedTime_Total < 0)
                iElapsedTime_Total = 0;
        }
        pCKObject_Locker->Unlock();
    }
    void Reset(void)
    {
        pCKObject_Locker->Lock();
        iElapsedTime_Total = 0;
        bIsRunning = false;
        iElapsedTime_Start = 0;
        pCKObject_Locker->Unlock();
    }
    void Restart(void)
    {
        pCKObject_Locker->Lock();
        iElapsedTime_Total = 0;
        iElapsedTime_Start = ::clock();
        bIsRunning = true;
        pCKObject_Locker->Unlock();
    }
    bool IsRunning()
    {
        bool bStatus = false;
        pCKObject_Locker->Lock();
        bStatus = bIsRunning;
        pCKObject_Locker->Unlock();
        return bStatus;
    }
    double ElapsedMilliseconds()
    {
        double fElapsedTime_UntilNow = 0.0;
        pCKObject_Locker->Lock();
        clock_t iElapsedTime = iElapsedTime_Total;
        if (bIsRunning)
        {
            clock_t iElapsedTime_Current = clock();
            iElapsedTime += (iElapsedTime_Current - iElapsedTime_Start);
        }
        fElapsedTime_UntilNow = (double)iElapsedTime / CLOCKS_PER_SEC * 1000;
        pCKObject_Locker->Unlock();
        return fElapsedTime_UntilNow;
    }
};
//---------------------------------------------------------------------------
#endif
