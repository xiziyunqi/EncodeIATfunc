
#include "stdafx.h"
#include "SetHardWare.h"
typedef 
ULONG
(NTAPI 
*PRtlNtStatusToDosError)(
   ULONG Status
   );
//#include "../../testbase/Memory/Memory.h"
//PRtlNtStatusToDosError RtlNtStatusToDosError=(PRtlNtStatusToDosError)::GetProcAddress(::GetModuleHandle(L"Ntdll.dll"),"RtlNtStatusToDosError");
#include "../../BPHookFunction/BPHookFunction/BPHookFunction.h"
//#include "../../testbase/Memory/Memory.h"
#define MAX_LOADSTRING 100
typedef ULONG (WINAPI *pfnRtlDispatchException)(PEXCEPTION_RECORD pExcptRec,CONTEXT * pContext);
pfnRtlDispatchException m_fnRtlDispatchException=(pfnRtlDispatchException)::GetProcAddress(::GetModuleHandleA("ntdll"),"KiUserExceptionDispatcher");;
// ȫ�ֱ���:
HINSTANCE hInst;                                                                // ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];                                        // �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];                        // ����������
int APIENTRY _tWinMain2(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow);
// �˴���ģ���а����ĺ�����ǰ������:
ATOM                                MyRegisterClass(HINSTANCE hInstance);
BOOL                                InitInstance(HINSTANCE, int);
LRESULT CALLBACK        WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK        About(HWND, UINT, WPARAM, LPARAM);
int APIENTRY _tWinMain2(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow);
LONG WINAPI MyTopUnhandledExceptionFilter(
         struct _EXCEPTION_POINTERS *ExceptionInfo
);
                //  extern "C" __declspec(dllimport) ULONG NTAPI  RtlNtStatusToDosError(
//  _In_ LONG Status
//);
BOOL RtlDispatchException(PEXCEPTION_RECORD pExcptRec,CONTEXT * pContext)
{
  // ����TRUE,����쳣���Ѿ��������,�������г���
  // ����FALSE,����쳣�����ҵ�,�ұ��˴���ȥ
        MessageBox(0,L"DDDD",L"DDSADS",0);
        return 1;
}
ULONG WINAPI _RtlDispatchException( PEXCEPTION_RECORD pExcptRec,CONTEXT * pContext )
{
  if(RtlDispatchException(pExcptRec,pContext)) return 1;
  return m_fnRtlDispatchException(pExcptRec,pContext);
}
BOOL HookSystemSEH()
{
        m_fnRtlDispatchException=(pfnRtlDispatchException)::GetProcAddress(::GetModuleHandleA("ntdll"),"KiUserExceptionDispatcher");
        return 0;
//        return SetInlineHook((PVOID*)&m_fnRtlDispatchException,_RtlDispatchException);
}
DWORD Address = 0,EndAddress = 0;
LPVOID Data = 0;
DWORD IsCall = 0;
HANDLE VectoredHandler = 0;
VOID WINAPI RegFilter()
{
        PVOID Data = (PVOID)Address;
        if(RtlNtStatusToDosError==0) exit(0);
#ifndef _DEBUG
        if(::IsDebuggerPresent()) return ;
#else

#endif
        VectoredHandler=AddVectoredExceptionHandler(1,MyTopUnhandledExceptionFilter);
        if(VectoredHandler==0) exit(0);
}
int WINAPI Show()
{
        return ::MessageBox(0,L"�Ҳ��Ȼ�е��������������������",L"������",16);
}
__declspec(naked)int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
        _asm
        {
                mov edi,edi//WINAPI Լ�����÷�ʽ �����һ���ָ���ʵ����һ����˼������jmp _tWinMain2
                push ebp 
                        mov ebp, esp
         push eax
         lea eax,dword ptr[Error]
         mov Address,eax
         lea eax,dword ptr[End]
         mov EndAddress,eax
         mov eax,110
         xor ebx ,ebx
         pop eax
        mov esp,ebp
                pop ebp                                
                 mov eax,110
        push eax
                        push dword ptr[Next]
                push RegFilter
                ret
        
Next:                pop eax
        Error:        _asm idiv ebx 
                        call Show
                        ret 16
End:
                
                #ifdef _DEBUG
                ret 16
        #else
                call dword ptr[DebugBreak]
                push Error+2
                #endif
                ret
                        ret 16
        }
}
LONG WINAPI MyTopUnhandledExceptionFilter(
         struct _EXCEPTION_POINTERS *ExceptionInfo
)
                 {
                         PVECTORED_EXCEPTION_HANDLER Handler=(PVECTORED_EXCEPTION_HANDLER)VectoredHandler;
                //         return Handler(ExceptionInfo);
                         WCHAR Buffer[32]={0};
                        DWORD  ExceptionCode=RtlNtStatusToDosError(ExceptionInfo->ExceptionRecord->ExceptionCode);
                //        ExceptionCode=::BaseNtStatusToDosError(ExceptionCode);
                        //_asm int 3 c0000094
                        //wsprintf(Buffer,L"%x",ExceptionCode);
                        //MessageBox(0,Buffer,L"Text",0);
                        //if(IsCall==1)
                        if(IsCall==2) return 0;
                        if(ExceptionInfo->ExceptionRecord->ExceptionCode==0x80000003)
                        {
                                if(ExceptionInfo->ContextRecord->Eax==120)
                                //if(EndAddress==(DWORD)ExceptionInfo->ExceptionRecord->ExceptionAddress)
                                {
                                                        ExceptionInfo->ContextRecord->Eax=119;
                                                        ExceptionInfo->ContextRecord->Esp=ExceptionInfo->ContextRecord->Esp+4;
                                                        ExceptionInfo->ContextRecord->Eip=(DWORD)Data;
                                                        return EXCEPTION_CONTINUE_EXECUTION;
                                }
                                return 0;
                        }
                        if(ExceptionCode==998)
                        {
                                //if(ExceptionInfo->ExceptionRecord->ExceptionAddress==(PVOID)Data)
                        //        {
                        //if(IsCall==2) return 0;

                        if(ExceptionInfo->ExceptionRecord->ExceptionAddress==Data)
                        {
                                memset(Data,0,16);
                                VirtualFree(Data,16,MEM_DECOMMIT);
                                        if(ExceptionInfo->ContextRecord->Eax==119)
                                        {
                                                IsCall=2;
                                                ExceptionInfo->ContextRecord->Eip=(DWORD)_tWinMain2;
                                                return EXCEPTION_CONTINUE_EXECUTION;
                                        }
                        }
                        //        }
                        }
                        if(ExceptionInfo->ExceptionRecord->ExceptionCode==0xc0000094)
                        {
                                //ExceptionInfo->ExceptionRecord->
                                if(ExceptionInfo->ExceptionRecord->ExceptionAddress==(PVOID)Address)
                                {
                                        if(ExceptionInfo->ContextRecord->Eax==110)
                                        { 
                                                 Data=VirtualAlloc(0,16,MEM_COMMIT, PAGE_READWRITE);
                                                 //::IsDebuggerPresent()
                                                 BOOL IsDebug= ::IsDebuggerPresent();
                                                #ifndef _DEBUG
                           //  if(::IsDebuggerPresent()) return ;
                          #else
                                                 IsDebug=FALSE;
                           #endif
                                                 if(IsDebug)
                                                 {
                                                         #ifndef _DEBUG
                                                ExceptionInfo->ContextRecord->Eip=ExceptionInfo->ContextRecord->Eip+2;
                                                #else
                                                         ExceptionInfo->ContextRecord->Eip=(DWORD)_tWinMain2;
                                                         #endif
                                                //return EXCEPTION_CONTINUE_EXECUTION;
                                                         RemoveVectoredExceptionHandler(VectoredHandler);
                                                        return EXCEPTION_CONTINUE_EXECUTION;
                                                 }
                                                 #ifdef _DEBUG
                                                 ExceptionInfo->ContextRecord->Eip=(DWORD)_tWinMain2;
                                                 RemoveVectoredExceptionHandler(VectoredHandler);
                                                 return EXCEPTION_CONTINUE_EXECUTION;
                                                 #endif
                                                if(Data==0)
                                                {
                                                        //exit(0);
                                                        ExceptionInfo->ContextRecord->Eip=ExceptionInfo->ContextRecord->Eip+2;
                                                }else
                                                {
                                                        CONTEXT lpContext={0};
                                                        IsCall=1;
                                                        memset(Data,0xcc,16);
                                                        ExceptionInfo->ContextRecord->Eax=120;
                                                        ExceptionInfo->ContextRecord->Eip=EndAddress;
                                                }
                                        }
                                }
                                return EXCEPTION_CONTINUE_EXECUTION;
                        }
                         return 0;
                 }
