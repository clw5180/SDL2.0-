//==============================================================================
/*
	̰���� made by clw 
	20180818 ~ 20180821
	���м����ط����Խ�һ���޸ģ�
	����1������fps���޸Ĳ���SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	       ���Ƿ��б仯 ���� ����ֻ����fps���У���������һ���н��ٶ�velocity��
	       ֡��������Ĭ�ϵ�һ��60֡���൱��16.6ms�ͻ��һ�Σ����һ���ƶ�32�����ص㣬
		   ��ô1s���ƶ�32*60���أ�̫���ˣ����ǲ����С֡�ʣ�Ҳ���ܼ�Сÿ���ƶ�������ֵ��
		   ��˿��Ǽ���һ��count������count==30 / velocity��ʱ��Ž���Update��
		   Ȼ��count���㣻����count++�����velocity=1����ô�൱��1s��count60�Σ�
		   �൱��1s�ƶ�64�����ص㡣

	����2�����ӵ÷���ʾ����һ��ʳ��+10�֣���Ҫ��һ��ѧϰttf
	��   3��ʳ��λ����Ȼ��������ֵģ�����Ӧ�ó������������ͷ
	��� 4�����Ӵ���������Сͼ��
	��� 5������tile�ڱ߽紦
	��   6�����������������н�������������ʱ��Ӧ���ж�Ϊ��ײ����Ӷ�������Ϸ


	�ѽ�� 1����ͨ�������ٶȣ�Ȼ����Update�ڼƴ�m_countΪ60/velocity�����������ǻ���Update��ס����û�и��õİ취�����̣߳���
	�ѽ�� 2��
	�ѽ�� 6������Update�ڼ�����Ӧ���жϼ��ɣ�

	20180913
	���԰ѿ���̨������������ �������� ��Ŀ����-����-ϵͳ-����̨

	20180918
	���������¼��������Ż���������Ŀǰ�Ǵ�һ����ǰ���¼�ֵ�Ľ�ͼ��Ҳ���ǿ���ͬʱ����������������֮ǰ����switch(e.key.keysym.sym)��ֻ��һ��·���ߣ�
	������Կ�����Vector2D���͵��ٶȱ�����ͨ��������x��y���ж��������ң�
	���Ƕ��̣߳�
*/
//==============================================================================
#include "Game.h"

const int FPS = 60;
const float DELAY_TIME = 1000.0f / FPS;

int main(int argc, char* argv[]) 
{
	Uint32 frameStart, frameTime;

	if (!Game::Instance()->Init())
		return -1;

	while (Game::Instance()->running())
	{
		frameStart = SDL_GetTicks();
		
		Game::Instance()->HandleEvents();
		Game::Instance()->Update();
		Game::Instance()->Render();

		frameTime = SDL_GetTicks() - frameStart;

		if (frameTime < DELAY_TIME)
		{
			SDL_Delay((int)(DELAY_TIME - frameTime));
		}
	}

	Game::Instance()->CleanUp();

	return 0;
}
