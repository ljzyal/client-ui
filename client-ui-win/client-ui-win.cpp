#include "client-ui-win.h"
#include <Commctrl.h>
#include <io.h>
#include <Fcntl.h>

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

HFONT hFont;
HMENU hmenu;
int w = 400;
int h = 250;
IP_ADAPTER_INFO netcard[50];
int netcardnum = 0;
HWND button, button2, user, password, net, window, ischeck, issave;
struct USER inf;
NOTIFYICONDATA nid;
UINT WM_TASKBARCREATED;
char config_name[255], name_cfg[255], pass_cfg[255], net_cfg[255];

#define IDB_BUTTON1 3301
#define IDB_BUTTON2 3302
#define IDR_EXIT 13

unsigned WINAPI login(void *arg_p)
{
	client_login(inf, window);
	return 1;
}

unsigned WINAPI logoff(void *arg_p)
{
	client_logoff();
	return 1;
}

unsigned WINAPI checknet(void *arg_p){
	int x = SendMessage(ischeck, BM_GETCHECK, 0, 0);
	if (!x)
		return 1;
	while (client_issuccess() == 0)
	{
		Sleep(5000);
	}
	if (client_issuccess() == -1)
	{
		client_logoff();
		NOTIFYICONDATA nid;
		memset(&nid, 0, sizeof(nid));
		nid.cbSize = sizeof(nid);
		nid.hWnd = window;
		nid.uID = 0;
		lstrcpy(nid.szInfo, L"重连");
		nid.uFlags = NIF_INFO;
		nid.dwInfoFlags = NIIF_INFO;
		Shell_NotifyIcon(NIM_MODIFY, &nid);
		unsigned id;
		HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, login, NULL, 0, &id);
		CloseHandle(handle);
		handle = (HANDLE)_beginthreadex(NULL, 0, checknet, NULL, 0, &id);
		CloseHandle(handle);
		return 1;
	}
	while (1) 
	{
		Sleep(60000);
		char buf[255];

		SECURITY_ATTRIBUTES sa;
		HANDLE hRead, hWrite;
		FILE *ret = NULL;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		if (!CreatePipe(&hRead, &hWrite, &sa, 0))
		{
			return FALSE;
		}
		STARTUPINFOA si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.hStdInput = hRead;
		si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
		si.hStdOutput = hWrite;
		si.wShowWindow = SW_HIDE;

		si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

		if (!CreateProcessA(NULL, "ping www.baidu.com", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
		{
			CloseHandle(hWrite);
			CloseHandle(hRead);
			return FALSE;
		}
		else
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		int hout = _open_osfhandle((intptr_t)hRead, _O_TEXT);
		FILE *x = _fdopen(hout,"r");
		if (!x)
			return 0;
		Sleep(5000);
		fgets(buf, sizeof(buf), x);
		if (strlen(buf)>2)
		{
			client_logoff();
			NOTIFYICONDATA nid;
			memset(&nid, 0, sizeof(nid));
			nid.cbSize = sizeof(nid);
			nid.hWnd = window;
			nid.uID = 0;
			lstrcpy(nid.szInfo, L"掉线自动重连");
			nid.uFlags = NIF_INFO;
			nid.dwInfoFlags = NIIF_INFO;
			Shell_NotifyIcon(NIM_MODIFY, &nid);
			unsigned id;
			HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, login, NULL, 0, &id);
			CloseHandle(handle);
			handle = (HANDLE)_beginthreadex(NULL, 0, checknet, NULL, 0, &id);
			CloseHandle(handle);
			CloseHandle(hRead);
			return 1;
		}
		CloseHandle(hRead);
	}
	return 1;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(6);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	int x = GetSystemMetrics(SM_CXSCREEN)/2-w/2;
	int y = GetSystemMetrics(SM_CYSCREEN)/2-h/2;

	HWND hWnd = CreateWindowEx(WS_EX_CONTROLPARENT| WS_EX_CLIENTEDGE,szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		x, y, w, h, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

WCHAR *c2w(const char *x) {
	size_t len = strlen(x) + 1;
	size_t converted = 0;
	WCHAR *tmp;
	tmp = (wchar_t*)malloc(len*sizeof(wchar_t));
	mbstowcs_s(&converted, tmp, len, x, _TRUNCATE);
	return tmp;
}

void readcfg()
{
	FILE *config = fopen(config_name, "rb");
	if (config)
	{
		int x = fgetc(config);
		if (x)
		{
			SendMessage(issave, BM_SETCHECK, 1, 0);
			do
			{
				x = fgetc(config);
				if (x == 255)break;
				name_cfg[strlen(name_cfg)] = x + '0';
			} while (1);
			do
			{
				x = fgetc(config);
				if (x == 255)break;
				pass_cfg[strlen(pass_cfg)] = x - name_cfg[strlen(name_cfg) - 1] + '0';
			} while (1);
			do
			{
				x = fgetc(config);
				if (x == 255)break;
				net_cfg[strlen(net_cfg)] = x;
			} while (1);
			x = fgetc(config);
			SendMessageA(user, WM_SETTEXT, 0, (LPARAM)name_cfg);
			SendMessageA(password, WM_SETTEXT, 0, (LPARAM)pass_cfg);
			for (int i = 0;i < netcardnum;i++)
				if (strstr(netcard[i].AdapterName, net_cfg))
					SendMessage(net, CB_SETCURSEL, i, 0);
			SendMessage(ischeck, BM_SETCHECK, x, 0);
		}
		fclose(config);
	}
}

void savecfg(bool flag)
{
	FILE *config = fopen(config_name, "wb");
	if (config)
	{
		if (!flag) 
		{ 
			fputc(0, config); 
			return; 
		}
		else fputc(1, config);
		for (int i = 0;i < strlen(inf.name);i++)
			fputc(inf.name[i] - '0', config);
		fputc(255, config);
		for (int i = 0;i < strlen(inf.password);i++)
			fputc(inf.password[i] + inf.name[strlen(inf.name) - 1] - '0', config);
		fputc(255, config);
		char netname[255];
		strcpy(netname, netcard[SendMessage(net, CB_GETCURSEL, 0, 0)].AdapterName);
		for (int i = 0;i < strlen(netname);i++)
			fputc(netname[i], config);
		fputc(255, config);
		fputc(SendMessage(ischeck, BM_GETCHECK, 0, 0), config);
		fputc(255, config);
		fclose(config);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int xx;
	POINT pt;

	switch (message)
	{
		case WM_CREATE:
		{
			button = CreateWindowEx(0L, L"Button", L"登录", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
				w - 150, h - 80, 100, 30, hWnd, (HMENU)IDB_BUTTON1, hInst, NULL);
			button2 = CreateWindowEx(0L, L"Button", L"关于", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
				30, h - 80, 100, 30, hWnd, (HMENU)IDB_BUTTON2, hInst, NULL);
			user = CreateWindowEx(0L, L"Edit", L"", WS_BORDER | WS_VISIBLE | WS_CHILD,
				80, 40, 150, 25, hWnd, NULL, hInst, NULL);
			password = CreateWindowEx(0L, L"Edit", L"", WS_BORDER | WS_VISIBLE | WS_CHILD | ES_PASSWORD,
				80, 80, 150, 25, hWnd, NULL, hInst, NULL);
			SendMessage(password, EM_SETPASSWORDCHAR, (WPARAM)L'●', NULL);
			net = CreateWindowEx(0L, L"ComboBox", L"", CBS_DROPDOWNLIST | WS_VISIBLE | WS_CHILD,
				80, 120, 300, 1000, hWnd, NULL, hInst, NULL);
			for (int i = 0;i < netcardnum;i++)
				SendMessage(net, CB_ADDSTRING, 0, (LPARAM)c2w(netcard[i].Description));
			SendMessage(net, CB_SETCURSEL, 0, 0);
			issave = CreateWindowEx(0L, L"Button", L"", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
				240, 45, 13, 13, hWnd, NULL, hInst, NULL);
			ischeck = CreateWindowEx(0L, L"Button", L"", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
				240, 85, 13, 13, hWnd, NULL, hInst, NULL);
			
			SendMessage(user, WM_SETFONT, (WPARAM)hFont, MAKELONG(TRUE, 0));
			SendMessage(password, WM_SETFONT, (WPARAM)hFont, MAKELONG(TRUE, 0));
			SendMessage(button, WM_SETFONT, (WPARAM)hFont, MAKELONG(TRUE, 0));
			SendMessage(button2, WM_SETFONT, (WPARAM)hFont, MAKELONG(TRUE, 0));
			SendMessage(net, WM_SETFONT, (WPARAM)hFont, MAKELONG(TRUE, 0));

			WM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));
			nid.cbSize = sizeof(nid);
			nid.hWnd = hWnd;
			nid.uID = 0;
			nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			nid.uCallbackMessage = WM_USER;
			nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL));
			if (!nid.hIcon)
				MessageBox(NULL, L"wrong!!", L"", MB_OK);
			lstrcpy(nid.szTip, L"客户端");
			Shell_NotifyIcon(NIM_ADD, &nid);
			hmenu = CreatePopupMenu();
			AppendMenu(hmenu, MF_STRING, IDR_EXIT, L"退出");

			readcfg();
		}
		break;
		case WM_USER:
		{
			if (lParam == WM_RBUTTONDOWN)
			{
				GetCursorPos(&pt);
				SetForegroundWindow(hWnd);
				xx = TrackPopupMenu(hmenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd, NULL);
				if (xx == IDR_EXIT)
				{
					inf.terminate = true;
					unsigned id;
					HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, logoff, NULL, 0, &id);
					CloseHandle(handle);
					SendMessage(hWnd, WM_CLOSE, wParam, lParam);
				}
				if (xx == 0) PostMessage(hWnd, WM_LBUTTONDOWN, NULL, NULL);
			}

		}
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			switch (wmId)
			{
			case IDB_BUTTON1:
				SendMessageA(user, WM_GETTEXT, 50, (LPARAM)inf.name);
				SendMessageA(password, WM_GETTEXT, 50, (LPARAM)inf.password);
				if (getnic(netcard[SendMessage(net, CB_GETCURSEL, 0, 0)].AdapterName, inf.nic))
					MessageBox(NULL, L"网卡错误", L"", MB_OK);
				else {
					savecfg(SendMessage(issave, BM_GETCHECK, 0, 0));
					unsigned id;
					window = hWnd;
					HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, login, NULL, 0, &id);
					CloseHandle(handle);
					handle = (HANDLE)_beginthreadex(NULL, 0, checknet, NULL, 0, &id);
					CloseHandle(handle);
					//MessageBox(NULL, client_test(inf) + L"0", L"", MB_OK);
					ShowWindow(hWnd, SW_HIDE);
				}
				break;
			case IDB_BUTTON2:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
			SelectObject(hdc, hFont);
			TextOut(hdc, 10, 42, L"用户名：", 4);
			TextOut(hdc, 10, 83, L"密码：", 3);
			TextOut(hdc, 10, 123, L"网卡：", 3);
			TextOut(hdc, 260, 40, L"保存账号密码", 6);
			TextOut(hdc, 260, 80, L"断线重连", 4);
			EndPaint(hWnd, &ps);
		}
		break;
		case WM_DESTROY:
			Shell_NotifyIcon(NIM_DELETE, &nid);
			PostQuitMessage(0);
			break;
		default:
			if (message == WM_TASKBARCREATED)
				SendMessage(hWnd, WM_CREATE, wParam, lParam);
			break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

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

