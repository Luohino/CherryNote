#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windowsx.h>

/* ============================================================================
   CherryNote - Ultra-Lightweight Floating Notes Widget
   Native Win32 Wide-API (W) Implementation
   ============================================================================ */

#define NOTES_FILE L"CherryNote.txt"
#define MAX_TEXT_SIZE 65536
#define HEADER_HEIGHT 28
#define RADIUS 12
#define RESIZE_MARGIN 8

/* Colors (BGR) */
#define COLOR_HEADER_START 0x0000008B
#define COLOR_HEADER_END   0x002D2DFF
#define COLOR_BODY_BG      0x001E1E1E
#define COLOR_TEXT         0x00FFFFFF
#define COLOR_IOS_RED      0x004B4BFF
#define COLOR_IOS_YELLOW   0x0047D4FF
#define COLOR_IOS_BLUE     0x00FF8800  /* Bright Blue */
#define COLOR_IOS_BLUE_DIM 0x00885500  /* Darker Blue */

/* Global State */
HWND g_hwndMain = NULL;
HWND g_hwndEdit = NULL;
WNDPROC g_wpOldEditProc = NULL;
wchar_t g_noteText[MAX_TEXT_SIZE] = {0};
BOOL g_isShaded = FALSE;
BOOL g_isLocked = TRUE;  
BOOL g_needsSave = FALSE;
int g_winWidth = 250;
int g_winHeight = 400;

