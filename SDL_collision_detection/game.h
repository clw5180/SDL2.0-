#ifndef __Game__
#define __Game__

#include <SDL.h>

class CGame
{
public:

	static CGame* Instance()
	{
		if (pInstance == NULL)
		{
			pInstance = new CGame();
		}
		return pInstance;
	}

	// Initialize our SDL game / app
	bool Init();

	// main loop������
	void HandleEvents();
	void Update();
	void Render();

	// Free up resources
	void CleanUp();

	//=========================================================
	//Get��������
	//=========================================================
	static int GetWindowWidth() { return m_screenWidth; }
	static int GetWindowHeight() { return m_screenHeight; }
	SDL_Renderer* GetRenderer() { return Renderer; }
	//GameStateMachine* getStateMachine() { return m_pGameStateMachine; }

	// ����main loop��������The game is still running
	bool running() { return m_bRunning; }

	void SetGameOverFlag(bool gameover) { m_bGameOver = gameover; }
	bool GetGameOverFlag() { return m_bGameOver; }
	void SetScore(int score) { m_score = score; }
	int GetScore() { return m_score; }

	void quit() { m_bRunning = false; }


private:
	CGame() {}

	static CGame* pInstance;

	//GameStateMachine* m_pGameStateMachine;

	bool m_bRunning = true;
	bool m_bGameOver = false;

	SDL_Window* Window = NULL;
	SDL_Renderer* Renderer = NULL;

	//Screen dimension constants
	static const int m_screenWidth =  /*512*/ 768  /*1024*/ /*1536*/;
	static const int m_screenHeight =  /*384*/  576  /*768*/;

	int m_score = 0;
};
//clw note 20180819����������ֺ�ע����˻���֪��������

// create the typedef
typedef CGame TheGame;


#endif  /*defined(__Game__)*/
