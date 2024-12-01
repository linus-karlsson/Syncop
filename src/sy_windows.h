#pragma once
#ifndef SYNCOP_UNIT_BUILD
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#endif

#define WM_CREATE 0x0001
#define WM_DESTROY 0x0002
#define WM_MOVE 0x0003
#define WM_SIZE 0x0005
#define WM_CLOSE 0x0010
#define WM_QUIT 0x0012
#define WM_ACTIVATEAPP 0x001C

#define WM_SETCURSOR 0x0020

#define WM_LBUTTONDOWN 0x0201
#define WM_LBUTTONUP 0x0202
#define WM_RBUTTONDOWN 0x0204
#define WM_RBUTTONUP 0x0205
#define WM_MOUSEMOVE 0x0200
#define WM_MOUSEWHEEL 0x020A

#define WM_SYSKEYDOWN 0x0104
#define WM_SYSKEYUP 0x0105

#define WM_KEYDOWN 0x0100
#define WM_KEYUP 0x0101
#define WM_CHAR 0x0102

#define MONITOR_DEFAULTTONULL 0x00000000
#define MONITOR_DEFAULTTOPRIMARY 0x00000001
#define MONITOR_DEFAULTTONEAREST 0x00000002

#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
#define VK_MENU 0x12
#define VK_PAUSE 0x13
#define VK_CAPITAL 0x14

#define VK_BACK 0x08
#define VK_TAB 0x09

#define VK_CLEAR 0x0C
#define VK_RETURN 0x0D

#define GWL_STYLE (-16)

#define MB_OK 0x00000000L

#define WS_OVERLAPPED 0x00000000L
#define WS_POPUP 0x80000000L
#define WS_CHILD 0x40000000L
#define WS_MINIMIZE 0x20000000L
#define WS_VISIBLE 0x10000000L
#define WS_DISABLED 0x08000000L
#define WS_CLIPSIBLINGS 0x04000000L
#define WS_CLIPCHILDREN 0x02000000L
#define WS_MAXIMIZE 0x01000000L
#define WS_CAPTION 0x00C00000L
#define WS_BORDER 0x00800000L
#define WS_DLGFRAME 0x00400000L
#define WS_VSCROLL 0x00200000L
#define WS_HSCROLL 0x00100000L
#define WS_SYSMENU 0x00080000L
#define WS_THICKFRAME 0x00040000L
#define WS_GROUP 0x00020000L
#define WS_TABSTOP 0x00010000L

#define WS_MINIMIZEBOX 0x00020000L
#define WS_MAXIMIZEBOX 0x00010000L

#define WS_TILED WS_OVERLAPPED
#define WS_ICONIC WS_MINIMIZE
#define WS_SIZEBOX WS_THICKFRAME
#define WS_TILEDWINDOW WS_OVERLAPPEDWINDOW

#define WS_OVERLAPPEDWINDOW                                                         \
    (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX |     \
     WS_MAXIMIZEBOX)

#define WS_POPUPWINDOW (WS_POPUP | WS_BORDER | WS_SYSMENU)

#define WS_CHILDWINDOW (WS_CHILD)

#define SWP_NOSIZE 0x0001
#define SWP_NOMOVE 0x0002
#define SWP_NOZORDER 0x0004
#define SWP_NOREDRAW 0x0008
#define SWP_NOACTIVATE 0x0010
#define SWP_FRAMECHANGED 0x0020
#define SWP_SHOWWINDOW 0x0040
#define SWP_HIDEWINDOW 0x0080
#define SWP_NOCOPYBITS 0x0100
#define SWP_NOOWNERZORDER 0x0200
#define SWP_NOSENDCHANGING 0x0400

#define MAX_PATH 260

#define HWND_TOP ((HWND)0)
#define HWND_BOTTOM ((HWND)1)
#define HWND_TOPMOST ((HWND)-1)
#define HWND_NOTOPMOST ((HWND)-2)

#define CW_USEDEFAULT ((int)0x80000000)

