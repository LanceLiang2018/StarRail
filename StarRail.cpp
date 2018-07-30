#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <windows.h>

#define WIDTH	1200
#define HEIGHT	700
#define PI		3.14159

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* insert window procedure function here */
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION); /* use "A" as icon name when you want to use the project icon */
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION); /* as above */

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","ÐÇ¹ì",WS_VISIBLE|WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,WIDTH+24,HEIGHT+48,NULL,NULL,hInstance,NULL);
	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

#define CNUM		500
#define SPEED		1
#define ASPEED		0.01
#define PER			((WIDTH > HEIGHT ? WIDTH : HEIGHT) / CNUM / 2)

#define SCOLOR_R	0xB0
#define SCOLOR_G	0xD0
#define SCOLOR_B	0xE0
#define ECOLOR_R	0xD0
#define ECOLOR_G	0xE0
#define ECOLOR_B	0xFF

/*
#define SCOLOR_R	0xE0
#define SCOLOR_G	0xE0
#define SCOLOR_B	0xE0
#define ECOLOR_R	0xFF
#define ECOLOR_G	0xFF
#define ECOLOR_B	0xFF
*/
#define ENLARGE	10
#define SHPEN		3
#define EHPEN		15
#define SEDGE		0.618

class Point
{
public:
	double x, y;
	Point(): x(0), y(0) {}
	Point(double ix, double iy): x(ix), y(iy) {}
	Point* set(double ix, double iy) {x = ix; y = iy; return this;}
};

Point pos[CNUM+1];
Point last[CNUM+1];
Point O(WIDTH/2, HEIGHT/2);
double cnt[CNUM+1];
double r[CNUM+1];
unsigned long int color[CNUM+1];
int hpensize[CNUM+1];
HDC gdc = NULL;
HBITMAP gitmap;
RECT rc;

double count = 0;

//×ø±ê, °ë¾¶, ½Ç¶È 
Point get_pos(double r, double a)
{
	return Point((O.x + r * cos(a)), (O.y + r * sin(a)));
}

#define MAXRAND		1000
double get_rand()
{
	return (double)(rand() % MAXRAND) / (double)MAXRAND;
}

void Create(HWND hwnd)
{
	gdc = GetDC(hwnd);
	//gdc = CreateDC("DISPLAY",NULL,NULL,NULL);
	gitmap = CreateCompatibleBitmap(gdc, WIDTH*ENLARGE, HEIGHT*ENLARGE);
	SetStretchBltMode(gdc, HALFTONE);
	GetClientRect(hwnd, &rc);
	rc.bottom *= ENLARGE;
	rc.left *= ENLARGE;
	rc.right *= ENLARGE;
	rc.top *= ENLARGE;
	//FillRect(gdc, &rc, CreateSolidBrush(RGB(255, 255, 255)));
	HDC hdc = CreateCompatibleDC(gdc);
	SelectObject(hdc, gitmap);
	//FillRect(hdc, &rc, CreateSolidBrush(RGB(255, 255, 255)));
	FillRect(hdc, &rc, CreateSolidBrush(RGB(32, 32, 32)));
	DeleteDC(hdc);

	
	srand(time(NULL));
	SetTimer(hwnd, 1, SPEED, NULL);
	for (int i=1; i<=CNUM; i++)
	{
		color[i] = RGB(SCOLOR_R+(ECOLOR_R-SCOLOR_R)*get_rand(),
					   SCOLOR_G+(ECOLOR_G-SCOLOR_G)*get_rand(),
					   SCOLOR_B+(ECOLOR_B-SCOLOR_B)*get_rand());
		
		//color[i] = RGB(SCOLOR_R, SCOLOR_B, SCOLOR_R);
		hpensize[i] = get_rand() * (EHPEN - SHPEN) + SHPEN;
		r[i] = i * PER + get_rand() * PER / 2 * (rand() % 2?1:-1);
		pos[i].set(WIDTH/2 + r[i], HEIGHT/2);
		last[i] = pos[i];
		cnt[i] = 2 * PI * get_rand();
	}
	count = 0;
}

void Draw(HWND hwnd)
{
	HDC hdc = CreateCompatibleDC(gdc);
	SelectObject(hdc, gitmap);
	//FillRect(hdc, &rc, CreateSolidBrush(RGB(255, 255, 255)));
	//MoveToEx(hdc, O.x, O.y, 0);
	//LineTo(hdc, O.x, O.y);
	for (int i=1; i<=CNUM; i++)
	{
		HPEN hPen = CreatePen(PS_SOLID, hpensize[i] * ENLARGE / 5, color[i]);
		SelectObject(hdc, hPen);
		MoveToEx(hdc, (int)pos[i].x * ENLARGE, (int)pos[i].y * ENLARGE, 0);
		//MoveToEx(hdc, (int)last[i].x * ENLARGE, (int)last[i].y * ENLARGE, 0);
		LineTo(hdc, (int)pos[i].x * ENLARGE, (int)pos[i].y * ENLARGE);
		DeleteObject(hPen);
	}
	StretchBlt(gdc, 0, 0, WIDTH, HEIGHT, hdc, 0, 0, WIDTH*ENLARGE, HEIGHT*ENLARGE, SRCCOPY);
	//StretchBlt(gdc, 0, 0, WIDTH*ENLARGE, HEIGHT*ENLARGE, hdc, 0, 0, WIDTH*ENLARGE, HEIGHT*ENLARGE, SRCCOPY);
	DeleteDC(hdc);
}

void Timer(HWND hwnd)
{
	if (count >= 2 * PI)
		Create(hwnd);
	count += ASPEED / PI / 2;
	for (int i=1; i<=CNUM; i++)
	{
		last[i] = pos[i];
		cnt[i] += ASPEED / PI / 2;
		pos[i] = get_pos(r[i], cnt[i]);
	}
	Draw(hwnd);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
		case WM_CREATE:
			Create(hwnd);
			break;
		case WM_TIMER:
			Timer(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

