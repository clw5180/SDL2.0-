#include "player.h"
#include "inputhandler.h"
#include "global.h"  //for SCREEN_WIDTH��SCREEN_HEIGHT
#include "map.h"     //�����ɫ���ߵ���ͼ�߽������


int g_moveVelocity = 4; //clw note��debug

CPlayer* CPlayer::s_pInstance = NULL;

CPlayer::CPlayer()
{
	m_x = (SCREEN_WIDTH - m_w) / 2;
	m_y = (SCREEN_HEIGHT - m_h) / 2;
}

CPlayer::~CPlayer()
{
}

void CPlayer::Update()
{
	/*
	  ���ݽ�ɫλ�õĲ�ͬ����ɫ�ڵ�ͼ�ϵ��ƶ��ɷ�Ϊ�����ࣺ
	  ��1����ɫ�ڵ�ͼ�м�ʱ���ý�ɫ��ʾ����Ļ�м�λ�ã���
	  m_x = (SCREEN_WIDTH - m_w) / 2;
	  m_y = (SCREEN_HEIGHT - m_h) / 2;
	  ��2����ɫ�ڵ�ͼ��Եλ�ã�����/��/��/�ң�
	  �ڼ�⵽��ɫλ�ڵ�ͼ��Ե��Ӧ��֤��ͼ��������ɫ�뿪��������λ�ã�
	  �����Ϊ���ӣ����¡�
	                                                                  */
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LSHIFT))  //�������shift�����ܶ�ģʽ
	{ 
		g_moveVelocity = 5;  //�ܶ��ٶ�
		m_bWalkState = false;
	}
	else
	{ 
		g_moveVelocity = 3;  //�����ٶ�
		m_bWalkState = true;
	}

	//���δ������ҡ��ϡ������ְ���
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT)) //������
	{
		m_bWalkState ? m_currentRow = 4 : m_currentRow = 8;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2)));
		m_movedirection = PLAYERLEFT;
		//��ɫ�ڵ�ͼ������ҵ�λ�ã�������
		m_x -= g_moveVelocity; //���ڵ�ͼ��������򣺽�ɫ�뿪��������λ�ü��������ƶ������ڵ�ͼ���Ҳ����򣺽�ɫ���ұ������ĵ�λ�÷��ء�
		if (CMap::Instance()->GetX() >= 0) //����Ե����  TODO��������ڵ��ڸĳɵ��ڣ���
		{	
			if (m_x < 0)   //��ֹ���������ʱ����ɫ�����ƶ�����ͼ��Ե֮��
				m_x = 0;
		}
		else if (CMap::Instance()->GetX() <= -(MAP_WIDTH - SCREEN_WIDTH)) //�Ҳ��Ե����
		{
			if (m_x < (SCREEN_WIDTH - m_w) / 2)   //��ֹ��ɫ�����ƶ������������ĵ�
				m_x = (SCREEN_WIDTH - m_w) / 2;
		}
		else
		{
			m_x = (SCREEN_WIDTH - m_w) / 2;
		}
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT)) //������
	{
		m_bWalkState ? m_currentRow = 2 : m_currentRow = 6;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2)));
		m_movedirection = PLAYERRIGHT;
		m_x += g_moveVelocity; //���ڵ�ͼ��������򣺽�ɫ����������ĵ�λ�÷��ء����ڵ�ͼ���Ҳ����򣺽�ɫ�뿪��������λ�ü��������ƶ���
		if (CMap::Instance()->GetX() >= 0) //����Ե����
		{
			if (m_x > (SCREEN_WIDTH - m_w) / 2)  //��ֹ��ɫ�����ƶ������������ĵ�
				m_x = (SCREEN_WIDTH - m_w) / 2; 
		}
		else if (CMap::Instance()->GetX() <= -(MAP_WIDTH - SCREEN_WIDTH))//�Ҳ��Ե����
		{
			if (m_x > SCREEN_WIDTH - m_w)   //��ֹ���Ҳ�����ʱ����ɫ�����ƶ�����ͼ��Ե֮��
				m_x = SCREEN_WIDTH - m_w;
		}
		else
		{
			m_x = (SCREEN_WIDTH - m_w) / 2;
		}
	}

	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP)) //������
	{
		m_bWalkState ? m_currentRow = 3 : m_currentRow = 7;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2)));
		m_movedirection = PLAYERUP;
		m_y -= g_moveVelocity;
		if (CMap::Instance()->GetY() >= 0)   //�ϱ�Ե����
		{
			if (m_y < 0)    //��ֹ��ɫ�����ƶ�����ͼ��Ե֮��
				m_y = 0;
		}
		else if (CMap::Instance()->GetY() <= -(MAP_HEIGHT - SCREEN_HEIGHT)) //�±�Ե����
		{
			if (m_y < (SCREEN_HEIGHT - m_h) / 2)  //��ֹ�����߳����������ĵ�
				m_y = (SCREEN_HEIGHT - m_h) / 2;
		}
		else
		{
			m_y = (SCREEN_HEIGHT - m_h) / 2;
		}
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN)) //������
	{
		m_bWalkState ? m_currentRow = 1 : m_currentRow = 5;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2)));
		m_movedirection = PLAYERDOWN;
		m_y += g_moveVelocity;
		if (CMap::Instance()->GetY() >= 0)  //�ϱ�Ե����
		{
			if (m_y > (SCREEN_HEIGHT - m_h) / 2)
				m_y = (SCREEN_HEIGHT - m_h) / 2; //��ֹ�����߳����������ĵ�
		}
		else if (CMap::Instance()->GetY() <= -(MAP_HEIGHT - SCREEN_HEIGHT))  //�±�Ե����
		{
			if (m_y > SCREEN_HEIGHT - m_h)   //��ֹ��ɫ�����ƶ�����ͼ��Ե֮��
				m_y = SCREEN_HEIGHT - m_h;
		}
		else
		{
			m_y = (SCREEN_HEIGHT - m_h) / 2;
		}
	}

	//���δ������£����ϣ����ϣ�����
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) &&
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN))
	{
		m_bWalkState ? m_currentRow = 4 : m_currentRow = 8;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2) + (m_numFrames / 2)));
		m_movedirection = PLAYERLEFTDOWN;
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT) &&
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP))
	{
		m_bWalkState ? m_currentRow = 2 : m_currentRow = 6;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2) + (m_numFrames / 2)));
		m_movedirection = PLAYERRIGHTUP;
	}
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) &&
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT))
	{
		m_bWalkState ? m_currentRow = 3 : m_currentRow = 7;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2) + (m_numFrames / 2)));
		m_movedirection = PLAYERLEFTUP;
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) &&
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT))
	{
		m_bWalkState ? m_currentRow = 1 : m_currentRow = 5;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2) + (m_numFrames / 2)));
		m_movedirection = PLAYERRIGHTDOWN;
	}

	//�������Ҷ�û���£�������֮ǰ�ƶ��ķ���ֹ
	if (!CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT)
		&& !CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT)
		&& !CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP)
		&& !CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN))
	{
		m_currentRow = 0;
		switch (m_movedirection)
		{
		case PLAYERLEFT:
			m_currentFrame = 6;
			break;
		case PLAYERRIGHT:
			m_currentFrame = 2;
			break;
		case PLAYERUP:
			m_currentFrame = 4;
			break;
		case PLAYERDOWN:
			m_currentFrame = 0;
			break;
		case PLAYERLEFTDOWN:
			m_currentFrame = 7;
			break;
		case PLAYERRIGHTUP:
			m_currentFrame = 3;
			break;
		case PLAYERLEFTUP:
			m_currentFrame = 5;
			break;
		case PLAYERRIGHTDOWN:
			m_currentFrame = 1;
			break;
		default:
			break;
		}
	}
}

void CPlayer::Render()
{
	//Render current frame
    //size of SpriteSheet: 448 * 819 = (56*8) * (91*9)        
	SDL_Rect srcRect;
	SDL_Rect destRect;

	srcRect.x = m_w * m_currentFrame;
	srcRect.y = m_h * m_currentRow;
	srcRect.w = destRect.w = m_w;
	srcRect.h = destRect.h = m_h;

	destRect.x = m_x;
	destRect.y = m_y;

	SDL_RenderCopy(g_pRenderer, g_pSpriteSheetTexture, &srcRect, &destRect);
}