#define MAKEINTRESOURCEA(i) ((LPSTR)((ULONG_PTR)((WORD)(i))))
#define MAKEINTRESOURCEW(i) ((LPWSTR)((ULONG_PTR)((WORD)(i))))
#ifdef UNICODE
#define MAKEINTRESOURCE MAKEINTRESOURCEW
#else
#define MAKEINTRESOURCE MAKEINTRESOURCEA
#endif // !UNICODE
#define IDC_ARROW MAKEINTRESOURCE(32512)
#define IDC_IBEAM MAKEINTRESOURCE(32513)
#define IDC_WAIT MAKEINTRESOURCE(32514)
#define IDC_CROSS MAKEINTRESOURCE(32515)
#define IDC_UPARROW MAKEINTRESOURCE(32516)
#define IDC_SIZE MAKEINTRESOURCE(32640)
#define IDC_ICON MAKEINTRESOURCE(32641)
#define IDC_SIZENWSE MAKEINTRESOURCE(32642)
#define IDC_SIZENESW MAKEINTRESOURCE(32643)
#define IDC_SIZEWE MAKEINTRESOURCE(32644)
#define IDC_SIZENS MAKEINTRESOURCE(32645)
#define IDC_SIZEALL MAKEINTRESOURCE(32646)
#define IDC_NO MAKEINTRESOURCE(32648)
#define IDC_HAND MAKEINTRESOURCE(32649)
#define IDC_APPSTARTING MAKEINTRESOURCE(32650)

#if 0
#if (WINVER >= 0x0606)
#define IDC_PIN MAKEINTRESOURCE(32671)
#define IDC_PERSON MAKEINTRESOURCE(32672)
#endif
#endif

#define CS_VREDRAW 0x0001
#define CS_HREDRAW 0x0002
#define CS_DBLCLKS 0x0008
#define CS_OWNDC 0x0020
#define CS_CLASSDC 0x0040
#define CS_PARENTDC 0x0080
#define CS_NOCLOSE 0x0200
#define CS_SAVEBITS 0x0800
#define CS_BYTEALIGNCLIENT 0x1000
#define CS_BYTEALIGNWINDOW 0x2000
#define CS_GLOBALCLASS 0x4000

#define CS_IME 0x00010000
#if 0
#if (_WIN32_WINNT >= 0x0501)
#define CS_DROPSHADOW 0x00020000
#endif /* _WIN32_WINNT >= 0x0501 */
#endif

#define LOWORD(l) ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l) ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define GET_WHEEL_DELTA_WPARAM(wParam) ((short)HIWORD(wParam))

#define PM_NOREMOVE 0x0000
#define PM_REMOVE 0x0001
#define PM_NOYIELD 0x0002

#define FILE_READ_DATA (0x0001)      // file & pipe
#define FILE_LIST_DIRECTORY (0x0001) // directory

#define FILE_WRITE_DATA (0x0002) // file & pipe
#define FILE_ADD_FILE (0x0002)   // directory

#define FILE_APPEND_DATA (0x0004)          // file
#define FILE_ADD_SUBDIRECTORY (0x0004)     // directory
#define FILE_CREATE_PIPE_INSTANCE (0x0004) // named pipe

#define FILE_READ_EA (0x0008) // file & directory

#define FILE_WRITE_EA (0x0010) // file & directory

#define FILE_EXECUTE (0x0020)  // file
#define FILE_TRAVERSE (0x0020) // directory

#define FILE_DELETE_CHILD (0x0040) // directory

#define FILE_READ_ATTRIBUTES (0x0080) // all

#define FILE_WRITE_ATTRIBUTES (0x0100) // all

#define FILE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x1FF)

#define DELETE (0x00010000L)
#define READ_CONTROL (0x00020000L)
#define WRITE_DAC (0x00040000L)
#define WRITE_OWNER (0x00080000L)
#define SYNCHRONIZE (0x00100000L)

#define STANDARD_RIGHTS_REQUIRED (0x000F0000L)

#define STANDARD_RIGHTS_READ (READ_CONTROL)
#define STANDARD_RIGHTS_WRITE (READ_CONTROL)
#define STANDARD_RIGHTS_EXECUTE (READ_CONTROL)

#define STANDARD_RIGHTS_ALL (0x001F0000L)

