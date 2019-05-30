#ifndef __SNAKE_H__
#define __SNAKE_H__

#include "SDLGameObject.h"
#include "inputhandler.h"
#include "gameobjectfactory.h"
#include "Log.h"

enum MoveDirection 
{ 
	MOVEUP = 1, 
	MOVEDOWN, 
	MOVELEFT, 
	MOVERIGHT 
};

class Snake : public SDLGameObject
{
public:
	Snake();
	virtual ~Snake();

	virtual void load(std::unique_ptr<LoaderParams> const &pParams);

	virtual void update();
	virtual void draw();
	virtual void clean();

	void handleInput();

	virtual std::string type() { return "Player"; }

	//int GetDiameter() { return m_diameter; }

private:
	//=========================================================
	//�ӳ�ʽ����
	//=========================================================
	void RandomGenerateFood(); //�������ʳ���λ��
	void moveSnakeByDirection();  //update�Ӻ���

	// С���ƶ������������ҷֱ���1��2��3��4��ʾ
	int m_moveDirection;

	//velocity = 1�൱��1s��m_diameter * 1�ľ��롣��Ϊ��Game::Update�������ж�m_count > 60 / m_velocity��������Fps��60����1s��ִ��60��Update������
	int m_velocity = 4;
	static const int m_velocityMax = 32;

	//Setup the clips for our image
	//clw note 20180819��������һ���ṹ�����飬ÿһ��clip������x,y,w,h�ĸ�������ǡ�ÿ�ѡsprite list��һ��sprite
	SDL_Rect clips[2];
	//int useClip = 0;
	int m_iW = 0;
	int m_iH = 0;

	int m_DstX = 0;
	int m_DstY = 0;

	//clw note 20180820���������Ҫд��static const����ʽ
	//                   ���ֻдstatic����ʾ"�������ڳ�ʼֵ�趨��ĳ�Ա����Ϊ����"��
	//                   ���ֻдconst����ô�����ڲ����ó������ʽ���������飨������ԣ���

	static const int m_diameter = 32;
	int m_lineNum;     //�����ڵ�����=���ڸ߶�/��ÿһ�ڵĳߴ�������diameter
	int m_columnNum;   //�����ڵ�����=���ڿ��/��ÿһ�ڵĳߴ�������diameter

    // ��ά����洢��Ϸ�����ж�Ӧ��Ԫ��
	// 0Ϊ�ո�-1Ϊ�߿�#��1Ϊ��ͷ@������1������Ϊ����*
	int **m_pCanvas;
	

	int m_foodY;
	int m_foodX;

	//�Դ��޸Ľ���Update�ļ��
	int m_count = 0;
};

class SnakeCreator : public BaseCreator
{
	GameObject* createGameObject() const
	{
		return new Snake();
	}
};

#endif