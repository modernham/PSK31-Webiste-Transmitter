//Added ASM 
//DISABLED LINKER OPTIMIZATION REF AND COMDAT FOLDING
//DIABLED SOME FUNCTION ENCRYPTING

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>  
#include <stdlib.h>  
#include <string.h>  
#include <tchar.h>  
#include <thread>
#include <TlHelp32.h> 
#include <iostream> 
#include <atlstr.h>
#include <cstdlib>
#include <string>
#include <conio.h>
#include <vector>
#include <math.h>  
#include <sstream>
#include <cmath>
#include <ctime>
#include <fstream>
#include "resource.h"
#pragma comment(lib, "urlmon.lib")
using namespace std;
//Define Buttons
#define TXFILE      1008
#define CHECKRXBUTTON 1009
#define TXOFFBUTTON 1010
#define SAVEINFOBUTTON 1015
#define CALLSIGNTEXTID 1012
#define DESTCALLSIGNID 1013
#define FILENAMEID 1014
#define MSG_PSKCHARRDY (WM_USER + 1001)
#define  MSG_STATUSCHANGE ( WM_USER+1002)


//Define DLL Functions
typedef int(__stdcall *f_scard)(HWND hWnd, long cardnum, long numRXchannels);
typedef int(__stdcall *f_fnSendTXString)(char* lpszTXStrg);
typedef int(__stdcall *f_fnStartSoundCardEx)(HWND hWnd, long cardnum, long numRXchannels, long IOMode);
typedef void(__stdcall *f_fnStartTX)(long mode);
typedef int(__stdcall *f_fnSendTXCharacter)(long txchar, long cntrl);
typedef int(__stdcall *f_fnEnableRXChannel)(long chan, long enable);
typedef int(__stdcall *f_fnIsRXChannelActive)(long chan);
typedef int(__stdcall *f_fnSetRXPSKMode)(long mode, long chan);
typedef int(__stdcall *f_fnStopTX)();

bool txOn;
HINSTANCE hGetProcIDDLL;
ofstream logfile;
string buffer;
string CALLSIGN;
string DESTCALLSIGN;
string FILENAME;

string incCallBuffer, incDestBuffer, incFileNameBuffer, incContentBuffer;

bool incCall, incDest, incFileName, incContent;
void opendebugConsole() {
	 // these next few lines create and attach a console
  // to this process.  note that each process is only allowed one console.
  AllocConsole() ;
  AttachConsole( GetCurrentProcessId() ) ;
  freopen( "CON", "w", stdout ) ;

  printf("DEBUG CONSOLE \n" ) ;
}

void setupSoundcard() {
	hGetProcIDDLL = LoadLibrary("PSKCore.dll");
	if (!hGetProcIDDLL) {
		printf("Could not load Dll");
	}

	// resolve function address here
	f_scard funci = (f_scard)GetProcAddress(hGetProcIDDLL, "fnStartSoundCard");
	if (!funci) {
		printf("Could not locate function");
	}

	printf("Start Soundcard return: %d \n" + funci(GetActiveWindow(), -1, 1));

}

void setupSoundcardEX() {
	hGetProcIDDLL = LoadLibrary("PSKCore.dll");
	if (!hGetProcIDDLL) {
		printf("Could not load Dll");
	}

	// resolve function address here
	f_fnStartSoundCardEx funci = (f_fnStartSoundCardEx)GetProcAddress(hGetProcIDDLL, "fnStartSoundCardEx");
	if (!funci) {
		printf("Could not locate function");
	}

	printf("Start soundcard EX return: %d \n" + funci(GetActiveWindow(), -1, 1, 00000));

}
void enableRXChannel(int i) {
	hGetProcIDDLL = LoadLibrary("\PSKCore.dll");
	if (!hGetProcIDDLL) {
		printf("Could not load Dll");
	}

	// resolve function address here
	f_fnEnableRXChannel funci = (f_fnEnableRXChannel)GetProcAddress(hGetProcIDDLL, "fnEnableRXChannel");
	if (!funci) {
		printf("Could not locate function");
	}

	printf("Enable Rx Channel: %d \n" + funci(i, 1));

}

