/*


*/
/*
     File:  tsync.cpp
*/

#define _WIN32_WINNT 0x0400
#include "g:\vs\include\windows.h"
#include <stdio.h>
#include <winsock2.h>
#include <conio.h>
#include <time.h>

#pragma comment(linker, "/opt:NOWIN98")

template <class T>
VOID ReverseBytes(T &x);

struct NTPDATA {
	DWORD BitField;
	DWORD RootDelay;
	DWORD RootDispersion;
	DWORD RefClockId;
	ULONGLONG RefStamp;
	ULONGLONG OrigStamp;
	ULONGLONG RcvStamp;
	ULONGLONG XmitStamp;
};

BOOL bTimeWasSet = FALSE, bLog = FALSE, bNoErrMsg = FALSE, bWinsock = FALSE;

CHAR szMyLog[MAX_PATH];

VOID ErrorCleanup(VOID);

VOID SetTime(LONGLONG);

VOID Verbose(SYSTEMTIME&, SYSTEMTIME&, SYSTEMTIME&, SYSTEMTIME&);

VOID Log(CHAR *);

VOID ErrorMsg(CHAR *, BOOL);

/* messages */
#define MSG_SYNTAX "Syntax: MYTSYNC hostname | ip_address [auto] [verbose] [noset] [noerrmsg] [log]\n"
#define MSG_NO_WINSOCK "WSAStartup failed\n"
#define MSG_NO_SOCKET "invalid socket\n"
#define MSG_NO_HOSTNAME "gethostbyname failed: %s\n"
#define MSG_CONNECT_ERROR "connect error\n"
#define MSG_NO_RESPONSE "The server did not respond\n"
#define MSG_NOT_SERVER "Host did not identify itself as a server.\n"
#define MSG_SERVER_NOT_SYNCED "Warning: Server said it was not synchronized.\n"

