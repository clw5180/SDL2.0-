#include "player.h"
#include "inputhandler.h"
#include "global.h"  //for SCREEN_WIDTH��SCREEN_HEIGHT
#include "map.h"     //�����ɫ���ߵ���ͼ�߽������

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
	//���ݽ�ɫλ�õĲ�ͬ����ɫ�ڵ�ͼ�ϵ��ƶ��ɷ�Ϊ3�������
	//��1��������ɫ�����ڵ�ͼ�м䣺�ý�ɫ��ʾ����Ļ�м�λ�ã���
	//m_x = (SCREEN_WIDTH - m_w) / 2;
	//m_y = (SCREEN_HEIGHT - m_h) / 2;

	//���δ������ҡ��ϡ���
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT))
	{
		m_currentRow = 4;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2)));
		m_movedirection = PLAYERLEFT;
		//��2����ɫ�ڵ�ͼ������ҵ�λ��ʱ
		if (CMap::Instance()->GetX() >= 0 || CMap::Instance()->GetX() <= -(MAP_WIDTH - SCREEN_WIDTH))
		{
			m_x -= 4; // �����ɫ���������ƶ���
		}
		else
		{
			m_x = (SCREEN_WIDTH - m_w) / 2;
		}
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT))
	{
		m_currentRow = 2;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2)));
		m_movedirection = PLAYERRIGHT;
		if (CMap::Instance()->GetX() <= -(MAP_WIDTH - SCREEN_WIDTH) || CMap::Instance()->GetX() >= 0) //��ɫ�ڵ�ͼ����λ��ʱ
		{
			m_x += 4;  //�����ɫ���������ƶ���ͬ��
		}
		else
		{
			m_x = (SCREEN_WIDTH - m_w) / 2;
		}
	}
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP))
	{
		m_currentRow = 3;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2)));
		m_movedirection = PLAYERUP;
		//��3����ɫ�ڵ�ͼ���ϻ��µ�λ�ã������ɫ�������ϻ������ƶ���ͬ��
		if (CMap::Instance()->GetY() >= 0 || CMap::Instance()->GetY() <= -(MAP_HEIGHT - SCREEN_HEIGHT))
		{
			m_y -= 4;
		}
		else
		{
			m_y = (SCREEN_HEIGHT - m_h) / 2;
		}
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN))
	{
		m_currentRow = 1;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2)));
		m_movedirection = PLAYERDOWN;
		if (CMap::Instance()->GetY() <= -(MAP_HEIGHT - SCREEN_HEIGHT) || CMap::Instance()->GetY() >= 0)
		{
			m_y += 4;
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
		m_currentRow = 4;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2) + (m_numFrames / 2)));
		m_movedirection = PLAYERLEFTDOWN;
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT) &&
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP))
	{
		m_currentRow = 2;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2) + (m_numFrames / 2)));
		m_movedirection = PLAYERRIGHTUP;
	}
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) &&
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT))
	{
		m_currentRow = 3;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2) + (m_numFrames / 2)));
		m_movedirection = PLAYERLEFTUP;
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) &&
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT))
	{
		m_currentRow = 1;
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