#define SPECIFIC_RIGHTS_ALL (0x0000FFFFL)

#define FILE_GENERIC_READ                                                           \
    (STANDARD_RIGHTS_READ | FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_READ_EA |  \
     SYNCHRONIZE)

#define FILE_GENERIC_WRITE                                                          \
    (STANDARD_RIGHTS_WRITE | FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES |              \
     FILE_WRITE_EA | FILE_APPEND_DATA | SYNCHRONIZE)

#define FILE_GENERIC_EXECUTE                                                        \
    (STANDARD_RIGHTS_EXECUTE | FILE_READ_ATTRIBUTES | FILE_EXECUTE | SYNCHRONIZE)

#define FILE_BEGIN 0
#define FILE_CURRENT 1
#define FILE_END 2

#define GENERIC_READ (0x80000000L)
#define GENERIC_WRITE (0x40000000L)
#define GENERIC_EXECUTE (0x20000000L)
#define GENERIC_ALL (0x10000000L)

#define FILE_SHARE_READ 0x00000001
#define FILE_SHARE_WRITE 0x00000002
#define FILE_SHARE_DELETE 0x00000004

#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define TRUNCATE_EXISTING 5

#define PAGE_READWRITE 0x04
#define MEM_COMMIT 0x00001000
#define MEM_RESERVE 0x00002000
#define MEM_RELEASE 0x00008000

#define FILE_NOTIFY_CHANGE_LAST_WRITE 0x00000010

#define TRUE 1
#define FALSE 0

#define DECLARE_HANDLE(name)                                                        \
    struct name##__                                                                 \
    {                                                                               \
        int unused;                                                                 \
    };                                                                              \
    typedef struct name##__* name

DECLARE_HANDLE(HINSTANCE);
DECLARE_HANDLE(HCURSOR);
DECLARE_HANDLE(HWND);
DECLARE_HANDLE(HICON);
DECLARE_HANDLE(HBRUSH);
DECLARE_HANDLE(HMONITOR);
DECLARE_HANDLE(HMENU);

typedef __int64 INT_PTR, *PINT_PTR;
typedef unsigned __int64 UINT_PTR, *PUINT_PTR;

typedef __int64 LONG_PTR, *PLONG_PTR;
typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;

typedef __int64 LONGLONG;

#define __int3264 __int64

typedef unsigned int UINT;

typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;

typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;

typedef char* LPSTR;
typedef unsigned long DWORD;
typedef long LONG;
typedef int INT;
typedef int BOOL;
typedef short SHORT;
typedef unsigned short WORD;
typedef size_t SIZE_T;

typedef void *LPVOID, *PVOID, VOID;
typedef LONG* PLONG;
typedef DWORD* LPDWORD;
typedef const void* LPCVOID;

typedef const char *LPCSTR, *PCSTR;
typedef const wchar_t* LPCWSTR;

typedef WORD ATOM;

typedef PVOID HANDLE;

#define WINAPI __stdcall
#define CALLBACK __stdcall

typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

typedef struct tagWNDCLASSA
{
    UINT style;
    WNDPROC lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    LPCSTR lpszMenuName;
    LPCSTR lpszClassName;
} WNDCLASSA, *PWNDCLASSA, *NPWNDCLASSA, *LPWNDCLASSA;

typedef struct tagWNDCLASSW
{
    UINT style;
    WNDPROC lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    LPCWSTR lpszMenuName;
    LPCWSTR lpszClassName;
} WNDCLASSW, *PWNDCLASSW, *NPWNDCLASSW, *LPWNDCLASSW;

#ifdef UNICODE
typedef WNDCLASSW WNDCLASS;
typedef PWNDCLASSW PWNDCLASS;
typedef NPWNDCLASSW NPWNDCLASS;
typedef LPWNDCLASSW LPWNDCLASS;
#else
typedef WNDCLASSA WNDCLASS;
typedef PWNDCLASSA PWNDCLASS;
typedef NPWNDCLASSA NPWNDCLASS;
typedef LPWNDCLASSA LPWNDCLASS;
#endif // UNICODE