int WINAPI _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    hFont = (HFONT)GetStockObject(SYSTEM_FONT);
	LOGFONT font;
	GetObject(hFont, sizeof(font), &font);
	font.lfWidth = 0;
	font.lfHeight = 20;
	font.lfWeight = 400;
	font.lfQuality = PROOF_QUALITY;
	wcscpy_s(font.lfFaceName, L"微软雅黑");
	hFont = CreateFontIndirect(&font);
	DeleteObject(GetStockObject(SYSTEM_FONT));

	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	char tmp[20] = "";
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		delete pIpAdapterInfo;
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}
	if (ERROR_SUCCESS == nRel)
		for (;pIpAdapterInfo;pIpAdapterInfo = pIpAdapterInfo->Next) {
			strcpy_s(netcard[netcardnum].AdapterName, pIpAdapterInfo->AdapterName);
			strcpy_s(netcard[netcardnum++].Description, pIpAdapterInfo->Description);
		}
	if (pIpAdapterInfo)
		delete pIpAdapterInfo;

	char *env = getenv("USERPROFILE");
	strcpy(config_name, env);
	strcat(config_name, "\\AppData\\Local\\ljzyal\\data.dat");
	char cmd[255]="mkdir ";
	strcat(cmd, env);
	strcat(cmd, "\\AppData\\Local\\ljzyal");
	WinExec(cmd, SW_HIDE);

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
        return FALSE;

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}
