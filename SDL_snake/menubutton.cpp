#include "menubutton.h"
#include "inputhandler.h"
#include "SoundManager.h"
#include <string>
#include "Game.h"

//MenuButton::MenuButton(const LoaderParams* pParams, void(*callback)()) : SDLGameObject(pParams), m_callback(callback)
MenuButton::MenuButton() : SDLGameObject(), m_callback(0), m_bReleased(true)
{
	
}

void MenuButton::load(std::unique_ptr<LoaderParams> const &pParams)
{
	SDLGameObject::load(std::move(pParams));
	m_callbackID = pParams->getCallbackID();
	m_currentFrame = MOUSE_OUT;  // start at frame 0
}


void MenuButton::draw()
{
	SDLGameObject::draw(); // use the base class drawing
}

void MenuButton::update()
{
	Vector2D* pMousePos = TheInputHandler::Instance()->getMousePosition();

	//�������ƶ����˰�ť�ķ�Χ��
	//if (pMousePos->getX() < (m_position.getX() + m_width) && pMousePos->getX() > m_position.getX()
	//	&& pMousePos->getY() < (m_position.getY() + m_height) && pMousePos->getY() > m_position.getY())                       
	if (pMousePos->getX() < (m_position.getX() + m_width) * Game::GetScaleX()  && pMousePos->getX() > m_position.getX() * Game::GetScaleX()
		&& pMousePos->getY() < (m_position.getY() + m_height) * Game::GetScaleY() && pMousePos->getY() > m_position.getY() * Game::GetScaleY())                       
	{
		//Play the sound effects
		//clw modify 20180918 ���ӵ��playbutton��ť������Ч������û��
		if (!m_bMouseMoveToButton)
		{
			SoundManager::Instance()->playSound("MouseMoveToButton", 0);
			m_bMouseMoveToButton = true;
		}

		if (!TheInputHandler::Instance()->getMouseButtonState(LEFT) )  //��1����������������ɿ���״̬
		{
			if (m_bReleased)                   //��1�����ǰһʱ�����Ҳ���ɿ���״̬
			{
				m_currentFrame = MOUSE_OVER; //˵����û�е����ť�����ֻ�軻һ֡button��ͼƬ��������button�Ѵ���ѡ��״̬(��δ���)
			}
			else                            //��2�����ǰһʱ���ǰ��µ�״̬
			{
				//Play the sound effects
				//clw modify 20180918 ���ӵ��playbutton��ť������Ч������û��
				if (this->GetTextureID() == string("playbutton"))
					SoundManager::Instance()->playSound("MouseClickButton", 0);

				m_currentFrame = CLICKED; //˵�����ڰ�ť�ϵ������Ȼ�������ɿ��ˣ��������Ҫ׼����mainmenustate��playstate�л�

				if (m_callback != 0)    //�������Ҫ��
				{
					m_callback();     //���û��������жϣ���ǡ�ú���ָ��m_callbackΪ�գ��ͻ�崵���
				}

				m_bReleased = true;
			}
			
		}
		else if (TheInputHandler::Instance()->getMouseButtonState(LEFT))  //��2��������������ǰ��µ�״̬
		{
			m_bReleased = false;   //����֮����button��Χ���ɿ����ʱ���ܹ�����ִ��m_callback()��if-else��֧
		}
	}
	else  //���λ���ڰ�ť�ķ�Χ��֮��
	{
		m_bReleased = true;
		m_currentFrame = MOUSE_OUT;
		m_bMouseMoveToButton = false;
	}
}

void MenuButton::clean()
{
	SDLGameObject::clean();
}

