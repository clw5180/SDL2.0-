//==============================================================================
#include "Game.h"
#include "TextureManager.h"
#include "inputhandler.h"
#include "MainMenuState.h"
#include "gameobjectfactory.h"
#include "Snake.h"
#include "SoundManager.h" 
#include "menubutton.h"
#include "AnimatedGraphic.h"
#include "playstate.h" //�������new�Ĳ������˵�������Ϸ״̬�����������Ҫinclude����

#include "Log.h"

Game* Game::pInstance = NULL;
const float Game::SCALE_X = m_screenWidth / 768.0f; //��̬������Ա�ĳ�ʼ��
const float Game::SCALE_Y = m_screenHeight / 576.0f;

bool Game::Init() 
{
	//Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		Log("Unable to Init SDL: %s", SDL_GetError());
		//printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		Log("Unable to Init hinting: %s", SDL_GetError());
	}

	if((Window = SDL_CreateWindow(
		"My SDL Game",
		/*SDL_WINDOWPOS_UNDEFINED*//*, SDL_WINDOWPOS_UNDEFINED,*/
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		m_screenWidth, m_screenHeight, SDL_WINDOW_SHOWN)
	) == NULL) {
		Log("Unable to create SDL Window: %s", SDL_GetError());
		return false;
	}

	if((Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) == NULL) {
	    Log("Unable to create renderer");
	    return false;
	}

	// Initialize image loading for PNGs
	if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		Log("Unable to init SDL_image: %s", IMG_GetError());
		return false;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		return false;
	}

	//Initialize SDL_ttf
	if (TTF_Init() == -1)
	{
		printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}


	//clw note 20180820����ͼƬ���ؽ�������m_textureMap����TextureManager���map<std::string, SDL_Texture*>����
	
	//clw note 20180916��������StateParser���parseObjects���������Texture��ͼƬ��صļ��ض�����Ҫ����������ˣ�����תΪ����changeState����������Ϸ״̬�������
	
	//if (!TextureManager::Instance()->AddImage(Renderer, "snake", "./res/image.png"))
	//{
	//	cout << "fail to AddImage!" << endl;
	//}


	//clw note 20180824�������ּ��ؽ�����ͬ������m_textureMap
	if (!TextureManager::Instance()->AddText(Renderer, "score", "./res/lazy.ttf"))
	{
		cout << "fail to AddText!" << endl;
	}

	//clw note 20180825�������ּ��ؽ���������SoundManager��
	if (!SoundManager::Instance()->AddMusic("./res/backgroundmusic.wav", "BackgroundMusic", SOUND_MUSIC)   || 
		!SoundManager::Instance()->AddMusic("./res/gameover.mp3", "GameOver", SOUND_MUSIC)                 ||
		!SoundManager::Instance()->AddMusic("./res/mousemovetobutton.wav", "MouseMoveToButton", SOUND_SFX) ||
		!SoundManager::Instance()->AddMusic("./res/mouseclickbutton.wav", "MouseClickButton", SOUND_SFX))
	{
		cout << "fail to AddMusic!" << endl;
	}

	//Play the music  clw note ������Ϸ��
	//SoundManager::Instance()->playMusic("BackgroundMusic", -1);

	// register the types for the game
	TheGameObjectFactory::Instance()->registerType("MenuButton", new MenuButtonCreator());
	TheGameObjectFactory::Instance()->registerType("Snake", new SnakeCreator());
	TheGameObjectFactory::Instance()->registerType("AnimatedGraphic", new AnimatedGraphicCreator());

	// start the menu state
	m_pGameStateMachine = new GameStateMachine();
	//m_pGameStateMachine->changeState(new PlayState());
	m_pGameStateMachine->changeState(new MainMenuState());

	cout << "init success\n";
	m_bRunning = true; // everything inited successfully, start the main loop

	return true;
}

//=========================================================
//�������ƣ�HandleEvents
//˵����
//�޸ļ�¼��
//=========================================================
void Game::HandleEvents()
{
	TheInputHandler::Instance()->update();
}

//=========================================================
//�������ƣ�Update
//˵����
//�޸ļ�¼��
//=========================================================
void Game::Update()
{
	m_pGameStateMachine->update();
}

//=========================================================
//�������ƣ�Render
//˵����
//�޸ļ�¼��
//=========================================================
void Game::Render() 
{
	// clear the renderer to the draw color
	SDL_SetRenderDrawColor(Renderer, 0x00, 0x00, 0x00, 0xFF);

	//Draw the image
	SDL_RenderClear(Renderer);

	m_pGameStateMachine->render();

	// draw to the screen
	SDL_RenderPresent(Renderer);
}



//=========================================================
void Game::CleanUp() 
{
#if _DEBUG
	cout << "cleaning game\n";
#endif

	//Mix_FreeMusic(gMusic);  //clw note 20180825���Ƿ��б�Ҫ�����Ǻ��ڼ��ϣ�����ѧϰTextureManager��д��

	if(Renderer) 
	{
		SDL_DestroyRenderer(Renderer);
		Renderer = NULL;
	}

	if(Window) 
	{
		SDL_DestroyWindow(Window);
		Window = NULL;
	}

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}
//=========================================================
