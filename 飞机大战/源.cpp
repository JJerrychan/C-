#include "标头.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szWndClassName[] = L"hellowin";
	HWND         hwnd;
	MSG          msg;
	WNDCLASS     wndclass;


	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szWndClassName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, L"注册失败",
			L"错误", MB_ICONERROR);
		return 0;
	}

	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);
	RECT rc;
	rc.left = (cxScreen - CLIENTWIDTH) / 2;
	rc.top = (cyScreen - CLIENTHEIGHT) / 2;
	rc.right = rc.left + CLIENTWIDTH;
	rc.bottom = rc.top + CLIENTHEIGHT;
	AdjustWindowRect(&rc,WS_OVERLAPPED| WS_CAPTION| WS_SYSMENU| WS_MINIMIZEBOX, FALSE);

	hwnd = CreateWindow(szWndClassName,             // window class name
		L"飞机大战",					  // window caption
		WS_OVERLAPPED| WS_CAPTION| WS_SYSMENU| WS_MINIMIZEBOX,        // window style
		rc.left,              // initial x position
		rc.top,              // initial y position
		rc.right-rc.left,              // initial x size
		rc.bottom-rc.top,              // initial y size
		NULL,                       // parent window handle
		NULL,                       // window menu handle
		hInstance,                  // program instance handle
		NULL);                     // creation parameters

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))  //消息队列
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	GdiplusShutdown(gdiplusToken);
	return msg.wParam;  //WM_QUIT
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		GameInit();
		return 0;
	case WM_TIMER:
		GameLogic();
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	case WM_LBUTTONDOWN:
		MouseClick(hwnd, LOWORD(lParam), HIWORD(lParam));
		return 0;
	case WM_MOUSEMOVE:
		if (MK_LBUTTON&wParam)
		{
			MouseMoveWithLButton(LOWORD(lParam), HIWORD(lParam));
		}
		else
		{
			MouseMove(LOWORD(lParam), HIWORD(lParam));
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GamePaint(hdc);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_SETFOCUS:
		SetTimer(hwnd, TIMERID, 1000 / GAMEFRAME, NULL);
		return 0;
	case WM_KILLFOCUS:
		KillTimer(hwnd, TIMERID);
		return 0;
	case WM_SYSCOMMAND:
		if (wParam == SC_CLOSE) //劫持关闭按钮消息
		{
			if (IDCANCEL == MessageBox(hwnd, L"你确定要退出吗？",
				L"", MB_OKCANCEL | MB_ICONQUESTION))
			{
				return 0;
			}
		}
		break;
	case WM_DESTROY:
		ReleaseRes();
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void MouseClick(HWND hwnd, int x, int y)
{
	switch (curScene)
	{
	case SCENE_START:
		MenuClick(hwnd, StartMenu);
		break;
	case SCENE_OVER:
		MenuClick(hwnd, OverMenu);
		break;
	case SCENE_PLAY:
		prePoint.x = x;
		prePoint.y = y;
		break;
	}
}

void MenuClick(HWND hwnd, MenuItem menu[])
{
	if (menu[0].isFocus)	//开始游戏
	{
		frameIndex = 0;
		InitPlayer();
		npcList.clear();
		curScene = SCENE_PLAY;
	}
	else if(menu[1].isFocus)
	{
		//退出游戏
		SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
	}
}

void MouseMoveWithLButton(int x, int y)
{
	if (curScene == SCENE_PLAY)
	{
		int dx = x - prePoint.x;
		int dy = y - prePoint.y;
		if (player.pos.x + dx<0 || player.pos.x + dx>CLIENTWIDTH - player.size.cx)
		{
			dx = 0;
		}
		if (player.pos.y + dy<0 || player.pos.y + dy>CLIENTHEIGHT - player.size.cy)
		{
			dy = 0;
		}
		player.pos.x += dx;
		player.pos.y += dy;
		prePoint.x = x;
		prePoint.y = y;
	}
}

void MouseMove(int x, int y)
{
	POINT pt = { x,y };
	if (curScene == SCENE_START)
	{
		MenuFocus(pt, StartMenu);
	}
	else if (curScene == SCENE_OVER)
	{
		MenuFocus(pt, OverMenu);
	}
}

void MenuFocus(POINT pt,MenuItem menu[])
{
	for (int i = 0; i < 2; i++)
	{
		if (isPointInRectF(pt, menu[i].itemRect))
		{
			menu[i].isFocus = TRUE;
		}
		else
		{
			menu[i].isFocus = FALSE;
		}
	}
}

BOOL isPointInRectF(POINT pt, RectF rectf)
{
	RECT rect;
	rect.left = (LONG)rectf.X;
	rect.top = (LONG)rectf.Y;
	rect.right = (LONG)rectf.X + (LONG)rectf.Width;
	rect.bottom = (LONG)rectf.Y + (LONG)rectf.Height;
	if (PtInRect(&rect, pt))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void GameInit()
{
	srand(GetTickCount());
	LoadImage();
	InitMenu();
	InitNpcStaticInfo();
	InitPlayer();
	curScene = SCENE_START;
}

void LoadImage() 
{
	Bitmap* pBmp = Bitmap::FromFile(L".\\res\\img\\first.png");
	pBmp->GetHBITMAP(NULL, &hBitmap_start);
	pBmp = Bitmap::FromFile(L".\\res\\img\\back.png");
	pBmp->GetHBITMAP(NULL, &hBitmap_play);
	pBmp = Bitmap::FromFile(L".\\res\\img\\over.png");
	pBmp->GetHBITMAP(NULL, &hBitmap_over);
	//NPC
	for (int i = 0; i < 3; i++)
	{
		TCHAR npcPath[30];
		wsprintf(npcPath, L".\\res\\img\\npc%d.png", i + 1);
		LoadImageInfo(pBmp, npcPath, &npcImgInfo[i]);
	}
	//玩家战机
	LoadImageInfo(pBmp, (TCHAR*)L".\\res\\img\\player.png", &PlayerImgInfo);
	delete pBmp;
	pBmp = NULL;
}

void LoadImageInfo(Bitmap* pBmp,TCHAR *path,ImgInfo *imgInfo)
{
	pBmp = Bitmap::FromFile(path);
	pBmp->GetHBITMAP(NULL, &(imgInfo->hBmp));
	imgInfo->size.cx = pBmp->GetWidth();
	imgInfo->size.cy = pBmp->GetHeight();
}

void InitMenu()
{
	//初始化开始场景菜单
	memset(StartMenu, 0, sizeof(StartMenu));
	TCHAR StartMenuName[2][5] = { L"开始游戏",L"退出游戏" };
	for (int i = 0; i < 2; i++)
	{
		//菜单项文字
		lstrcpy(StartMenu[i].itemText, StartMenuName[i]);
		//所关联矩形
		RectF itemRect;
		itemRect.X = (CLIENTWIDTH - MENU_ITEM_WIDTH) / 2;
		itemRect.Y = 320.0f + (MENU_ITEM_HEIGHT + 50)*i;
		itemRect.Width = MENU_ITEM_WIDTH;
		itemRect.Height = MENU_ITEM_HEIGHT;
		StartMenu[i].itemRect = itemRect;
		//是否获得焦点
		StartMenu[i].isFocus = FALSE;
	}

	//初始化结束场景菜单
	memset(OverMenu, 0, sizeof(OverMenu));
	TCHAR OverMenuName[2][5] = { L"重新开始",L"退出游戏" };
	for (int i = 0; i < 2; i++)
	{
		//菜单项文字
		lstrcpy(OverMenu[i].itemText, OverMenuName[i]);
		//所关联矩形
		RectF itemRect;
		itemRect.X = (float)(i == 0 ? 50 : (CLIENTWIDTH - MENU_ITEM_WIDTH - 50));
		itemRect.Y = 300;
		itemRect.Width = (float)MENU_ITEM_WIDTH;
		itemRect.Height = (float)MENU_ITEM_HEIGHT;
		OverMenu[i].itemRect = itemRect;
		//是否获得焦点
		OverMenu[i].isFocus = FALSE;
	}
}

void InitNpcStaticInfo()
{
	for (int i = 0; i < 3; i++)
	{
		npcStaticInfo[i].npcBitmap = npcImgInfo[i].hBmp;
		npcStaticInfo[i].size.cx = npcImgInfo[i].size.cx / 4;
		npcStaticInfo[i].size.cy = npcImgInfo[i].size.cy;
		npcStaticInfo[i].speed = (3 - i) * 5;
	}
}

void InitPlayer()
{
	player.hBmp = PlayerImgInfo.hBmp;
	player.size.cx = PlayerImgInfo.size.cx / 5;
	player.size.cy = PlayerImgInfo.size.cy;
	player.pos.x = (CLIENTWIDTH - player.size.cx) / 2;
	player.pos.y = 450;
}

void GameLogic()
{
	if (curScene == SCENE_PLAY)
	{	
		frameIndex++;
		if (frameIndex % 30 == 0)
		{
			ProduceNpc(0);
		}
		if (frameIndex % 100 == 0)
		{
			ProduceNpc(1);
		}
		if (frameIndex % 300 == 0)
		{
			ProduceNpc(2);
			frameIndex = 0;
		}
		UpdateNpcPos();
		CollsionDetection();
	}
}

void ProduceNpc(int type)
{
	NPC npc;
	npc.type = type;
	npc.pos.x = rand() % (CLIENTWIDTH - npcStaticInfo[type].size.cx);
	npc.pos.y = -npcStaticInfo[type].size.cy;
	npcList.push_back(npc);	//将npc添加到npclist当中
}

void UpdateNpcPos()
{
	list<NPC>::iterator iter;
	for (iter = npcList.begin(); iter != npcList.end(); )
	{
		iter->pos.y += npcStaticInfo[iter->type].speed;
		if (iter->pos.y > CLIENTHEIGHT)
		{
			iter = npcList.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

void CollsionDetection() 
{
	Rect playerCollider;
	playerCollider.Width = player.size.cx*0.8;
	playerCollider.Height = player.size.cy*0.8;
	playerCollider.X = player.pos.x + player.size.cx*0.1;
	playerCollider.Y = player.pos.y + player.size.cy*0.1;
	list<NPC>::iterator iter;
	for (iter = npcList.begin(); iter != npcList.end(); iter++)
	{
		Rect npcCollider;
		npcCollider.Width = npcStaticInfo[iter->type].size.cx*0.8;
		npcCollider.Height = npcStaticInfo[iter->type].size.cy*0.8;
		npcCollider.X = iter->pos.x + npcStaticInfo[iter->type].size.cx*0.1;
		npcCollider.Y = iter->pos.y + npcStaticInfo[iter->type].size.cy*0.1;
		if (isCollsionWithRect(playerCollider, npcCollider))
		{
			curScene = SCENE_OVER;
			break;
		}
	}
}

BOOL isCollsionWithRect(Rect rect1,Rect rect2) {
	if (rect1.X >= rect2.X && rect1.X >= rect2.X + rect2.Width) {
		return FALSE;
	}
	else if (rect1.X <= rect2.X && rect1.X + rect1.Width <= rect2.X) {
		return FALSE;
	}
	else if (rect1.Y >= rect2.Y && rect1.Y >= rect2.Y + rect2.Height) {
		return FALSE;
	}
	else if (rect1.Y <= rect2.Y && rect1.Y + rect1.Height <= rect2.Y) {
		return FALSE;
	}
	return TRUE;
}

void GamePaint(HDC hdc)
{
	switch (curScene)
	{
	case SCENE_START:
		//将位图对象选进内存DC中
		DrawStarOver(hdc, hBitmap_start,StartMenu);
		break;
	case SCENE_PLAY:
		DrawPlay(hdc);
		break;
	case SCENE_OVER:
		DrawStarOver(hdc, hBitmap_over, OverMenu);
		break;
	default:
		break;
	}
}

void DrawPlay(HDC hdc)
{
	//创建与DC兼容的内存DC
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBmp = CreateCompatibleBitmap(hdc, CLIENTWIDTH, CLIENTHEIGHT);
	SelectObject(hdcMem, hBmp);
	HDC hdcMemTemp = CreateCompatibleDC(hdc);
	//贴背景图
	SelectObject(hdcMemTemp, hBitmap_play);
	BitBlt(hdcMem, 0, 0, CLIENTWIDTH, CLIENTHEIGHT, hdcMemTemp, 0, 0, SRCCOPY);
	//设置透明度
	BLENDFUNCTION dc_bf;
	dc_bf.BlendOp = AC_SRC_OVER;
	dc_bf.BlendFlags = 0;
	dc_bf.SourceConstantAlpha = 250; //0全透明，255不透明
	dc_bf.AlphaFormat = AC_SRC_ALPHA;
	//贴NPC
	list<NPC>::iterator iter;
	for (iter = npcList.begin(); iter != npcList.end(); iter++)
	{
		SelectObject(hdcMemTemp, npcStaticInfo[iter->type].npcBitmap);
		AlphaBlend(hdcMem, iter->pos.x, iter->pos.y, npcStaticInfo[iter->type].size.cx, npcStaticInfo[iter->type].size.cy,
			hdcMemTemp, 0, 0, npcStaticInfo[iter->type].size.cx, npcStaticInfo[iter->type].size.cy, dc_bf);
	}
	//贴玩家战机
	SelectObject(hdcMemTemp, player.hBmp);
	AlphaBlend(hdcMem, player.pos.x, player.pos.y, player.size.cx, player.size.cy,
		hdcMemTemp, 0, 0, player.size.cx, player.size.cy, dc_bf);
	//将内存中的位图贴到屏幕上
	BitBlt(hdc, 0, 0, CLIENTWIDTH, CLIENTHEIGHT, hdcMem, 0, 0, SRCCOPY);
	//释放位图
	DeleteObject(hBmp);
	//释放内存DC
	DeleteDC(hdcMemTemp);
	DeleteDC(hdcMem);
}

void DrawStarOver(HDC hdc, HBITMAP bkBitmp, MenuItem menu[])
{
	//创建与DC兼容的内存DC
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBmp = CreateCompatibleBitmap(hdc, CLIENTWIDTH, CLIENTHEIGHT);
	SelectObject(hdcMem, hBmp);
	HDC hdcMemTemp = CreateCompatibleDC(hdc);
	//贴背景
	SelectObject(hdcMemTemp, bkBitmp);
	BitBlt(hdcMem, 0, 0, CLIENTWIDTH, CLIENTHEIGHT, hdcMemTemp, 0, 0, SRCCOPY);
	//贴菜单
	for (int i = 0; i < 2; i++)
	{
		if (menu[i].isFocus)
		{
			DrawMenu(hdcMem, menu[i].itemText, menu[i].itemRect, Color::Red);
		}
		else
		{
			DrawMenu(hdcMem, menu[i].itemText, menu[i].itemRect, Color::White);
		}
	}
	//将内存中的位图贴到屏幕上
	BitBlt(hdc, 0, 0, CLIENTWIDTH, CLIENTHEIGHT, hdcMem, 0, 0, SRCCOPY);
	//释放位图
	DeleteObject(hBmp);
	//释放内存DC
	DeleteDC(hdcMemTemp);
	DeleteDC(hdcMem);
}

void DrawMenu(HDC hdc, PTCHAR text, RectF rect, Color fontColor)
{
	Graphics graph(hdc);
	FontFamily fontFamily(L"黑体");
	Font font(&fontFamily, 17, FontStyleBold, UnitPoint);  //黑体、加粗
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter); //水平方向对齐方式
	format.SetLineAlignment(StringAlignmentCenter); //垂直方向对齐方式
	SolidBrush  solidBrush1(fontColor);
	graph.SetTextRenderingHint(TextRenderingHintAntiAlias);  //消除锯齿
	graph.DrawString(text, (INT)wcslen(text), &font, rect, &format, &solidBrush1);
	graph.ReleaseHDC(hdc);
}

void ReleaseRes()
{
	DelImage();
}

void DelImage()
{
	//删除位图对象
	DeleteObject(hBitmap_start);
	DeleteObject(hBitmap_play);
	DeleteObject(hBitmap_over);
	for (int i = 0; i < 3; i++)
	{
		DeleteObject(npcImgInfo[i].hBmp);
	}
}