//
//  GameOverState.cpp
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 17/02/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//

#include "GameOverState.h"
#include "MainMenuState.h"
#include "PlayState.h"
#include "TextureManager.h"
#include "Game.h"
#include "MenuButton.h"
#include "InputHandler.h"
#include "StateParser.h"
#include "SoundManager.h"

const std::string GameOverState::s_gameOverID = "GAMEOVER";

void GameOverState::s_gameOverToMain()
{
    TheGame::Instance()->getStateMachine()->changeState(new MainMenuState());
}

void GameOverState::s_restartPlay()
{
    TheGame::Instance()->getStateMachine()->changeState(new PlayState());
}

void GameOverState::update()
{
    if(m_loadingComplete && !m_gameObjects.empty())
    {
        for(vector<GameObject*>::size_type i = 0; i < m_gameObjects.size(); i++)
        {
            m_gameObjects[i]->update();
        }
    }
}

void GameOverState::render()
{
    if(m_loadingComplete && !m_gameObjects.empty())
    {
        for(vector<GameObject*>::size_type i = 0; i < m_gameObjects.size(); i++)
        {
            m_gameObjects[i]->draw();
        }
    }
}

bool GameOverState::onEnter()
{
	//clw modify 20180918 ��Ϸʧ�ܲ�������
	SDL_Delay(300);
	SoundManager::Instance()->playMusic("GameOver", 0);

    // parse the state
    StateParser stateParser;
    stateParser.parseState("test.xml", s_gameOverID, &m_gameObjects, &m_textureIDList);
    m_callbacks.push_back(0);
    m_callbacks.push_back(s_gameOverToMain);
    m_callbacks.push_back(s_restartPlay);
    
    // set the callbacks for menu items
    setCallbacks(m_callbacks);
    
    m_loadingComplete = true;
    
    std::cout << "entering GameOverState\n";
    return true;
}

bool GameOverState::onExit()
{
    if(m_loadingComplete && !m_gameObjects.empty())
    {
        for(vector<GameObject*>::size_type i = 0; i < m_gameObjects.size(); i++)
        {
            m_gameObjects[i]->clean();
            delete m_gameObjects[i];
        }
        
        m_gameObjects.clear();
    }
    
    std::cout << m_gameObjects.size();
    
    // clear the texture manager
    for(vector<string>::size_type i = 0; i < m_textureIDList.size(); i++)
    {
        TheTextureManager::Instance()->clearFromTextureMap(m_textureIDList[i]);
    }
    
    TheInputHandler::Instance()->reset();
    

	/***************************************************************/
	TheGame::Instance()->SetScore(0); //��Ϸ�������㣬����restart��gameoverstateת��playstateʱ��m_score�����Ļ�����һ��ķ�������

	//Render text
	//clw note 20180918��ֻ�����ʼ��ʼ��ʱͨ��AddText����������һ��LoadFromRenderedText��֮�����score����ʱ�Ż���ã�����Ϸ������Ҫ���¿�ʼʱ����Ҫ���ʼ��ʱ�����������µ���LoadFromRenderedText
	SDL_Color textColor = { 0xFF, 0xFF, 0xFF };
	string str = "score : 0";
	if (TheTextureManager::Instance()->LoadFromRenderedText(TheGame::Instance()->GetRenderer(), "score", str, textColor) == false)
	{
		printf("Unable to LoadFromRenderedText Texture: %s", "score");
		return false;
	}
	/***************************************************************/

    std::cout << "exiting GameOverState\n";
    return true;
}

void GameOverState::setCallbacks(const std::vector<Callback>& callbacks)
{
    // go through the game objects
    for(vector<GameObject*>::size_type i = 0; i < m_gameObjects.size(); i++)
    {
        // if they are of type MenuButton then assign a callback based on the id passed in from the file
        if(dynamic_cast<MenuButton*>(m_gameObjects[i]))
        {
            MenuButton* pButton = dynamic_cast<MenuButton*>(m_gameObjects[i]);
            pButton->setCallback(callbacks[pButton->getCallbackID()]);
        }
    }
}

