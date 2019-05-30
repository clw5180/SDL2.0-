#ifndef __GAMESTATE__
#define __GAMESTATE__

#include <string>
#include <vector>
#include <iostream>

using namespace std;

class GameState
{
public:
	
	virtual ~GameState() {}

	virtual void update() = 0;
	virtual void render() = 0;
	virtual bool onEnter() = 0;
	virtual bool onExit() = 0;

	virtual std::string getStateID() const = 0;

protected:

	GameState() : m_loadingComplete(false), m_exiting(false)
	{

	}

	bool m_loadingComplete;
	bool m_exiting;

	vector<string> m_textureIDList;  //clw note������MenuState��onEnter()�ڽ����������һ����������һ�뺬��
};

#endif