INT main (INT argc, CHAR *argv[]) {

	SOCKET s;
	SOCKADDR_IN sa;
	CHAR cResponse, szLogMsg[MAX_PATH];
	in_addr address = {0};
	struct hostent *hostinfo;
	BOOL bAutoSet = FALSE, bVerbose = FALSE, bNoSet = FALSE;
	INT i;
	float fDelay, fOffset;
	SYSTEMTIME stSnd, stRcv, stServerRcv, stServerSnd, st1900 = {1900,1,0,1,0,0,0,0};
	LONGLONG tSnd, tRcv, t1900, tServerRcv, tServerSnd,
		t100nsecDelay, t100nsecOffset, PerfFreq, PerfCount1, PerfCount2;
	/* some outgoing is constant */
	NTPDATA PktOut = {0x0008001B, 0, 0x00000100, 0}, PktIn = {0};

	if ( argc < 2 || !strcmp(argv[1], "/?") ) {
		fprintf(stderr, MSG_SYNTAX);
		return 1;
	}

	for (i=2; i<argc; i++) {
		if ( !stricmp(argv[i], "auto") ) bAutoSet = TRUE;
		if ( !stricmp(argv[i], "verbose") ) bVerbose = TRUE;
		if ( !stricmp(argv[i], "noerrmsg") ) bNoErrMsg = TRUE;
		if ( !stricmp(argv[i], "noset") ) bNoSet = TRUE;
		if ( !stricmp(argv[i], "log") ) bLog = TRUE;
	}

	GetModuleFileName(NULL, szMyLog, MAX_PATH);
	*(strrchr(szMyLog, '\\')+1) = 0;
	strcat(szMyLog, "tsync.log");

	/* this will be needed; NTP times are relative to 1/1/1900 00:00:00 */
	SystemTimeToFileTime(&st1900, (FILETIME*) &t1900);

	/* fire up Winsock */
	WORD wVersionRequested = MAKEWORD(2,0); 
	WSADATA wsaData; 
	if ( WSAStartup(wVersionRequested, &wsaData) ) ErrorMsg(MSG_NO_WINSOCK, TRUE);

	bWinsock = TRUE;

	QueryPerformanceFrequency((LARGE_INTEGER*) &PerfFreq);

	/* create the socket */
	if ( (s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET )
		ErrorMsg(MSG_NO_SOCKET, TRUE);

	/* figure out the host */
	if ( (address.S_un.S_addr = inet_addr(argv[1])) == INADDR_NONE ) {
		if ( (hostinfo = gethostbyname(argv[1])) == NULL ) {
			sprintf(szLogMsg, MSG_NO_HOSTNAME, argv[1]);
			ErrorMsg(szLogMsg, TRUE);
		}
		address = *((struct in_addr *) hostinfo->h_addr_list[0]);
	}

	/* where ... how to connect */
	memset(&sa, 0, sizeof(sa));
	memcpy(&(sa.sin_addr), &address, 4);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(123); /* 123 = NTP */

	/* connect to the socket */
	if ( connect(s, (SOCKADDR*) &sa, sizeof(SOCKADDR)) == SOCKET_ERROR )
		ErrorMsg(MSG_CONNECT_ERROR, TRUE);

	/* get current time and put it into the outgoing packet */
	GetSystemTime(&stSnd);
	SystemTimeToFileTime(&stSnd, (FILETIME*) &tSnd);
	/* seconds from 1/1/1900 at 00:00:00 */
	tSnd = tSnd - t1900;
	/* high 4 bytes is integer part; low 4 bytes is fractional part (denom = 16^8) */
	/* FILETIMEs are in 100 nanosecond units */
	PktOut.RefStamp = ((tSnd/10000000) << 32);	/* fractional part not needed */
	PktOut.OrigStamp = PktOut.RefStamp | (((tSnd % 10000000)*0x100000000)/10000000);
	/* NTP packets use the other ENDIAN */
	ReverseBytes(PktOut.RefStamp);
	ReverseBytes(PktOut.OrigStamp);

	/* send the packet */
	/* use perf counter to get better estimate of local transaction time */
	QueryPerformanceCounter((LARGE_INTEGER *) &PerfCount1);
	send(s, (CHAR*) &PktOut, 48,0);

	/* another way */
	/*WSABUF buf = {48, (CHAR*) &PktOut}; 
	DWORD dwSent;
	WSASend(s, &buf, 1, &dwSent, 0, NULL, NULL);*/

	/* if this select fails, the server didn't respond */
	WSAEVENT done = WSACreateEvent();
	WSAEventSelect(s, done, FD_READ);
	DWORD rc = WSAWaitForMultipleEvents(1, &done, FALSE, 3000, FALSE);
	WSACloseEvent(done);

	if ( rc == WSA_WAIT_TIMEOUT ) {
		closesocket(s);
		ErrorMsg(MSG_NO_RESPONSE, TRUE);
	}

	else recv(s, (CHAR*) &PktIn, 48, 0);

	/* another way to do the timeout???????????????????????????????????????????? */
	/*FD_SET fd_set;
	fd_set.fd_count = 1;
	fd_set.fd_array[0] = s;
	TIMEVAL tv; tv.tv_sec = 3; tv.tv_usec = 0;
	while ( select(s, &fd_set, NULL, NULL, &tv) recv(s, (CHAR*) &PktIn, 48, 0);
	?????????????????????????????????????????????????????????????????????????? */

	/* get time of received packet, (ASAP) */
	QueryPerformanceCounter((LARGE_INTEGER *) &PerfCount2);
	tRcv = tSnd + ((PerfCount2 - PerfCount1)*10000000)/PerfFreq + t1900;
	FileTimeToSystemTime((FILETIME*) &tRcv, &stRcv);
	tRcv -= t1900;
	/*GetSystemTime(&stRcv);
	SystemTimeToFileTime(&stRcv, (FILETIME*)&tRcv);
	tRcv = tRcv - t1900;*/

	closesocket(s);
	WSACleanup();
	bWinsock = FALSE;

	/* small validity check */
	if ( (PktIn.BitField & 0x3F) != 0x1C ) ErrorMsg(MSG_NOT_SERVER, TRUE);
	/* decode server's receive time? */
	ReverseBytes(PktIn.RcvStamp);
	tServerRcv = (PktIn.RcvStamp >> 32)*10000000
		+ ((PktIn.RcvStamp & 0x00000000FFFFFFFF)*10000000)/0x100000000
		+ t1900; /* local units */
	FileTimeToSystemTime((FILETIME*)&tServerRcv, &stServerRcv);
	tServerRcv -= t1900; /* back to NTP units */

	/* decode server's transmit time */
	ReverseBytes(PktIn.XmitStamp);
	tServerSnd = (PktIn.XmitStamp >> 32)*10000000
		+ ((PktIn.XmitStamp & 0x00000000FFFFFFFF)*10000000)/0x100000000
		+ t1900; /* local units */
	FileTimeToSystemTime((FILETIME*)&tServerSnd, &stServerSnd);
	tServerSnd -= t1900; /* back to NTP units */

	/* compute delay and time offset */
	/* in 100-nanosecond units */
	t100nsecDelay = (tRcv - tSnd) - (tServerSnd - tServerRcv);
	t100nsecOffset = ((tRcv + tSnd) - (tServerRcv + tServerSnd))/2;
	/* in seconds (float) */
	fDelay = (float) (LONGLONG) t100nsecDelay/10000000;
	fOffset = (float) t100nsecOffset/10000000;

	/* not auto; print the info; query whether to set time */
	if ( !bAutoSet ) {
		printf("Time server: %s (%s)\n\n", argv[1], inet_ntoa(address));
		if ( (PktIn.BitField & 0xC0) == 0xC0 ) ErrorMsg(MSG_SERVER_NOT_SYNCED, FALSE);
		if ( bVerbose ) Verbose(stSnd, stServerRcv, stServerSnd, stRcv);
		printf("Estimated round trip time: %3.f ms\n\n", fDelay*1000);
		printf("Estimated local time offset: %+3.3f seconds (%I64d * 100 nanoseconds)\n\n",
			fOffset, t100nsecOffset);
		if ( !bNoSet ) {
			printf("Do you wish to set the clock? (Y/N) ... ");
			if ( (cResponse = getch()) == 'y' || cResponse == 'Y') SetTime(t100nsecOffset);
		}
	}

	else if ( !bNoSet && fOffset < 5.0 && fOffset > -5.0 ) SetTime(t100nsecOffset);

	if ( bLog && bTimeWasSet ) {
		sprintf(szLogMsg, "Time corrected by %3.3f s    Round trip delay = %.f ms\n",
			-fOffset, fDelay*1000);
		Log(szLogMsg);
	}

	/* see ya! */
    return 0; 
}

VOID ErrorCleanup(VOID) {
	if ( bWinsock ) WSACleanup();
	ExitProcess(1);
}

VOID SetTime(LONGLONG t100nsecOffset) {
	SYSTEMTIME stDecide;
	ULONGLONG tDecide;
	GetSystemTime(&stDecide);
	SystemTimeToFileTime(&stDecide, (FILETIME*) &tDecide);
	tDecide -= t100nsecOffset;
	FileTimeToSystemTime((FILETIME*) &tDecide, &stDecide);
	SetSystemTime(&stDecide);
	bTimeWasSet = TRUE;
}

VOID Verbose(SYSTEMTIME &t1, SYSTEMTIME &t2, SYSTEMTIME &t3, SYSTEMTIME &t4) {
	printf("Client send time: (UTC) %4.4d/%2.2d/%2.2d  %2.2d:%2.2d:%2.2d.%3.3d\n",
		t1.wYear, t1.wMonth, t1.wDay,
		t1.wHour, t1.wMinute, t1.wSecond, t1.wMilliseconds);
	printf("Server recv time: (UTC) %4.4d/%2.2d/%2.2d  %2.2d:%2.2d:%2.2d.%3.3d\n",
		t2.wYear, t2.wMonth, t2.wDay,
		t2.wHour, t2.wMinute, t2.wSecond, t2.wMilliseconds);
	printf("Server send time: (UTC) %4.4d/%2.2d/%2.2d  %2.2d:%2.2d:%2.2d.%3.3d\n",
		t3.wYear, t3.wMonth, t3.wDay,
		t3.wHour, t3.wMinute, t3.wSecond, t3.wMilliseconds);
	printf("Client recv time: (UTC) %4.4d/%2.2d/%2.2d  %2.2d:%2.2d:%2.2d.%3.3d\n\n", 
		t4.wYear, t4.wMonth, t4.wDay,
		t4.wHour, t4.wMinute, t4.wSecond, t4.wMilliseconds);
}

template < class T >
VOID ReverseBytes(T &x) {
	BYTE tmp, *p = (BYTE*) &x;
	INT i, iLastByte = sizeof(T)-1;
	for ( i=0; i<(sizeof(T)/2); i++ ) {
		tmp = *(p+i);
		*(p+i) = *(p+iLastByte-i);
		*(p+iLastByte-i) = tmp;
	}
}

VOID Log(CHAR *szMsg) {
	time_t t;
	CHAR szOut[MAX_PATH];
	strcpy(szOut, ctime(&(t=time(NULL)))+4);
	szOut[20] = 0;
	strcat(szOut, "    ");
	strcat(szOut, szMsg);
	FILE *f = fopen(szMyLog, "at");
	fwrite(szOut, strlen(szOut), 1, f);
	fclose(f);
}

VOID ErrorMsg(CHAR *szMsg, BOOL bQuit) {
	if ( !bNoErrMsg ) fprintf(stderr, szMsg);
	if ( bLog ) Log(szMsg);
	if ( !bQuit ) return;
	ErrorCleanup();
}