/* Prototypes */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void SaveNotes(void);
void LoadNotes(void);
void UpdateClipping(HWND hwnd);
void DrawHeader(HDC hdc, RECT rc);
void ToggleStrikethroughLine(HWND hwndEdit);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"CherryNoteClassW";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(COLOR_BODY_BG);
    wc.style = CS_HREDRAW | CS_VREDRAW; /* Force repaint on resize */

    if (!RegisterClassW(&wc)) return 1;

    g_hwndMain = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        L"CherryNoteClassW", L"CherryNote",
        WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN,
        100, 100, g_winWidth, g_winHeight,
        NULL, NULL, hInstance, NULL
    );

    if (!g_hwndMain) return 1;

    UpdateClipping(g_hwndMain);
    LoadNotes();

    g_hwndEdit = CreateWindowExW(
        0, L"EDIT", g_noteText,
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL | WS_VSCROLL,
        10, HEADER_HEIGHT + 5, g_winWidth - 20, g_winHeight - HEADER_HEIGHT - 15,
        g_hwndMain, (HMENU)101, hInstance, NULL
    );

    ShowScrollBar(g_hwndEdit, SB_VERT, FALSE);
    g_wpOldEditProc = (WNDPROC)SetWindowLongPtrW(g_hwndEdit, GWLP_WNDPROC, (LONG_PTR)EditProc);

    HFONT hFont = CreateFontW(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              CLEARTYPE_QUALITY, VARIABLE_PITCH | FF_SWISS, L"Segoe UI Variable Text");
    if (!hFont) hFont = CreateFontW(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                    CLEARTYPE_QUALITY, VARIABLE_PITCH | FF_SWISS, L"Segoe UI");
    SendMessageW(g_hwndEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

    ShowWindow(g_hwndMain, nCmdShow);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_ERASEBKGND:
            return 1; /* Manually erase background in WM_PAINT */
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);

            /* Fill body background to prevent ghosting */
            HBRUSH hbrBg = CreateSolidBrush(COLOR_BODY_BG);
            FillRect(hdc, &rc, hbrBg);
            DeleteObject(hbrBg);

            DrawHeader(hdc, rc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_NCHITTEST: {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            RECT rc;
            GetWindowRect(hwnd, &rc);

            if (!g_isShaded && !g_isLocked) {
                if (pt.x >= rc.right - RESIZE_MARGIN && pt.y >= rc.bottom - RESIZE_MARGIN) return HTBOTTOMRIGHT;
                if (pt.x >= rc.right - RESIZE_MARGIN) return HTRIGHT;
                if (pt.y >= rc.bottom - RESIZE_MARGIN) return HTBOTTOM;
            }

            ScreenToClient(hwnd, &pt);
            if (pt.y < HEADER_HEIGHT && pt.x > 70) return HTCAPTION;
            if (!g_isLocked && pt.y >= HEADER_HEIGHT) return HTCAPTION;
            
            return HTCLIENT;
        }
        case WM_SIZE: {
            if (!g_isShaded) {
                g_winWidth = LOWORD(lParam);
                g_winHeight = HIWORD(lParam);
                MoveWindow(g_hwndEdit, 10, HEADER_HEIGHT + 5, g_winWidth - 20, g_winHeight - HEADER_HEIGHT - 15, TRUE);
                UpdateClipping(hwnd);
                ShowScrollBar(g_hwndEdit, SB_VERT, FALSE);
            }
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if (y >= 8 && y <= 20) {
                if (x >= 10 && x <= 22) SendMessageW(hwnd, WM_CLOSE, 0, 0); 
                if (x >= 28 && x <= 40) { 
                    g_isShaded = !g_isShaded;
                    int h = g_isShaded ? HEADER_HEIGHT : g_winHeight;
                    SetWindowPos(hwnd, NULL, 0, 0, g_winWidth, h, SWP_NOMOVE | SWP_NOZORDER);
                    ShowWindow(g_hwndEdit, g_isShaded ? SW_HIDE : SW_SHOW);
                    UpdateClipping(hwnd);
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                if (x >= 46 && x <= 58) { 
                    g_isLocked = !g_isLocked;
                    InvalidateRect(hwnd, NULL, TRUE);
                }
            }
            return 0;
        }
        case WM_CTLCOLOREDIT: {
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, COLOR_TEXT);
            SetBkColor(hdc, COLOR_BODY_BG);
            return (LRESULT)CreateSolidBrush(COLOR_BODY_BG);
        }
        case WM_TIMER: {
            if (wParam == 1 && g_needsSave) {
                SaveNotes();
                g_needsSave = FALSE;
                KillTimer(hwnd, 1);
            }
            return 0;
        }
        case WM_CLOSE: DestroyWindow(hwnd); return 0;
        case WM_DESTROY: SaveNotes(); PostQuitMessage(0); return 0;
        default: return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

LRESULT CALLBACK EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_MOUSEWHEEL: {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            int linesToScroll = -delta / WHEEL_DELTA * 4; 
            SendMessageW(hwnd, EM_LINESCROLL, 0, linesToScroll);
            return 0;
        }
        case WM_KEYDOWN:
            if (GetKeyState(VK_CONTROL) & 0x8000) {
                if (wParam == 'A') {
                    SendMessageW(hwnd, EM_SETSEL, 0, -1);
                    return 0;
                }
                if (wParam == 'D') {
                    ToggleStrikethroughLine(hwnd);
                    return 0;
                }
                if (wParam == VK_RETURN) {
                    int len = GetWindowTextLengthW(hwnd);
                    wchar_t *buf = malloc((len + 1) * sizeof(wchar_t));
                    GetWindowTextW(hwnd, buf, len + 1);
                    int next = 1;
                    for (int i = 0; i < len; i++) {
                        if (iswdigit(buf[i]) && (i == 0 || buf[i-1] == L'\n')) {
                            int val = _wtoi(&buf[i]);
                            if (val >= next) next = val + 1;
                        }
                    }
                    free(buf);
                    wchar_t newItem[32];
                    if (len == 0) swprintf(newItem, 32, L"%d. ", next);
                    else swprintf(newItem, 32, L"\r\n%d. ", next);
                    SendMessageW(hwnd, EM_REPLACESEL, TRUE, (LPARAM)newItem);
                    g_needsSave = TRUE;
                    SetTimer(g_hwndMain, 1, 500, NULL);
                    return 0;
                }
            }
            break;
        case WM_CHAR:
            g_needsSave = TRUE;
            SetTimer(g_hwndMain, 1, 1000, NULL);
            break;
    }
    return CallWindowProcW(g_wpOldEditProc, hwnd, uMsg, wParam, lParam);
}

