//---------------------------------------------------------------------------
#ifndef CKMODULE_SERIALPORT_H
#define CKMODULE_SERIALPORT_H
#include <windows.h>
#include <stdio.h>
//---------------------------------------------------------------------------

//Callback Functions
/*
EV_BREAK		A break was detected on input.
EV_CTS			The CTS (clear-to-send) signal changed state.
EV_DSR			The DSR (data-set-ready) signal changed state.
EV_ERR			A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY.
EV_RING			A ring indicator was detected.
EV_RLSD			The RLSD (receive-line-signal-detect) signal changed state.
EV_RXCHAR		A character was received and placed in the input buffer.
EV_RXFLAG		The event character was received and placed in the input buffer. The event character is specified in the device's DCB structure, which is applied to a serial port by using the SetCommState function.
EV_TXEMPTY		The last character in the output buffer was sent.
*/
typedef void (*CKMODULE_SERIALPORT_ONNOTIFY)(void* pOwnObject, unsigned long iNotifiedEvent);

//---------------------------------------------------------------------------
typedef void* (*CKSerialPortDLLModule_Create)(void* pCallbackObject, CKMODULE_SERIALPORT_ONNOTIFY pCallback_OnNotify);
typedef void*  (*CKSerialPortDLLModule_Close)(void* pDLLModule);
typedef bool (*CKSerialPortDLLModule_OpenPort)(void* pDLLModule, char* pPortName, unsigned long iBaudRate, unsigned char iDataBits, unsigned char iParity, unsigned char iStopBits);
typedef bool (*CKSerialPortDLLModule_ClosePort)(void* pDLLModule);
typedef bool (*CKSerialPortDLLModule_SendData)(void* pDLLModule, unsigned char* pData, unsigned long iDataSize, bool bSynchronized);
typedef unsigned long (*CKSerialPortDLLModule_GetDataSize)(void* pDLLModule);
typedef unsigned long (*CKSerialPortDLLModule_GetData)(void* pDLLModule, unsigned char* pData, unsigned long iDataSize);

//---------------------------------------------------------------------------
class CKModule_SerialPort
{
private:
protected:
    CKSerialPortDLLModule_Create CKSerialPort_Create;
    CKSerialPortDLLModule_Close CKSerialPort_Close;
    CKSerialPortDLLModule_OpenPort CKSerialPort_OpenPort;
    CKSerialPortDLLModule_ClosePort CKSerialPort_ClosePort;
    CKSerialPortDLLModule_SendData CKSerialPort_SendData;
    CKSerialPortDLLModule_GetDataSize CKSerialPort_GetDataSize;
    CKSerialPortDLLModule_GetData CKSerialPort_GetData;
	HINSTANCE hCKSerialPortDLLModule;
	void* pCKSerialPort;
	char* pDLLVersion;

public:
    CKModule_SerialPort(void* pCallbackObject, CKMODULE_SERIALPORT_ONNOTIFY pCallback_OnNotify);
    ~CKModule_SerialPort();
	/*
	</Param> StopBits <Param/>
	ONESTOPBIT		0
	ONE5STOPBITS	1
	TWOSTOPBITS		2
	
	</Param> Parity <Param/>
	EVENPARITY		2
	MARKPARITY		3
	NOPARITY		0
	ODDPARITY		1
	SPACEPARITY		4
	*/
	bool OpenPort(char* pPortName = "COM1", unsigned long iBaudRate = 115200, unsigned char iDataBits = 8, unsigned char iParity = 0, unsigned char iStopBits = ONESTOPBIT);
	bool ClosePort(void);
	bool SendData(unsigned char* pData, unsigned long iDataSize, bool bSynchronized = true);
	unsigned long GetDataSize(void);
	unsigned long GetData(unsigned char* pData, unsigned long iDataSize);
};
//---------------------------------------------------------------------------
#endif