typedef struct tagRECT
{
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT;

typedef struct tagMONITORINFO
{
    DWORD cbSize;
    RECT rcMonitor;
    RECT rcWork;
    DWORD dwFlags;
} MONITORINFO, *LPMONITORINFO;

typedef struct tagPOINT
{
    LONG x;
    LONG y;
} POINT, *PPOINT, *NPPOINT, *LPPOINT;

typedef struct tagWINDOWPLACEMENT
{
    UINT length;
    UINT flags;
    UINT showCmd;
    POINT ptMinPosition;
    POINT ptMaxPosition;
    RECT rcNormalPosition;
} WINDOWPLACEMENT;
typedef WINDOWPLACEMENT *PWINDOWPLACEMENT, *LPWINDOWPLACEMENT;

typedef struct tagMSG
{
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    POINT pt;
} MSG, *PMSG, *NPMSG, *LPMSG;

typedef LONGLONG USN;

#if defined(MIDL_PASS)
typedef struct _LARGE_INTEGER
{
    LONGLONG QuadPart;
} LARGE_INTEGER;
#else  // MIDL_PASS
typedef union _LARGE_INTEGER
{
    struct
    {
        DWORD LowPart;
        LONG HighPart;
    } DUMMYSTRUCTNAME;
    struct
    {
        DWORD LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER;
#endif // MIDL_PASS

typedef LARGE_INTEGER* PLARGE_INTEGER;

typedef struct _SECURITY_ATTRIBUTES
{
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

typedef struct _OVERLAPPED
{
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    union
    {
        struct
        {
            DWORD Offset;
            DWORD OffsetHigh;
        } DUMMYSTRUCTNAME;
        PVOID Pointer;
    } DUMMYUNIONNAME;

    HANDLE hEvent;
} OVERLAPPED, *LPOVERLAPPED;

void __stdcall ExitProcess(unsigned int uExitCode);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
                   int nShowCmd);

HCURSOR WINAPI LoadCursorA(HINSTANCE hInstance, LPCSTR lpCursorName);
HCURSOR WINAPI LoadCursorW(HINSTANCE hInstance, LPCWSTR lpCursorName);
#ifdef UNICODE
#define LoadCursor LoadCursorW
#else
#define LoadCursor LoadCursorA
#endif // !UNICODE

int WINAPI MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);

HCURSOR WINAPI SetCursor(HCURSOR hCursor);
BOOL WINAPI SetCursorPos(int X, int Y);
BOOL WINAPI GetCursorPos(LPPOINT lpPoint);
BOOL WINAPI ClientToScreen(HWND hWnd, LPPOINT lpPoint);

LRESULT CALLBACK DefWindowProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

#define DefWindowProc DefWindowProcA

LONG WINAPI GetWindowLongA(HWND hWnd, int nIndex);
LONG WINAPI GetWindowLongW(HWND hWnd, int nIndex);
#ifdef UNICODE
#define GetWindowLong GetWindowLongW
#else
#define GetWindowLong GetWindowLongA
#endif // !UNICODE

BOOL WINAPI GetWindowPlacement(HWND hWnd, WINDOWPLACEMENT* lpwndpl);
BOOL WINAPI SetWindowPlacement(HWND hWnd, const WINDOWPLACEMENT* lpwndpl);

BOOL WINAPI GetMonitorInfoA(HMONITOR hMonitor, LPMONITORINFO lpmi);
BOOL WINAPI GetMonitorInfoW(HMONITOR hMonitor, LPMONITORINFO lpmi);
#ifdef UNICODE
#define GetMonitorInfo GetMonitorInfoW
#else
#define GetMonitorInfo GetMonitorInfoA
#endif // !UNICODE

HMONITOR WINAPI MonitorFromWindow(HWND hwnd, DWORD dwFlags);

LONG WINAPI SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong);
LONG WINAPI SetWindowLongW(HWND hWnd, int nIndex, LONG dwNewLong);
#ifdef UNICODE
#define SetWindowLong SetWindowLongW
#else
#define SetWindowLong SetWindowLongA
#endif // !UNICODE

BOOL WINAPI SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx,
                         int cy, UINT uFlags);

