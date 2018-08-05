 #include <windows.h>
#include"resource.h"
#include<gdiplus.h>
#include<list>
#pragma comment(lib,"gdiplus.lib")
#pragma comment(lib,"Msimg32.lib")

using namespace Gdiplus;
using namespace std;


#define CLIENTWIDTH 400
#define CLIENTHEIGHT 600
#define TIMERID 10
#define GAMEFRAME 25
#define MENU_ITEM_WIDTH 150
#define MENU_ITEM_HEIGHT 60

typedef enum GAMESCENE
{
	SCENE_START,
	SCENE_PLAY,
	SCENE_OVER
}GAMESCENE;
typedef struct ImgInfo
{
	HBITMAP hBmp;
	SIZE size;
}ImgInfo;
typedef struct MenuItem
{
	TCHAR itemText[5];
	RectF itemRect;
	BOOL isFocus;
}MenuItem;
typedef struct NPC
{
	int type;
	POINT pos;
}NPC;
typedef struct Player
{
	HBITMAP hBmp;
	SIZE size;
	POINT pos;
};
typedef struct NpcStaticInfo
{
	HBITMAP npcBitmap;
	SIZE size;
	int speed;
}NpcStaticInfo;

HBITMAP hBitmap_start,hBitmap_play,hBitmap_over;
ImgInfo npcImgInfo[3],PlayerImgInfo;
GAMESCENE curScene;

POINT prePoint;
static NpcStaticInfo npcStaticInfo[3];
int frameIndex;
Player player; //玩家战机
list <NPC> npcList;	//敌机列表
MenuItem StartMenu[2];	//开始场景菜单
MenuItem OverMenu[2];	//结束场景菜单

void GameInit();	//游戏初始化
void LoadImage();	//加载游戏位图
void GamePaint(HDC hdc);	//界面重绘
void DrawPlay(HDC hdc);	//重绘玩游戏场景
void DrawStarOver(HDC hdc, HBITMAP bkBitmp,MenuItem menu[]);	//重绘开始&结束场景
void ReleaseRes();	//释放资源
void DelImage();	//删除位图资源
void MouseClick(HWND hwnd, int x, int y);	//鼠标左键点击事件
void MouseMove(int x,int y);	//鼠标移动
void GameLogic();	//游戏逻辑
void DrawMenu(HDC hdc, PTCHAR text, RectF rect, Color fontColor);	//输出菜单文字
void InitMenu(); //初始化游戏菜单
BOOL isPointInRectF(POINT pt, RectF rectf);	//判断点是否落在菜单项关联的矩形框(RectF)内
void MenuFocus(POINT pt, MenuItem meun[]);	//设置菜单项是否获得焦点
void MenuClick(HWND hwnd, MenuItem menu[]);	//点击菜单项
void UpdateNpcPos();	//更新敌机位置
void ProduceNpc(int type);	//产生敌机
void InitNpcStaticInfo();	//初始化三种敌机的固有属性
void LoadImageInfo(Bitmap* pBmp, TCHAR *path, ImgInfo *imgInfo);	//加载位图及其尺寸
void InitPlayer();	//初始化玩家战机
void MouseMoveWithLButton(int x, int y);	//鼠标按下左键并移动事件
BOOL isCollsionWithRect(Rect rect1, Rect rect2);	//判断两个矩形是否相交
void CollsionDetection();	//碰撞检测