int APIENTRY _tWinMain2(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
        UNREFERENCED_PARAMETER(hPrevInstance);
        UNREFERENCED_PARAMETER(lpCmdLine);

        // TODO: �ڴ˷��ô��롣
//        _asm int  3
        MSG msg;
        HACCEL hAccelTable;

        // ��ʼ��ȫ���ַ���
        LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
        LoadString(hInstance, IDC_SETHARDWARE, szWindowClass, MAX_LOADSTRING);
        MyRegisterClass(hInstance);
        
        // ִ��Ӧ�ó����ʼ��:
        if (!InitInstance (hInstance, nCmdShow))
        {
                return FALSE;
        }

        hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SETHARDWARE));

        // ����Ϣѭ��:
        while (GetMessage(&msg, NULL, 0, 0))
        {
                if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
                {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                }
        }

        return (int) msg.wParam;
}

//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style                        = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc        = WndProc;
        wcex.cbClsExtra                = 0;
        wcex.cbWndExtra                = 0;
        wcex.hInstance                = hInstance;
        wcex.hIcon                        = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SETHARDWARE));
        wcex.hCursor                = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground        = (HBRUSH)(COLOR_WINDOW+1);
        wcex.lpszMenuName        = MAKEINTRESOURCE(IDC_SETHARDWARE);
        wcex.lpszClassName        = szWindowClass;
        wcex.hIconSm                = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

        return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND        - ����Ӧ�ó���˵�
//  WM_PAINT        - ����������
//  WM_DESTROY        - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
        int wmId, wmEvent;
        PAINTSTRUCT ps;
        HDC hdc;

        switch (message)
        {
        case WM_COMMAND:
                wmId    = LOWORD(wParam);
                wmEvent = HIWORD(wParam);
                // �����˵�ѡ��:
                switch (wmId)
                {
                case IDM_ABOUT:
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                        break;
                case IDM_EXIT:
                        DestroyWindow(hWnd);
                        break;
                default:
                        return DefWindowProc(hWnd, message, wParam, lParam);
                }
                break;
        case WM_PAINT:
                hdc = BeginPaint(hWnd, &ps);
                // TODO: �ڴ���������ͼ����...
                EndPaint(hWnd, &ps);
                break;
        case WM_DESTROY:
                PostQuitMessage(0);
                break;
        default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
        UNREFERENCED_PARAMETER(lParam);
        switch (message)
        {
        case WM_INITDIALOG:
                return (INT_PTR)TRUE;

        case WM_COMMAND:
                if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
                {
                        EndDialog(hDlg, LOWORD(wParam));
                        return (INT_PTR)TRUE;
                }
                break;
        }
        return (INT_PTR)FALSE;
}

