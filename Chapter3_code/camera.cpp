#include "camera.h"
#include "inputhandler.h" 
#include "global.h"

CCamera* CCamera::s_pInstance = NULL;

CCamera::CCamera()
{
}

CCamera::~CCamera()
{
}

//=========================================================
//�������ƣ�Update
//˵������game.cpp�е�Update�����б����ã����ݼ��̡���������
//      ��ͬ�����������������
//=========================================================
void CCamera::Update()
{
	//handle keys
	//ע�⣺�����ɫ�����ߣ��൱������������ߣ�
	//     ������˵����棻
	if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) /*&& m_position.getY() > 0*/)
	{
		m_y += 3;
		if (m_y > 0)
			m_y = 0;
	}
	else if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) /*&& (m_position.getY() + m_height) < CGame::Instance()->GetWindowHeight() - 10*/)
	{
		m_y -= 3;
		if (m_y < -SCREEN_HEIGHT)
			m_y = -SCREEN_HEIGHT;
	}
	if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) /*&& m_position.getX() > 0*/)
	{
		m_x += 3;
		if(m_x > 0)
			m_x = 0;
	}
	else if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT) /*&& (m_position.getX() + m_width) < CGame::Instance()->GetWindowWidth()*/)
	{
		m_x -= 3;
		if (m_x < -SCREEN_WIDTH)
	    	m_x = -SCREEN_WIDTH;
	}
}