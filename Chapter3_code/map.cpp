#include "map.h"
#include "inputhandler.h" 
#include "global.h"
#include "player.h"

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
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) && m_y < 0 
		&& CPlayer::Instance()->GetY() == (SCREEN_HEIGHT - CPlayer::Instance()->GetHeight()) / 2)
	{
		m_y += g_moveVelocity;
		if (m_y > 0)
			m_y = 0; //clw note����ͼλ����������Ҫ�����߽�
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) && m_y > -(MAP_HEIGHT - SCREEN_HEIGHT)
		&& CPlayer::Instance()->GetY() == (SCREEN_HEIGHT - CPlayer::Instance()->GetHeight()) / 2)
	{
		m_y -= g_moveVelocity;
		if (m_y < -(MAP_HEIGHT - SCREEN_HEIGHT)) 
			m_y = -(MAP_HEIGHT - SCREEN_HEIGHT);
	}
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) && m_x < 0 
		&& CPlayer::Instance()->GetX() == (SCREEN_WIDTH - CPlayer::Instance()->GetWidth()) / 2)
	{
		m_x += g_moveVelocity;
		if (m_x > 0)
			m_x = 0;
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT) && m_x > -(MAP_WIDTH - SCREEN_WIDTH) 
		&& CPlayer::Instance()->GetX() == (SCREEN_WIDTH - CPlayer::Instance()->GetWidth()) / 2) //��ɫ����ڵ�ͼ�����λ�ã����������ߵ��������ĵ�����ƶ���ͼ
	{
		m_x -= g_moveVelocity;
		if (m_x < -(MAP_WIDTH - SCREEN_WIDTH))
			m_x = -(MAP_WIDTH - SCREEN_WIDTH);
	}

	//clw note��for debug�������ڴ�ӡ������Ϣ
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) ||
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) ||
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) ||
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT))
	{
		std::cout << "x_Map:" << m_x << "  ";  //clw note��for debug
        std::cout << "y_Map:" << m_y << std::endl;
	}
}

void CMap::Render()
{
	SDL_Rect srcRect = { 0, 0, MAP_WIDTH, MAP_HEIGHT }; //TODO���½�һ��������CTexture����LoadImage()��ʱ���ͼƬ�����б�
										 //      ����¼ͼƬ�ߴ磬���������MAP_WIDTH��MAP_HEIGHT
	SDL_Rect destRect = { m_x, m_y, MAP_WIDTH, MAP_HEIGHT }; //TODO���滻�����2560��1200
	SDL_RenderCopy(g_pRenderer, g_pMapTexture, &srcRect, &destRect); //�������Ƶ���Ⱦ���У�
}