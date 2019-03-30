#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>

class CPlayer
{
public:
	static CPlayer* Instance()
	{
		if (s_pInstance == NULL)
			s_pInstance = new CPlayer();
		return s_pInstance;
	}

	int GetX() { return m_x; }
	int GetY() { return m_y; }
	int GetWidth() { return m_w; }
	int GetHeight() { return m_h; }

	void Render();
	void Update();

private:
	CPlayer();
	~CPlayer();

	static CPlayer* s_pInstance;

	enum PlayerDirection
	{
		PLAYERLEFT,
		PLAYERRIGHT,
		PLAYERUP,
		PLAYERDOWN,
		PLAYERLEFTDOWN,
		PLAYERRIGHTUP,
		PLAYERLEFTUP,
		PLAYERRIGHTDOWN,
	};


	// �Ͷ�����صı���
	int m_currentRow = 0;  //��ǰ֡���ھ����Sprite Sheet���ĵڶ�����
	int m_currentFrame = 0; //��ǰ֡���ھ����Sprite Sheet���ĵڶ�����
	int m_numFrames = 8;  
	int m_numRows = 9;  //׼������8�У�9�еĽ�ɫ�����     TODO�����濼���޸�����д��?
	int m_movedirection = PLAYERDOWN;
	int m_bWalkState = true;

	// �����н�ɫ�Ŀ�ߺͽ�ɫ��λ��
	int m_x = 0;
	int m_y = 0;
	int m_w = 448 / m_numFrames; //TODO��������ʱhard codeͼƬ�Ŀ�߳ߴ磬�����������������࣬
						   //ÿ��LoadͼƬʱ�ͻ�����ÿһ��ͼƬ�ĳߴ����Ϣ
	int m_h = 819 / m_numRows;  //TODO

	
};

#endif  /* defined(__PLAYER_H__)*/