void saveWebpage() {
	fstream webpageName;
	string Saved;
	webpageName.open(incFileNameBuffer, ios::out | ios::app);
	webpageName << incContentBuffer;
	Saved = "Recieced: " + incFileNameBuffer + " From: " + incCallBuffer + " For: " + incDestBuffer + " With content: " + incContentBuffer;
	printf(Saved.c_str());
	webpageName.close();
	incFileNameBuffer = "";
	incCallBuffer = "";
	incDestBuffer = "";
	incContentBuffer = "";



}

void setrxMode() {
	hGetProcIDDLL = LoadLibrary("PSKCore.dll");
	if (!hGetProcIDDLL) {
		printf("Could not load Dll");
	}

	// resolve function address here
	f_fnSetRXPSKMode funci = (f_fnSetRXPSKMode)GetProcAddress(hGetProcIDDLL, "fnSetRXPSKMode");
	if (!funci) {
		printf("Could not locate function");
	}
	funci(0, 0);
	printf("Set the RX Mode on Chan 0 to BPSK \n");

}


void isrxChanActive(int i) {
	hGetProcIDDLL = LoadLibrary("PSKCore.dll");
	if (!hGetProcIDDLL) {
		printf("Could not load Dll");
	}

	// resolve function address here
	f_fnIsRXChannelActive funci = (f_fnIsRXChannelActive)GetProcAddress(hGetProcIDDLL, "fnIsRXChannelActive");
	if (!funci) {
		printf("Could not locate function");
	}
	if (funci(i) == 0)
		printf("RX Chan is NOT active \n");
	if (funci(i) > 0)
		printf("RX Chan is active \n");


}

void startTX() {
	txOn = true;
	// resolve function address here

	f_fnStartTX funci4 = (f_fnStartTX)GetProcAddress(hGetProcIDDLL, "fnStartTX");
	if (!funci4) {
		printf("Could not locate function");
	}
	funci4(0);
	printf("Started Transmitting \n");
}

void stopTX() {
	txOn = false;
	// resolve function address here
	f_fnStopTX funci4 = (f_fnStopTX)GetProcAddress(hGetProcIDDLL, "fnStopTX");
	if (!funci4) {
		printf("Could not locate function");
	}
	funci4();
	printf("\nStopped Transmitting \n");
}


void transmitPage() {
	const int MAX_SIZE = 100000;
	startTX();
	// resolve function address here
	f_fnSendTXString funci3 = (f_fnSendTXString)GetProcAddress(hGetProcIDDLL, "fnSendTXString");
	if (!funci3) {
		printf("Could not locate function");
	}
	ifstream file;
	std::ifstream in(FILENAME);
	std::string contents((std::istreambuf_iterator<char>(in)),
		std::istreambuf_iterator<char>());
	char hexarray[MAX_SIZE];
	string temp = contents;
	contents = "*^" + CALLSIGN + "^%^" + DESTCALLSIGN + "^$^" + FILENAME + "^#^" + temp + "~^";
	strcpy(hexarray, contents.c_str());

	string content;
	if (file.is_open())
	{
		file.close();
	}
	printf("Transmit String Returned: %d \n" + funci3(hexarray));
	stopTX();
}


// The main window class name.  
static TCHAR szWindowClass[] = _T("win32app");
// The string that appears in the application's title bar.  
static TCHAR szTitle[100];
HINSTANCE hInst;
// Forward declarations of functions included in this code module:  
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	//Load Resources
	HCURSOR cursor;
	HICON icon, iconSmall;

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Failed to create Window!"),
			_T("ERROR"),
			NULL);

		return 1;
	}
	hInst = hInstance; // Store instance handle in our global variable  
	HWND hWnd = CreateWindow(
		szWindowClass,
		"PSK31 FILE TRANSFER",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		600, 200,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Failed to create Window!"),
			_T("NULL"),
			NULL);

		return 1;
	}

	// The parameters to ShowWindow explained:  
	// hWnd: the value returned from CreateWindow  
	// nCmdShow: the fourth parameter from WinMain  
	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);

	// Main message loop:  
	opendebugConsole();
   setupSoundcard();
	//setupSoundcardEX();
	enableRXChannel(0);
	setrxMode();
