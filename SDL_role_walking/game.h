#ifndef __GAME_H__
#define __GAME_H__

#include <iostream>

class CGame
{
public:
	static CGame* Instance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CGame();
		}
		return s_pInstance;
	}

	bool Init(); //��ʼ��
	void HandleEvents(); //�����¼�����������ꡢ���̡��ֱ���
	void Update();  //���ݲ�ͬ���¼�����Ϸ״̬�����ݽ��и���
	void Render();  //��Ⱦ�����ͼ��Ļ���
	void Close();  //�ͷ��ڴ棬����

	bool Running() { return m_bRunning; }
	void Quit() { m_bRunning = false; }

private:
	static CGame* s_pInstance;

	CGame();
	~CGame();

	bool m_bRunning = true;
};

#endif   /* defined(__GAME_H__)*/