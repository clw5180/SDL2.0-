#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>  //����ֱ��ʹ��SDL_image�⣬���Լ�����.jpg .png�ȸ��ָ�ʽ��ͼƬ
#include "game.h"
#include "inputhandler.h"
#include "global.h"
#include "map.h"
#include "player.h"

SDL_Window *g_pWindow = NULL;
SDL_Renderer *g_pRenderer = NULL;
SDL_Texture *g_pMapTexture = NULL;
SDL_Texture *g_pSpriteSheetTexture = NULL;

// ���ڿ��
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// ��ͼ���
const int MAP_WIDTH = 2560;
const int MAP_HEIGHT = 1200;

// ��̬��Ա�����ĳ�ʼ��
CGame* CGame::s_pInstance = NULL;

// ���캯��
CGame::CGame()
{
}

// ��������
CGame::~CGame()
{
}

//=========================================================
//�������ƣ�LoadImage
//˵��������ͼƬ���������
//=========================================================
bool LoadImage(const char* filename, SDL_Texture*& texture)
{
	if (filename == NULL)
	{
		printf("LoadImage() error: filename is NULL!\n");
		return false;
	}

	SDL_Surface *pTempSurface = IMG_Load(filename);  //��������ͼ��(splash image)��SDL_Surface����
	if (pTempSurface == NULL)
	{
		printf("Unable to load image and recommend to check the path of image file! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	texture = SDL_CreateTextureFromSurface(g_pRenderer, pTempSurface); //ʹ��SDL_Surface���󴴽�����(texture)
	if (texture == NULL)
	{
		printf("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	SDL_FreeSurface(pTempSurface);  //����surface����ռ�õ��ڴ�ռ�
	return true;
}


//=========================================================
//�������ƣ�Init
//˵�������SDL��ʼ�����������ڵ�
//=========================================================
bool CGame::Init()  //��ʼ��
{
	//��1����ʼ������SDL�ĸ�ģ��
	if (SDL_Init(SDL_INIT_VIDEO) < 0) //������Գ�ʼ��SDL�ĸ���ģ�飬������ʱ������Ƶ����Ƶ��ҡ�˵�
									  //ʵ����Ŀ��һ��ֻ��ʼ��Ҫ�õ���ģ�鼴�ɣ���������Щģ�����ͨ��
									  //��SDL_INIT_VIDEO�ϵ���Ҽ�-ת���������鿴���ɹ�����0
	{
		printf("Unable to Init SDL: %s", SDL_GetError()); //����SDL_GetError()���鿴ʧ��ԭ��
		return false;
	}

	//��2��ʹ��SDL��������
	g_pWindow = SDL_CreateWindow("Chapter 1: Hello World!",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (g_pWindow == NULL)
	{
		printf("Unable to Create Window: %s", SDL_GetError());
		return false;
	}

	//��3����ʼ����Ⱦ��(renderer)�������ڴ��ڻ�ͼ
	g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);
	if (g_pRenderer == NULL)
	{
		printf("Unable to Create Renderer: %s", SDL_GetError());
		return false;
	}

	//��4������ͼƬ
	if (!LoadImage("./res/town.png", g_pMapTexture))
	{
		printf("Failed to load image 'town.png'!\n");
		return false;
	}
	if (!LoadImage("./res/xiahouyi.png", g_pSpriteSheetTexture))
	{
		printf("Failed to load image 'xiahouyi.png'!\n");
		return false;
	}
	return true;
}

//=========================================================
//�������ƣ�HandleEvents
//˵���������豸�¼�����������ꡢ���̡��ֱ���
//=========================================================
void CGame::HandleEvents() 
{
	CInputHandler::Instance()->update();
}


//=========================================================
//�������ƣ�Update
//˵�������ݲ�ͬ���¼�����Ϸ״̬�����ݽ��и���
//=========================================================
void CGame::Update() 
{
	CPlayer::Instance()->Update();
	CMap::Instance()->Update();
}


//=========================================================
//�������ƣ�Render
//˵������Ⱦ�����ͼ��Ļ���
//=========================================================
void CGame::Render()  
{
	SDL_SetRenderDrawColor(g_pRenderer, 255, 255, 255, 0);  //������Ⱦ������ɫ����һ������������Ⱦ������
             												//������������R��G��Bֵ��Alphaֵ
    //����ͼƬ���師:SDL_RenderClear()��SDL_RenderCopy()��SDL_RenderPresent()
	SDL_RenderClear(g_pRenderer);  //ʹ��ĳ�ֻ�ͼ��ɫ�����������ǰ����

	CMap::Instance()->Render();
	CPlayer::Instance()->Render();

	SDL_RenderPresent(g_pRenderer);  //����Ⱦ���е������ڴ�������ʾ����
}


//=========================================================
//�������ƣ�Close
//˵�������ٸ�����
//=========================================================
void CGame::Close()
{
	//���ٴ��ڶ���
	SDL_DestroyWindow(g_pWindow);
	g_pWindow = NULL;  //��g_pWindow����ָ�򴰿�ʱ���ֶ���ΪNULL��

	//������Ⱦ������
	SDL_DestroyRenderer(g_pRenderer);
	g_pRenderer = NULL;

	//�����������
	SDL_DestroyTexture(g_pMapTexture);
	g_pMapTexture = NULL;
	SDL_DestroyTexture(g_pSpriteSheetTexture);
	g_pSpriteSheetTexture = NULL;

	//��������ѳ�ʼ������ϵͳ�����˳�SDL���ڳ������ʱ���á�
	SDL_Quit();  
}