//	enableRXChannel(1);
//	enableRXChannel(2);
	txOn = false;

	MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return (int)msg.wParam;
	}

//  
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)  
//  
//  PURPOSE:  Processes messages for the main window.  
//  
//  WM_PAINT    - Paint the main window  
//  WM_DESTROY  - post a quit message and return  
//  
//  



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	static HWND txFileButton, rxButton, txOffButton, callsignText, destCallsignText, fileNameText, saveInfoB;
	char* SOMETHING;


	switch (message)
	{

	case MSG_PSKCHARRDY:
		if (txOn == false) {
			char textChar = (char)wParam;
			int channel = (int)lParam;
				printf("%c", textChar);

				if (textChar == '^') {
					char check = buffer.back();
					switch (check) {
					case '*':
						incCall = true;
						break;
					case '%':
						incDest = true;
						break;
					case '$':
						incFileName = true;
						break;
					case '#':
						incContent = true;
						break;
					case '~':
						saveWebpage();
						break;
					default:
						printf("?");
						incCall = false;
						incDest = false;
						incFileName = false;
						incContent = false;
						break;
					}
				}

				buffer += textChar;
					if (textChar != '^'){
						if (incCall)
							incCallBuffer += textChar;
						if (incDest)
							incDestBuffer += textChar;
						if (incFileName)
							incFileNameBuffer += textChar;
						if (incContent)
							incContentBuffer += textChar;
				}


		}
		break;

	case MSG_STATUSCHANGE:
		if (true) {
			int changeCode = (char)wParam;
				printf("\nStatus Change: %d \n", changeCode);
		}
		break;

	case WM_CREATE:

		txFileButton = CreateWindow("button", "TX File",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			5, 100,
			120, 20,
			hWnd, (HMENU)TXFILE,
			hInst, NULL);


		saveInfoB = CreateWindow("button", "SaveInfo",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			440, 25,
			120, 20,
			hWnd, (HMENU)SAVEINFOBUTTON,
			hInst, NULL);

		callsignText = CreateWindow("EDIT", "YourCall",
			WS_CHILD | WS_VISIBLE | ES_LEFT,
			5, 25,
			120, 20,
			hWnd, (HMENU)CALLSIGNTEXTID,
			hInst, NULL);

		destCallsignText = CreateWindow("EDIT", "TheirCall",
			WS_CHILD | WS_VISIBLE | ES_LEFT,
			150, 25,
			120, 20,
			hWnd, (HMENU)DESTCALLSIGNID,
			hInst, NULL);

		fileNameText = CreateWindow("EDIT", "index.html",
			WS_CHILD | WS_VISIBLE | ES_LEFT,
			295, 25,
			120, 20,
			hWnd, (HMENU)FILENAMEID,
			hInst, NULL);

		txOffButton = CreateWindow("button", "TX OFF",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			130, 100,
			120, 20,
			hWnd, (HMENU)TXOFFBUTTON,
			hInst, NULL);



		rxButton = CreateWindow("button", "Check RX",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			5, 125,
			120,  20,
			hWnd, (HMENU)CHECKRXBUTTON,
			hInst, NULL);
		break;

	case WM_COMMAND: {

		switch (LOWORD(wParam)) {

		case TXFILE: {
			transmitPage();
			break;
		}
		case CHECKRXBUTTON: {
			isrxChanActive(0);
			break;
		}
		case SAVEINFOBUTTON: {
			char buff[40];
			CALLSIGN = GetDlgItemText(hWnd, CALLSIGNTEXTID, buff, 20);
			CALLSIGN = buff;
			FILENAME = GetDlgItemText(hWnd, FILENAMEID, buff, 20);
			FILENAME = buff;
			DESTCALLSIGN = GetDlgItemText(hWnd, DESTCALLSIGNID, buff, 20);
			DESTCALLSIGN = buff;
			break;
		}
		case TXOFFBUTTON: {
			stopTX();
			break;
		}
		}
	}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc,
			5, 5,
			"Callsign:", 10);
		TextOut(hdc,
			150, 5,
			"Recpient Callsign:", 18);
		TextOut(hdc,
			295, 5,
			"File to Transmit:", 20);

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		logfile.close();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}
