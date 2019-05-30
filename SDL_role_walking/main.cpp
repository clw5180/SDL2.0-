#include "game.h"
#include <SDL.h>

//������Ϸ֡�ʣ�FPS��
const int FPS = 60;
const int DELAY_TIME = 1000.0f / FPS;

//=========================================================
//�������ƣ�main
//˵����������
//=========================================================
int main(int argc, char* argv[]) //ע�����������main�������ڼ���int argc, char* argv[]���򱨴�"LNK2019�޷��������ⲿ���� _SDL_main"
{
	Uint32 frameStart, frameTime; 

	if (!CGame::Instance()->Init())  //Step1����ʼ��SDL
	{
		printf("Failed to Init!\n");
		return -1;
	}

	while (CGame::Instance()->Running())
	{
		frameStart = SDL_GetTicks();

		
		CGame::Instance()->HandleEvents();  //Step2���¼�����
		CGame::Instance()->Update();  //Step3������״̬
		CGame::Instance()->Render();  //Step4����Ⱦ������ͼ��

		frameTime = SDL_GetTicks() - frameStart;
		if (frameTime < DELAY_TIME)
		{
			SDL_Delay((int)(DELAY_TIME - frameTime));
		}
	}

	CGame::Instance()->Close();  //Step5�����������ͷ��ڴ�
	return 0;
}