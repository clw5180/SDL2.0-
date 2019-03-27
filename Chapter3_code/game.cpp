#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>  //����ֱ��ʹ��SDL_image�⣬���Լ�����.jpg .png�ȸ��ָ�ʽ��ͼƬ
#include "game.h"
#include "inputhandler.h"
#include "global.h"
#include "camera.h"

SDL_Window *g_pWindow = NULL;
SDL_Renderer *g_pRenderer = NULL;
SDL_Texture *g_pTexture = NULL;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// ��̬�����ĳ�ʼ��
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
bool LoadImage(const char* filename)
{
	if (filename == NULL)
	{
		printf("LoadImage() error: filename is NULL!\n");
		return false;
	}

	SDL_Surface *pTempSurface = IMG_Load(filename);  //��������ͼ��(splash image)��SDL_Surface����
	if (pTempSurface == NULL)
	{
		printf("Unable to load image! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	g_pTexture = SDL_CreateTextureFromSurface(g_pRenderer, pTempSurface); //ʹ��SDL_Surface���󴴽�����(texture)
	if (g_pTexture == NULL)
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
	if (!LoadImage("./res/town.png"))
	{
		printf("Failed to load image!\n");
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
	CCamera::Instance()->Update();
	//TODO
}


//=========================================================
//�������ƣ�Render
//˵������Ⱦ�����ͼ��Ļ���
//=========================================================
void CGame::Render()  
{
	SDL_SetRenderDrawColor(g_pRenderer, 255, 255, 255, 0);  //������Ⱦ������ɫ����һ������������Ⱦ������
														//������������R��G��Bֵ��Alphaֵ
//����ͼƬ���師
	SDL_RenderClear(g_pRenderer);  //ʹ��ĳ�ֻ�ͼ��ɫ�����������ǰ����
    
	//SDL_Rect srcRect = { CCamera::Instance()->GetX(), CCamera::Instance()->GetY(), SCREEN_WIDTH, SCREEN_HEIGHT }; //TODO���½�һ��������CTexture����LoadImage()��ʱ���ͼƬ�����б�
	//                                         //      ����¼ͼƬ�ߴ磬���������1600��1680
	//SDL_Rect destRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	SDL_Rect srcRect = { 0, 0, 1600, 1680 }; //TODO���½�һ��������CTexture����LoadImage()��ʱ���ͼƬ�����б�
	                                         //      ����¼ͼƬ�ߴ磬���������1600��1680
	SDL_Rect destRect = { CCamera::Instance()->GetX(), CCamera::Instance()->GetY(), 1600, 1680 };
	SDL_RenderCopy(g_pRenderer, g_pTexture, &srcRect, &destRect); //�������Ƶ���Ⱦ���У�
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
	g_pWindow = NULL;

	//������Ⱦ������
	SDL_DestroyRenderer(g_pRenderer);
	g_pRenderer = NULL;

	//�����������
	SDL_DestroyTexture(g_pTexture);
	g_pTexture = NULL;

	//��������ѳ�ʼ������ϵͳ���ڳ������ʱ���á�
	SDL_Quit();
}