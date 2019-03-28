#include "map.h"
#include "inputhandler.h" 
#include "global.h"

CMap* CMap::s_pInstance = NULL;

CMap::CMap()
{
}

CMap::~CMap()
{
}

//=========================================================
//�������ƣ�Update
//˵������game.cpp�е�Update�����б����ã����ݼ��̡���������
//      ��ͬ����������Map����
//=========================================================
void CMap::Update()
{
	//handle keys
	//ע�⣺�����ɫ�����ߣ����Կ�����ɫ��������screen image�����ƶ���
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) /*&& m_position.GetY() > 0*/)
	{
		m_y += 4;
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) /*&& (m_position.GetY() + m_height) < CGame::Instance()->GetWindowHeight() - 10*/)
	{
		m_y -= 4;
	}
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) /*&& m_position.GetX() > 0*/)
	{
		m_x += 4;
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT) /*&& (m_position.GetX() + m_width) < CGame::Instance()->GetWindowWidth()*/)
	{
		m_x -= 4;
	}

	//clw note��for debug�������ڴ�ӡ������Ϣ
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) ||
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) ||
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) ||
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT))
	{
		std::cout << "x_Map:" << m_x << "  ";  //clw note��for debug
        std::cout << "y_Map:" << m_y << std::endl;
		std::cout << "x_Role:" << (SCREEN_WIDTH - 56) / 2 - m_x << "  ";   //TODO��һ֡����Ŀ�ߣ�ȥ��hard code
		std::cout << "y_Role:" << (SCREEN_HEIGHT - 91) / 2 - m_y << std::endl;
	}
}

void CMap::Render()
{
	SDL_Rect srcRect = { 0, 0, MAP_WIDTH, MAP_HEIGHT }; //TODO���½�һ��������CTexture����LoadImage()��ʱ���ͼƬ�����б�
										 //      ����¼ͼƬ�ߴ磬���������MAP_WIDTH��MAP_HEIGHT

//��ɫ�ڵ�ͼ����λ��ʱ����ͼ�����������ʾ����ɫ�ڵ�ͼ����λ��ʱ����ͼ�������ұ���ʾ��
//���·���ͬ��
	int tempX = CMap::Instance()->GetX();
	int tempY = CMap::Instance()->GetY();
	if (tempX > 0)
		tempX = 0;
	if (tempX < -(MAP_WIDTH - SCREEN_WIDTH))
		tempX = -(MAP_WIDTH - SCREEN_WIDTH);
	if (tempY > 0)
		tempY = 0;
	if (tempY < -(MAP_HEIGHT - SCREEN_HEIGHT))
		tempY = -(MAP_HEIGHT - SCREEN_HEIGHT);
	SDL_Rect destRect = { tempX, tempY, MAP_WIDTH, MAP_HEIGHT }; //TODO���滻�����2560��1200
	SDL_RenderCopy(g_pRenderer, g_pMapTexture, &srcRect, &destRect); //�������Ƶ���Ⱦ���У�
}