BOOL WINAPI SetWindowPlacement(HWND hWnd, const WINDOWPLACEMENT* lpwndpl);

HCURSOR WINAPI LoadCursorA(HINSTANCE hInstance, LPCSTR lpCursorName);
HCURSOR WINAPI LoadCursorW(HINSTANCE hInstance, LPCWSTR lpCursorName);
#ifdef UNICODE
#define LoadCursor LoadCursorW
#else
#define LoadCursor LoadCursorA
#endif // !UNICODE

typedef HINSTANCE HMODULE; /* HMODULEs can be used in place of HINSTANCEs */

HMODULE WINAPI GetModuleHandleA(LPCSTR lpModuleName);
HMODULE WINAPI GetModuleHandleW(LPCWSTR lpModuleName);
#ifdef UNICODE
#define GetModuleHandle GetModuleHandleW
#else
#define GetModuleHandle GetModuleHandleA
#endif // !UNICODE

ATOM WINAPI RegisterClassA(const WNDCLASSA* lpWndClass);
ATOM WINAPI RegisterClassW(const WNDCLASSW* lpWndClass);
#ifdef UNICODE
#define RegisterClass RegisterClassW
#else
#define RegisterClass RegisterClassA
#endif // !UNICODE

HWND WINAPI CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName,
                            DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                            HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
                            LPVOID lpParam);

HWND WINAPI CreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName,
                            LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y,
                            int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                            HINSTANCE hInstance, LPVOID lpParam);
#ifdef UNICODE
#define CreateWindowEx CreateWindowExW
#else
#define CreateWindowEx CreateWindowExA
#endif // !UNICODE

BOOL WINAPI PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin,
                         UINT wMsgFilterMax, UINT wRemoveMsg);

BOOL WINAPI PeekMessageW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin,
                         UINT wMsgFilterMax, UINT wRemoveMsg);
#ifdef UNICODE
#define PeekMessage PeekMessageW
#else
#define PeekMessage PeekMessageA
#endif // !UNICODE

SHORT WINAPI GetKeyState(int nVirtKey);

BOOL WINAPI TranslateMessage(const MSG* lpMsg);

LRESULT WINAPI DispatchMessageA(const MSG* lpMsg);
LRESULT WINAPI DispatchMessageW(const MSG* lpMsg);
#ifdef UNICODE
#define DispatchMessage DispatchMessageW
#else
#define DispatchMessage DispatchMessageA
#endif // !UNICODE

BOOL WINAPI DestroyWindow(HWND hWnd);

int WINAPI MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);

int WINAPI MessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);
#ifdef UNICODE
#define MessageBox MessageBoxW
#else
#define MessageBox MessageBoxA
#endif // !UNICODE

void WINAPI Sleep(DWORD dwMilliseconds);

#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

HANDLE WINAPI CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess,
                          DWORD dwShareMode,
                          LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                          DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
                          HANDLE hTemplateFile);

HANDLE WINAPI CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess,
                          DWORD dwShareMode,
                          LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                          DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
                          HANDLE hTemplateFile);

#ifdef UNICODE
#define CreateFile CreateFileW
#else
#define CreateFile CreateFileA
#endif // !UNICODE

DWORD GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);

BOOL WINAPI GetFileSizeEx(HANDLE hFile, PLARGE_INTEGER lpFileSize);

BOOL WINAPI ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead,
                     LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);

BOOL WINAPI CloseHandle(HANDLE hObject);

DWORD WINAPI SetFilePointer(HANDLE hFile, LONG lDistanceToMove,
                            PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);

BOOL WINAPI WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,
                      LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);

#define InterlockedIncrement _InterlockedIncrement
#define InterlockedExchange _InterlockedExchange
#define InterlockedCompareExchange _InterlockedCompareExchange
#define INFINITE 0xFFFFFFFF // Infinite timeout
#define WINAPI __stdcall
typedef long LONG;
typedef long* LPLONG;
typedef unsigned long DWORD;
typedef DWORD* LPDWORD;
typedef void* HANDLE;
typedef int BOOL;
typedef void *LPVOID, *PVOID;
typedef const char *LPCSTR, *PCSTR;
typedef const wchar_t* LPCWSTR;
typedef DWORD(WINAPI* PTHREAD_START_ROUTINE)(LPVOID lpThreadParameter);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

