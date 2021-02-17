// PSK31TRANSMITTERv2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include <cstring>
#include <iostream>


using namespace std;

int CallMyDLL(void)
{
	/* get handle to dll */
	HINSTANCE hGetProcIDDLL = LoadLibrary("C:\\PSKCore.dll");
	/* get pointer to the function in the dll*/
	FARPROC lpfnGetProcessID = GetProcAddress(HMODULE(hGetProcIDDLL), "fnStartSoundCard");
	cout << "Got Proc";
	/*
	Define the Function in the DLL for reuse. This is just prototyping the dll's function.
	A mock of it. Use "stdcall" for maximum compatibility.
	*/
	typedef long(__stdcall * pICFUNC)(HWND hWnd, long cardnum, long numRXchannels);


	pICFUNC MyFunction;
	MyFunction = pICFUNC(lpfnGetProcessID);

	/* The actual call to the function contained in the dll */
	int intMyReturnVal = MyFunction(GetConsoleWindow(), -1, 1);
	/* Release the Dll */
	FreeLibrary(hGetProcIDDLL);

	/* The return val from the dll */
	return intMyReturnVal;
}





int CallMyDLL1(void)
{
	/* get handle to dll */
	HINSTANCE hGetProcIDDLL = LoadLibrary("C:\\PSKCore.dll");

	/* get pointer to the function in the dll*/
	FARPROC lpfnGetProcessID = GetProcAddress(HMODULE(hGetProcIDDLL), "fnSendTXCharacter");

	/*
	Define the Function in the DLL for reuse. This is just prototyping the dll's function.
	A mock of it. Use "stdcall" for maximum compatibility.
	*/
	typedef long(__stdcall * pICFUNC)(long txchar, long cntrl);


	pICFUNC MyFunction;
	MyFunction = pICFUNC(lpfnGetProcessID);

	/* The actual call to the function contained in the dll */
	string test = "Yolo";
	int intMyReturnVal = MyFunction('d', 0);

	/* Release the Dll */
	FreeLibrary(hGetProcIDDLL);

	/* The return val from the dll */
	return intMyReturnVal;
}



int main()
{
//	cout << CallMyDLL();
	CallMyDLL1();
}