void DrawHeader(HDC hdc, RECT rc) {
    TRIVERTEX v[2];
    v[0].x = rc.left; v[0].y = rc.top;
    v[0].Red = GetRValue(COLOR_HEADER_START) << 8; v[0].Green = GetGValue(COLOR_HEADER_START) << 8; v[0].Blue = GetBValue(COLOR_HEADER_START) << 8; v[0].Alpha = 0;
    v[1].x = rc.right; v[1].y = HEADER_HEIGHT;
    v[1].Red = GetRValue(COLOR_HEADER_END) << 8; v[1].Green = GetGValue(COLOR_HEADER_END) << 8; v[1].Blue = GetBValue(COLOR_HEADER_END) << 8; v[1].Alpha = 0;
    GRADIENT_RECT gr = {0, 1};
    GradientFill(hdc, v, 2, &gr, 1, GRADIENT_FILL_RECT_H);

    HBRUSH hRed = CreateSolidBrush(COLOR_IOS_RED), hYel = CreateSolidBrush(COLOR_IOS_YELLOW);
    HBRUSH hBlue = CreateSolidBrush(g_isLocked ? COLOR_IOS_BLUE : COLOR_IOS_BLUE_DIM);
    HPEN hNull = CreatePen(PS_NULL, 0, 0);
    HGDIOBJ oP = SelectObject(hdc, hNull);
    HGDIOBJ oB = SelectObject(hdc, hRed);
    Ellipse(hdc, 10, 8, 22, 20);
    SelectObject(hdc, hYel);
    Ellipse(hdc, 28, 8, 40, 20);
    SelectObject(hdc, hBlue);
    Ellipse(hdc, 46, 8, 58, 20);
    
    SelectObject(hdc, oB); SelectObject(hdc, oP);
    DeleteObject(hRed); DeleteObject(hYel); DeleteObject(hBlue); DeleteObject(hNull);

    SetTextColor(hdc, COLOR_TEXT); SetBkMode(hdc, TRANSPARENT);
    HFONT hF = CreateFontW(13, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH | FF_SWISS, L"Segoe UI");
    HGDIOBJ oF = SelectObject(hdc, hF);
    /* Perfect Center Logic: Full width rectangle forcentering */
    RECT tr = {0, 0, rc.right, HEADER_HEIGHT};
    DrawTextW(hdc, L"CherryNote \xD83C\xDF52", -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, oF); DeleteObject(hF);
}

void ToggleStrikethroughLine(HWND hwndEdit) {
    DWORD s, e; SendMessageW(hwndEdit, EM_GETSEL, (WPARAM)&s, (LPARAM)&e);
    int idx = SendMessageW(hwndEdit, EM_LINEFROMCHAR, s, 0);
    int start = SendMessageW(hwndEdit, EM_LINEINDEX, idx, 0);
    int len = SendMessageW(hwndEdit, EM_LINELENGTH, start, 0);
    if (len == 0) return;
    wchar_t *line = malloc((len + 1) * sizeof(wchar_t));
    *(WORD*)line = (WORD)len;
    SendMessageW(hwndEdit, EM_GETLINE, idx, (LPARAM)line);
    line[len] = L'\0';

    BOOL hasS = (wcschr(line, 0x0336) != NULL);
    wchar_t *newVal = malloc((len * 2 + 1) * sizeof(wchar_t));
    wchar_t *q = newVal;
    for (int i = 0; i < len; i++) {
        if (hasS) {
            if (line[i] != 0x0336) *q++ = line[i];
        } else {
            *q++ = line[i];
            if (!iswspace(line[i])) *q++ = 0x0336;
        }
    }
    *q = L'\0';
    SendMessageW(hwndEdit, EM_SETSEL, start, start + len);
    SendMessageW(hwndEdit, EM_REPLACESEL, TRUE, (LPARAM)newVal);
    free(line); free(newVal);
    g_needsSave = TRUE; SetTimer(g_hwndMain, 1, 500, NULL);
}

void UpdateClipping(HWND hwnd) {
    RECT r; GetWindowRect(hwnd, &r);
    HRGN h = CreateRoundRectRgn(0, 0, r.right - r.left, r.bottom - r.top, RADIUS, RADIUS);
    SetWindowRgn(hwnd, h, TRUE);
}

void SaveNotes(void) {
    int len = GetWindowTextLengthW(g_hwndEdit);
    wchar_t *buf = malloc((len + 1) * sizeof(wchar_t));
    GetWindowTextW(g_hwndEdit, buf, len + 1);
    FILE *f = _wfopen(NOTES_FILE, L"wb");
    if (f) {
        unsigned short bom = 0xFEFF;
        fwrite(&bom, 2, 1, f);
        fwrite(buf, sizeof(wchar_t), len, f);
        fclose(f);
    }
    free(buf);
}

void LoadNotes(void) {
    FILE *f = _wfopen(NOTES_FILE, L"rb");
    if (f) {
        fseek(f, 0, SEEK_END); long size = ftell(f); fseek(f, 2, SEEK_SET);
        if (size > 2) fread(g_noteText, sizeof(wchar_t), (size - 2) / 2, f);
        fclose(f);
    }
}