HANDLE WINAPI CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes,
                           size_t dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress,
                           LPVOID lpParameter, DWORD dwCreationFlags,
                           LPDWORD lpThreadId);

BOOL WINAPI CloseHandle(HANDLE hObject);

void WINAPI Sleep(DWORD dwMilliseconds);

HANDLE WINAPI CreateMutexA(LPSECURITY_ATTRIBUTES lpMutexAttributes,
                           BOOL bInitialOwner, LPCSTR lpName);

HANDLE WINAPI CreateMutexW(LPSECURITY_ATTRIBUTES lpMutexAttributes,
                           BOOL bInitialOwner, LPCWSTR lpName);
#ifdef UNICODE
#define CreateMutex CreateMutexW
#else
#define CreateMutex CreateMutexA
#endif // !UNICODE

DWORD WINAPI WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);

BOOL WINAPI ReleaseMutex(HANDLE hMutex);

HANDLE WINAPI CreateSemaphoreA(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
                               LONG lInitialCount, LONG lMaximumCount,
                               LPCSTR lpName);
#ifndef UNICODE
#define CreateSemaphore CreateSemaphoreA
#endif

BOOL WINAPI ReleaseSemaphore(HANDLE hSemaphore, LONG lReleaseCount,
                             LPLONG lpPreviousCount);

VOID WINAPI OutputDebugStringA(LPCSTR lpOutputString);

VOID WINAPI OutputDebugStringW(LPCWSTR lpOutputString);
#ifdef UNICODE
#define OutputDebugString OutputDebugStringW
#else
#define OutputDebugString OutputDebugStringA
#endif // !UNICODE
       //

LPVOID WINAPI VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType,
                           DWORD flProtect);
BOOL VirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);

BOOL WINAPI FindCloseChangeNotification(HANDLE hChangeHandle);

HANDLE WINAPI FindFirstChangeNotificationA(LPCSTR lpPathName, BOOL bWatchSubtree,
                                           DWORD dwNotifyFilter);

HANDLE WINAPI FindFirstChangeNotificationW(LPCWSTR lpPathName, BOOL bWatchSubtree,
                                           DWORD dwNotifyFilter);
#ifdef UNICODE
#define FindFirstChangeNotification FindFirstChangeNotificationW
#else
#define FindFirstChangeNotification FindFirstChangeNotificationA
#endif // !UNICODE

DWORD GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);

#define GWLP_USERDATA -21
LONG_PTR SetWindowLongPtrA(HWND hWnd, int nIndex, LONG_PTR dwNewLong);
LONG_PTR GetWindowLongPtrA(HWND hWnd, int nIndex);

typedef struct _FILETIME
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

typedef unsigned __int64 ULONGLONG;

typedef union _ULARGE_INTEGER
{
    struct
    {
        DWORD LowPart;
        DWORD HighPart;
    } DUMMYSTRUCTNAME;
    struct
    {
        DWORD LowPart;
        DWORD HighPart;
    };
    ULONGLONG QuadPart;
} ULARGE_INTEGER;

typedef struct _SYSTEM_INFO
{
    union
    {
        DWORD dwOemId;
        struct
        {
            WORD wProcessorArchitecture;
            WORD wReserved;
        };
    };
    DWORD dwPageSize;
    LPVOID lpMinimumApplicationAddress;
    LPVOID lpMaximumApplicationAddress;
    DWORD_PTR dwActiveProcessorMask;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel;
    WORD wProcessorRevision;
} SYSTEM_INFO;

typedef SYSTEM_INFO* LPSYSTEM_INFO;

void WINAPI GetSystemInfo(LPSYSTEM_INFO lpSystemInfo);

void GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime);

BOOL WINAPI DestroyWindow(HWND hWnd);

BOOL WINAPI ShowWindow(HWND hWnd, int nCmdShow);
