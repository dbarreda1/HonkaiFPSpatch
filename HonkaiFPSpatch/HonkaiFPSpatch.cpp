// HonkaiFPSpatch.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "HonkaiFPSpatch.h"
#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
using namespace std;

constexpr auto MAX_LOADSTRING = 100;
constexpr auto PATCH_COMMAND = 1;
constexpr auto RESTORE_COMMAND = 2;
constexpr LPCTSTR HONKAI_SUBKEY = L"Software\\Cognosphere\\Star Rail";
constexpr LPCTSTR name = L"GraphicsSettings_Model_h2986158309";
constexpr auto backupFile = "settingsBackup.bak";


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING] = _T("HonkaiSR FPS patch");                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];       // the main window class name
SHORT winHeight = 190;
SHORT winWidth = 300;
HWND restoreButton;
HWND patchButton;
HWND infoLabel;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
VOID patchReg();
VOID restoreReg();
BOOLEAN fileExists();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HONKAIFPSPATCH, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HONKAIFPSPATCH));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HONKAIFPSPATCH));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HONKAIFPSPATCH);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED|WS_SYSMENU,
      CW_USEDEFAULT, 0, winWidth, winHeight, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   SetMenu(hWnd, NULL);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            cout << wmId;
            // Parse the menu selections:
            switch (wmId)
            {
            case PATCH_COMMAND:
                patchReg();
                break;
            case RESTORE_COMMAND:
                restoreReg();
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            infoLabel = CreateWindowW(
                L"Static", 
                L"Ready.", 
                WS_CHILD | WS_VISIBLE| SS_CENTER, 
                0, 15, winWidth-20, 55, 
                hWnd, 
                0, 
                NULL, 
                0
            );


            patchButton = CreateWindow(
                L"BUTTON",
                L"Patch",
                BS_FLAT | WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                35, winHeight - 100, 100, 30,
                hWnd,
                (HMENU)PATCH_COMMAND,
                (HINSTANCE)GetWindowLongPtr(hWnd, 0),
                NULL
            );

            restoreButton = CreateWindow(
                L"BUTTON",
                L"Restore",
                BS_FLAT | WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                150, winHeight - 100, 100, 30,
                hWnd,
                (HMENU) RESTORE_COMMAND,
                (HINSTANCE)GetWindowLongPtr(hWnd, 0),
                NULL
            );

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_CTLCOLORSTATIC: {
        HBRUSH hbrBkgnd = NULL;
        HDC hdcStatic = (HDC)wParam;
        SetBkColor(hdcStatic, RGB(255, 255, 255));
        if (hbrBkgnd == NULL)
        {
            hbrBkgnd = CreateSolidBrush(RGB(255,255, 255));
        }
        return (INT_PTR)hbrBkgnd;
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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

void patchReg() {
    HKEY key;
    unsigned char byteData[255]{};
    unsigned char finalByteData[255]{};
    DWORD size = sizeof(byteData);
    DWORD dataType;
    HWND hwnd = FindWindowA(NULL, "Honkai: Star Rail");
    if (hwnd != NULL) {
        DWORD procID = NULL;
        DWORD id = GetWindowThreadProcessId(hwnd, &procID);
        HANDLE starRail = OpenProcess(PROCESS_TERMINATE, TRUE, procID);
        TerminateProcess(starRail, 1);
        CloseHandle(starRail);
        CloseHandle(hwnd);
        SetWindowText(infoLabel, L"App running, closing now.");
        Sleep(1000);
    }
    
    //Open Registry Key
    if (RegOpenKeyExW(HKEY_CURRENT_USER, HONKAI_SUBKEY, 0, KEY_READ | KEY_WOW64_64KEY | KEY_WRITE, &key) == ERROR_SUCCESS) {
        RegGetValueW(key, NULL, name, RRF_RT_ANY, &dataType, byteData, &size); //get current value
        std::string byteDataStr(byteData, byteData + sizeof byteData / sizeof byteData[0]);//convert to string

        //check if backup has been made, if not create it.
        if (!fileExists()) {
            ofstream backupReg;
            backupReg.open("settingsBackup.bak");
            backupReg << byteDataStr;
            backupReg.close();
        }
        size_t pos = byteDataStr.find("\"FPS\":60");
        if (pos != string::npos) {
            byteDataStr.replace(byteDataStr.find("\"FPS\":60"), sizeof("\"FPS\":60") - 1, "\"FPS\":120"); //apply fps change.
            strcpy_s((char*)finalByteData, 255, byteDataStr.c_str()); //convert data back to be written.
            if (RegSetValueEx(key, name, NULL, REG_BINARY, finalByteData, size) == ERROR_SUCCESS) {
                SetWindowText(infoLabel, L"patch applied, press restore to revert settings.");
            }
        }
        else {
            SetWindowText(infoLabel, L"patch already applied.");
        }

    }
    RegCloseKey(key);
}

void restoreReg() {

    if (fileExists()) {
        ifstream backupReg(backupFile);
        HKEY key;
        UCHAR data[255]{};
        std::string backupData(std::istreambuf_iterator<char>{backupReg}, {}); //backup file contentst -> string.
        //open registry key
        if (RegOpenKeyExW(HKEY_CURRENT_USER, HONKAI_SUBKEY, 0, KEY_READ | KEY_WOW64_64KEY | KEY_WRITE, &key) == ERROR_SUCCESS) {
            strcpy_s((char*)data, 255, backupData.c_str());
            if (RegSetValueEx(key, name, NULL, REG_BINARY, data, sizeof(data)) == ERROR_SUCCESS) { //write data pulled from backup.
                SetWindowText(infoLabel, L"Backup Restored.");
            }
            else {
                SetWindowText(infoLabel, L"Error writing backup.");
            }
        }
        else {
            SetWindowText(infoLabel, L"Error opening settings.");
        }
        RegCloseKey(key);
    }
    else {
        SetWindowText(infoLabel, L"No backup to restore.");
    }
    
}

BOOLEAN fileExists() {
    struct stat buf;
    return stat(backupFile, &buf) != -1;
}