#ifndef __MENUBUTTON__
#define __MENUBUTTON__

#include "SDLGameObject.h"
#include "gameobjectfactory.h"

//============================================================
//�����ƣ�    MenuButton
//˵����      �˵���ť��
//���ڣ�      2018-09
//�޸ļ�¼��  �޸���update�ڵİ�ť��������߼�����֤�ڰ�ť��Χ��
//            ������ɿ����Ŵ���mainmenustate-playstate״̬�л�
//============================================================
class MenuButton : public SDLGameObject
{
public:
	MenuButton();

	virtual void load(std::unique_ptr<LoaderParams> const &pParams);  //clw note��std::unique_ptr<LoaderParams>����

	virtual void draw();
	virtual void update();
	virtual void clean();

	void setCallback(void(*callback)()) { m_callback = callback; }
	int getCallbackID() { return m_callbackID; }

	

private:
	enum button_state
	{
		MOUSE_OUT = 0,
		MOUSE_OVER = 1,
		CLICKED = 2
	};

	bool m_bReleased;

	int m_callbackID;

	void (*m_callback)();

	//add by clw 20180918
	bool m_bMouseMoveToButton = false;
};

class MenuButtonCreator : public BaseCreator
{
	GameObject* createGameObject() const
	{
		return new MenuButton();
	}
};

#endif
