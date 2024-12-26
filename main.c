#include <stdio.h>
#include <windows.h>
#include <math.h>
#define PI 3.1415926535f
#define P2 PI/2
#define P3 3*PI/2

//player position
int px = 312;
int py = 312;
float pdx,pdy,pa; // deltax,y and player angle

//map size
int mapX = 8, mapY=8,mapSize=64;
int map[] =
        {
        1,1,1,1,1,1,1,1,
        1,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,1,
        1,0,1,1,0,0,0,1,
        1,0,1,0,0,0,0,1,
        1,0,0,0,0,1,0,1,
        1,0,0,0,0,0,0,1,
        1,1,1,1,1,1,1,1,
        };

float dist(float ax, float ay, float bx, float by, float ang){
    return (sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay)));
}

void drawRays(HDC hdc) {
    int mx, my, mp, dof;
    float rx, ry, ra, xo, yo;
    ra = pa;
    for (int r = 0; r < 1; r++) {

        // Check Horizontal Lines
        dof=0;
        float disH = 1000000,hx=px,hy=py;
        float aTan=-1/tan(ra);
        if(ra>PI){ry=(((int)py>>6)<<6)-0.0001; rx=(py-ry)*aTan+px;yo=-64;xo=-yo*aTan;}
        if(ra<PI){ry=(((int)py>>6)<<6)+64; rx=(py-ry)*aTan+px;yo=64;xo=-yo*aTan;}
        if(ra==0 || ra==PI)
        {
            rx=px;
            ry=py;
            dof=8;
        }
        while(dof<8)
        {
            mx=(int)(rx)>>6;
            my=(int)(ry)>>6;
            mp=my*mapX+mx;
            if(mp>0 && mp<mapX*mapY && map[mp]==1)
            {
                hx=rx;
                hy=ry;
                disH=dist(px,py,hx,hy,ra);
                dof = 8;
            }else{
                rx+=xo;
                ry+=yo;
                dof+=1;
            }
        }
        // Check Vertical Lines
        dof = 0;
        float disV = 1000000,vx=px,vy=py;
        float nTan = -tan(ra);
        if (ra > P2 && ra < P3) {
            rx = (((int) px >> 6) << 6) - 0.0001;
            ry = (px - rx) * nTan + py;
            xo = -64;
            yo = -xo * nTan;
        }
        if (ra < P2 || ra > P3) {
            rx = (((int) px >> 6) << 6) + 64;
            ry = (px - rx) * nTan + py;
            xo = 64;
            yo = -xo * nTan;
        }
        if (ra == 0 || ra == PI) {
            rx = px;
            ry = py;
            dof = 8;
        }
        while (dof < 8) {
            mx = (int) (rx) >> 6;
            my = (int) (ry) >> 6;
            mp = my * mapX + mx;
            if (mp>0 && mp < mapX * mapY && map[mp] == 1) {
                vx=rx;
                vy=ry;
                disV=dist(px,py,vx,vy,ra);
                dof = 8;
            } else {
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }
        if(disV<disH){rx = vx; ry=vy;}
        if(disH<disV){rx = hx; ry=hy;}

        HPEN hPen1 = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
        HPEN oldPen1 = (HPEN) SelectObject(hdc, hPen1);
        MoveToEx(hdc, px, py, NULL);
        LineTo(hdc, rx, ry);
        SelectObject(hdc, oldPen1);
        DeleteObject(hPen1);
    }
}

void drawMap(HDC hdc)
{
    for(int y =0; y<mapY;y++){
        for(int x =0; x<mapX;x++){
            int xPos = x * mapSize;
            int yPos = y * mapSize;

            RECT square = {xPos+1,yPos+1,xPos+mapSize-1,yPos+mapSize-1};

            HBRUSH brush;

            if(map[y*mapX+x] == 1){
                brush = CreateSolidBrush(RGB(255,255,255));
            }else{
                brush = CreateSolidBrush(RGB(0,0,0));
            }
            FillRect(hdc, &square, brush);
            DeleteObject(brush);
        }
    }
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HBITMAP hBitmap = NULL;
    switch(uMsg) {
        case WM_KEYDOWN: {
            switch (wParam) {
                case 'W':
                    //py-=mapY;
                    px+=pdx;
                    py+=pdy;
                    break;
                case 'A':
                    //px-=mapX;
                    pa-= 0.1f;
                    if(pa<0){
                        pa+=2*PI;
                    }
                    pdx=cos(pa)*5;
                    pdy=sin(pa)*5;
                    break;
                case 'S':
                    //py+=mapY;
                    px-=pdx;
                    py-=pdy;
                    break;
                case 'D':
                    //px+=mapX;
                    pa+= 0.1f;
                    if(pa>2*PI){
                        pa-=2*PI;
                    }
                    pdx=cos(pa)*5;
                    pdy=sin(pa)*5;
                    break;
                default:
                    break;
            }
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            HDC hdcMem = CreateCompatibleDC(hdc);
            pdx = cos(pa)*5;
            pdy = sin(pa)*5;
            if (!hBitmap) {
                RECT clientRect;
                GetClientRect(hWnd, &clientRect);
                hBitmap = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
            }
            SelectObject(hdcMem, hBitmap);

            //background
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);
            HBRUSH bgBrush = CreateSolidBrush(RGB(72, 72, 72));
            FillRect(hdcMem, &clientRect, bgBrush);
            DeleteObject(bgBrush);

            //map
            drawMap(hdcMem);

            //draw rays
            drawRays(hdcMem);

            //player
            RECT player = {px, py, px+mapX, py+mapY};
            HBRUSH pBrush = CreateSolidBrush(RGB(255,0,0));
            FillRect(hdcMem,&player,pBrush);
            DeleteObject(pBrush);

            //Line
            HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
            HPEN oldPen = (HPEN)SelectObject(hdcMem, hPen);
            MoveToEx(hdcMem, px, py, NULL);
            LineTo(hdcMem, px + pdx * 10, py + pdy * 10);
            SelectObject(hdcMem, oldPen);
            DeleteObject(hPen);

            BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, hdcMem, 0, 0, SRCCOPY);
            DeleteDC(hdcMem);
            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_DESTROY: {
            if (hBitmap) DeleteObject(hBitmap);
            PostQuitMessage(0);
            return 0;
        }
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char* CLASS_NAME = "rayCasting";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hWnd = CreateWindowEx(
            0,
            CLASS_NAME,
            "Raycaster",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            800, 600,
            NULL,
            NULL,
            hInstance,
            NULL
    );

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}