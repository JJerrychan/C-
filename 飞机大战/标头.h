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
Player player; //���ս��
list <NPC> npcList;	//�л��б�
MenuItem StartMenu[2];	//��ʼ�����˵�
MenuItem OverMenu[2];	//���������˵�

void GameInit();	//��Ϸ��ʼ��
void LoadImage();	//������Ϸλͼ
void GamePaint(HDC hdc);	//�����ػ�
void DrawPlay(HDC hdc);	//�ػ�����Ϸ����
void DrawStarOver(HDC hdc, HBITMAP bkBitmp,MenuItem menu[]);	//�ػ濪ʼ&��������
void ReleaseRes();	//�ͷ���Դ
void DelImage();	//ɾ��λͼ��Դ
void MouseClick(HWND hwnd, int x, int y);	//����������¼�
void MouseMove(int x,int y);	//����ƶ�
void GameLogic();	//��Ϸ�߼�
void DrawMenu(HDC hdc, PTCHAR text, RectF rect, Color fontColor);	//����˵�����
void InitMenu(); //��ʼ����Ϸ�˵�
BOOL isPointInRectF(POINT pt, RectF rectf);	//�жϵ��Ƿ����ڲ˵�������ľ��ο�(RectF)��
void MenuFocus(POINT pt, MenuItem meun[]);	//���ò˵����Ƿ��ý���
void MenuClick(HWND hwnd, MenuItem menu[]);	//����˵���
void UpdateNpcPos();	//���µл�λ��
void ProduceNpc(int type);	//�����л�
void InitNpcStaticInfo();	//��ʼ�����ֵл��Ĺ�������
void LoadImageInfo(Bitmap* pBmp, TCHAR *path, ImgInfo *imgInfo);	//����λͼ����ߴ�
void InitPlayer();	//��ʼ�����ս��
void MouseMoveWithLButton(int x, int y);	//��갴��������ƶ��¼�
BOOL isCollsionWithRect(Rect rect1, Rect rect2);	//�ж����������Ƿ��ཻ
void CollsionDetection();	//��ײ���