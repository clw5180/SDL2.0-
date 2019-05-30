#include "Snake.h"
#include "Game.h"
#include "TextureManager.h"


Snake::Snake() : SDLGameObject()
{
	m_lineNum = TheGame::Instance()->GetWindowHeight() / m_diameter;
	m_columnNum = TheGame::Instance()->GetWindowWidth() / m_diameter;
	//������Ļ��Ⱥ�snake�ĳߴ磨ÿһ�ڵ�ֱ�����������ά���飬���ڴ洢snake��λ�õ���Ϣ
	m_pCanvas = new int *[m_lineNum];
	for (int i = 0; i < m_lineNum; ++i)
		m_pCanvas[i] = new int[m_columnNum]();
}

Snake::~Snake()
{
	if (m_pCanvas != NULL)
	{
		delete m_pCanvas;
		m_pCanvas = NULL;
	}
}

//=========================================================
//�������ƣ�load
//˵����
//�޸ļ�¼��
//=========================================================
void Snake::load(std::unique_ptr<LoaderParams> const &pParams)
{
	//SDLGameObject::load(std::move(pParams));

	m_iW = 100, m_iH = 100;
	for (int i = 0; i < 2; ++i)
	{
		//ѡȡ./res/image.png�ĵ�һ�е�����sprite������ɫ�ͻ�ɫ��Բ
		clips[i].x = m_iW;
		clips[i].y = i % 2 * m_iH;
		clips[i].w = m_iW;
		clips[i].h = m_iH;
	}

	/*
	clw note 20180820��
	������£�����1024*768��Բ���������δ�СΪ32*32��һ�п��Է�32��Բ��һ�п��Է�24��Բ
	��ʼ���߿�
	����ע��һ������Ҫ�����⣺�������һάi����y�᷽��
	���� ���� ���� ���� ���� ���� �� x��
	|
	|
	|
	��
	y��
	*/
	for (int i = 0; i < m_lineNum; i++)   //ע�����������ʼ��
	{
		m_pCanvas[i][0] = -1;
		m_pCanvas[i][m_columnNum - 1] = -1;
	}

	for (int j = 0; j < m_columnNum; j++)  //ע����������ʼ��
	{
		m_pCanvas[0][j] = -1;
		m_pCanvas[m_lineNum - 1][j] = -1;
	}

	// ��ʼ����ͷλ��
	m_pCanvas[m_lineNum / 2][m_columnNum / 2] = 1;

	// ��ʼ������������Ԫ��ֵ�ֱ�Ϊ2,3,4,5....
	for (int i = 1; i <= 4; i++)
	{
		m_pCanvas[m_lineNum / 2][m_columnNum / 2 - i] = i + 1;
	}

	m_moveDirection = MOVERIGHT;

	RandomGenerateFood();

	//clw note��ע��m_foodX�Ƚ�С�������canvasǰ��
	m_pCanvas[m_foodY / m_diameter][m_foodX / m_diameter] = -2;
}

//=========================================================
//�������ƣ�handleInput
//˵����
//�޸ļ�¼��
//=========================================================
void Snake::handleInput()
{
	// handle keys
	if ( !(TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) ||
		   TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) ||
		   TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) ||
		   TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT)) 
	     || !TheInputHandler::Instance()->isKeyRepeat() )
	{
		m_velocity = 4;
	}

	if(TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) 
		/*&& m_position.getY() > 0*/)  //����UP��
	{
		//m_velocity.setY(-m_moveSpeed);

		if (TheInputHandler::Instance()->isKeyRepeat() && m_moveDirection != MOVEDOWN)
		{
			m_velocity *= 4;
			if (m_velocity >= m_velocityMax)
				m_velocity = m_velocityMax;
		}
		/*clw note 20180914 �߼��������ʱ�ǳ�ĳһ�����ߣ���Ӧ�ñ�֤���෴�ķ������Ч����Ϊײ���Լ�����Ϊ����Update��Ҳ������ʧ�ܡ���
		�����õ������if�жϣ������ֻ�������жϻ�����һ�����⣺
		1������snake���������˶�����Ȼ��ʱ�����ϣ�����m_moveDirectionΪMOVEDOWN�������������if�Ĵ��ڲ����m_moveDirectionд��MOVEUP��Ҳ�Ͳ�����֮��ײ���Լ������Ǻ���ģ�
		2�����ǣ������ʱ�����ң���Ѹ�ٰ��ϣ�����snake��û�����ü������˶�����m_moveDirection�Ѿ����MOVERIGHT�˶�������MOVEDOWN�����m_moveDirection��д��MOVEUP���Ӷ�����֮��Updateʱ��snake�����˶�ײ���Լ���������Ϸʧ�ܣ����ǲ�����ġ���ˣ�����Ҫ��Update�����ٽ���������ײ���, ������Լ����Լ�ײֱ��returnʲô���������ɣ�������ö��߳��ܷ�����һ���⣿��

		����ע�⣬�����if�ж�Ҳ�ǲ���ɾ�ģ���ΪUpdate����if (m_count > 60 / m_velocity)�Ĵ��ڣ��൱��ÿ��1/m_velocity��Ż����һ�Σ�
		�������ɾ��if�жϣ���ô��û����Update�����뼸��HandleEvents��ʱ�򣬾ͻ���ְ������Ҳ��ʹsnake���ٵ������
		*/
		if (m_moveDirection != MOVEDOWN)
			m_moveDirection = MOVEUP;
		return;  //����ͬʱ�����Ϻ���������λ����ֵ��m_keystates������Ϻ��ң���Ϊif�жϵ�˳�����������ҵ�˳����˻����ȴ���������¼�Ȼ��return��
	}

	else if(TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN))  
	{
		
		if (TheInputHandler::Instance()->isKeyRepeat() && m_moveDirection != MOVEUP)
		{
			m_velocity *= 4;
			if (m_velocity >= m_velocityMax)
				m_velocity = m_velocityMax;
		}
		if (m_moveDirection != MOVEUP)
			m_moveDirection = MOVEDOWN;
		return;

	}
		
	else if(TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT))
	{
		if (TheInputHandler::Instance()->isKeyRepeat() && m_moveDirection != MOVERIGHT)
		{
			m_velocity *= 4;
			if (m_velocity >= m_velocityMax)
				m_velocity = m_velocityMax;
		}
		if (m_moveDirection != MOVERIGHT)
			m_moveDirection = MOVELEFT;
		return;
	}

	else if(TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT))
	{
		if (TheInputHandler::Instance()->isKeyRepeat() && m_moveDirection != MOVELEFT)
		{
			m_velocity *= 4;
			if (m_velocity >= m_velocityMax)
				m_velocity = m_velocityMax;
		}
		if (m_moveDirection != MOVELEFT)
			m_moveDirection = MOVERIGHT;
		return;
	}
}

//=========================================================
//�������ƣ�update
//˵����
//�޸ļ�¼��
//=========================================================
void Snake::update()
{
	handleInput();

	//SDLGameObject::update();  //clw note������һ��ͨ�õ�update����

	if (m_count > 60 / m_velocity)
	{
		moveSnakeByDirection();
	}
	else
	{
		m_count++;
	}

}

//=========================================================
//�������ƣ�draw
//˵����
//�޸ļ�¼��
//=========================================================
void Snake::draw()
{
	
	//clw note 20180820��SDL_RenderClear���������SDL_SetRenderDrawColor�������õ���ɫ���renderer��
	//		   ע�⣬RGBֵ��R=0,G=0,B=0���൱��û�й⣬��Ļ��������Ȼ�Ǻ�ɫ��
	//		   ��̫����Լ���ڰ�ɫ�����Դ�̫����������߲ʣ����R=255,G=255,B=255��ӦΪ��ɫ��

	//draw snake and food
	for (int i = 0; i < m_lineNum; i++)
	{
		for (int j = 0; j < m_columnNum; j++)
		{
			//clw note����res�����и�����Ϊsnake��ͼ���������ĸ�Բ
			//          �û�ɫ��Բ��ʾfood������ɫ��Բ��ʾsnake
			if (m_pCanvas[i][j] == -2)
			{
				//render food
				//TextureManager::Instance()->GetTextureFromList("snake")->RenderTexture(TextureManager::Instance()->GetTextureFromList("snake")->GetSDLTexture(), Renderer, j * m_diameter, i * m_diameter, &clips[1]);
				TextureManager::Instance()->drawClip("snake", j*m_diameter, i*m_diameter, m_diameter, TheGame::Instance()->GetRenderer(), &clips[1]);
			}

			else if (m_pCanvas[i][j] != 0 && m_pCanvas[i][j] != -1 && m_pCanvas[i][j] != -2)
			{
				//render snake
				//TextureManager::Instance()->GetTextureFromList("snake")->RenderTexture(TextureManager::Instance()->GetTextureFromList("snake")->GetSDLTexture(), Renderer, j * m_diameter, i * m_diameter, &clips[0]);
				TextureManager::Instance()->drawClip("snake", j*m_diameter, i*m_diameter, m_diameter, TheGame::Instance()->GetRenderer(), &clips[0]);
			}
		}
	}

	//render score
	//clw note 20180824������Score��λ�ã�����Ҫ��֤����룬������ò�Ҫ��������
	SDL_Texture* tempTex = TextureManager::Instance()->GetTextureFromList("score");
	SDL_Rect dst;
	//Get dimensions of text Texture
	SDL_QueryTexture(tempTex, NULL, NULL, &dst.w, &dst.h);
	//TextureManager::Instance()->RenderTexture(tempTex, TheGame::Instance()->GetRenderer(), int(TheGame::Instance()->GetWindowWidth() * 0.7), dst.h * 2, m_diameter, NULL);
	TextureManager::Instance()->RenderTexture(tempTex, TheGame::Instance()->GetRenderer(), int(TheGame::Instance()->GetWindowWidth() - 200), dst.h * 2, m_diameter, NULL);
}

//=========================================================
//�������ƣ�clean
//˵����
//�޸ļ�¼��
//=========================================================
void Snake::clean()
{
	SDLGameObject::clean();
}

//==============����Ϊ�Ӻ���==============//
void Snake::RandomGenerateFood()
{
	int tmpXMax = (TheGame::Instance()->GetWindowWidth() - 2 * m_diameter) / m_diameter;
	int tmpYMax = (TheGame::Instance()->GetWindowHeight() - 2 * m_diameter) / m_diameter;
	m_foodX = (rand() % tmpXMax + 1) * m_diameter;
	m_foodY = (rand() % tmpYMax + 1) * m_diameter;
}

void Snake::moveSnakeByDirection()
{
	int i, j;

	int oldTail_i, oldTail_j, oldHead_i, oldHead_j;
	int max = 0;

	for (i = 1; i < m_lineNum - 1; i++)
	{
		for (j = 1; j < m_columnNum - 1; j++)
		{
			if (m_pCanvas[i][j] > 0)
			{
				//�ҵ��ƶ�֮ǰ��βλ�ã�����֮ǰ��5 4 3 2 1
				//����5��λ��
				if (max < m_pCanvas[i][j])
				{
					max = m_pCanvas[i][j];
					oldTail_i = i;
					oldTail_j = j;
				}
				if (m_pCanvas[i][j] == 1)
				{
					//�ҵ��ƶ�ǰ��ͷλ�ã�����֮ǰ��5 4 3 2 1
					//����1��λ��
					oldHead_i = i;
					oldHead_j = j;
				}
			}
		}
	}

	//canvas[oldTail_i][oldTail_j] = 0;  //û��ʳ��֮ǰ��
	int newHead_i, newHead_j;

	// clw note 20180824��������һ������moveSnakeByDirection()
	switch (m_moveDirection)
	{
	case MOVEUP:  // �����ƶ�
				  //clw modify 20180823
				  //�����snake���෴�����˶������������ƶ����º��Լ�ײ���ˣ������������ײ��������Ϸʧ��
				  //ֱ�Ӱѷ�����Ϊ�෴���򣬼����£�Ȼ��return
		if (m_pCanvas[oldHead_i - 1][oldHead_j] == 2)
		{
			m_moveDirection = MOVEDOWN;
			return;
		}

		//canvas[oldHead_i - 1][oldHead_j] = 1;  //û��ʳ��֮ǰ��
		newHead_i = oldHead_i - 1;
		newHead_j = oldHead_j;
		break;
	case MOVEDOWN:
		if (m_pCanvas[oldHead_i + 1][oldHead_j] == 2)
		{
			m_moveDirection = MOVEUP;
			return;
		}

		//canvas[oldHead_i + 1][oldHead_j] = 1;  //û��ʳ��֮ǰ��
		newHead_i = oldHead_i + 1;
		newHead_j = oldHead_j;
		break;
	case MOVELEFT:
		if (m_pCanvas[oldHead_i][oldHead_j - 1] == 2)
		{
			m_moveDirection = MOVERIGHT;
			return;
		}

		//canvas[oldHead_i][oldHead_j - 1] = 1;  //û��ʳ��֮ǰ��
		newHead_i = oldHead_i;
		newHead_j = oldHead_j - 1;
		break;
	case MOVERIGHT:
		if (m_pCanvas[oldHead_i][oldHead_j + 1] == 2)
		{
			m_moveDirection = MOVELEFT;
			return;
		}

		//canvas[oldHead_i][oldHead_j + 1] = 1;  //û��ʳ��֮ǰ��
		newHead_i = oldHead_i;
		newHead_j = oldHead_j + 1;
		break;
	}

	for (i = 1; i < m_lineNum - 1; i++)
	{
		for (j = 1; j < m_columnNum - 1; j++)
		{
			if (m_pCanvas[i][j] > 0)
				m_pCanvas[i][j]++;
			//�����������ƶ������Ȱ�snake�����в�λ+1����ʾ�ƶ�
			//�ٸ��ݷ���ȷ��snake head��λ��
			//���磬��ʼ��ʱ����5 4 3 2 1 ���߽綼��-1�������ط���0��
			//���ڱ����6 5 4 3 2��
		}
	}

	// ����ͷ����Ե�ʳ��, ԭ���ľ���β���ţ������Զ�+1
	if (m_pCanvas[newHead_i][newHead_j] == -2)
	{
		m_pCanvas[m_foodY / m_diameter][m_foodX / m_diameter] = 0;
		// ����һ���µ�ʳ��
		//m_foodY = rand() * m_diameter % (m_screenWidth - 5 * m_diameter) + 2 * m_diameter;
		//m_foodX = rand() * m_diameter % (m_screenHeight - 5 * m_diameter) + 2 * m_diameter;

		// clw modify 20180918������m_diameter ~ WindowWidth - m_diameter�������m_foodX��
		//                        ��m_diameter ~ WindowHeight- m_diameter�������m_foodY����ֹ�ڱ߽��߽�����ĵط�����ʳ�� 
		RandomGenerateFood();


		m_pCanvas[m_foodY / m_diameter][m_foodX / m_diameter] = -2;

		//clw modify 20180824 ����Ե�ʳ�����+10
		//Render text
		int tmpScore = TheGame::Instance()->GetScore();
		tmpScore += 10;
		TheGame::Instance()->SetScore(tmpScore);
		string strScore = "score : " + to_string(tmpScore);
		SDL_Color textColor = { 255, 255, 255 };
		if (TextureManager::Instance()->LoadFromRenderedText(TheGame::Instance()->GetRenderer(), "score", strScore, textColor) == false)
		{
			Log("Unable to LoadFromRenderedText Texture: %s", "score");
		}
	}
	else // ����ԭ���ľ���β���������ֳ��Ȳ���
		m_pCanvas[oldTail_i][oldTail_j] = 0;

	// �Ƿ�С�ߺ�����ײ�����ߺͱ߿�ײ����Ϸʧ��
	if (m_pCanvas[newHead_i][newHead_j] > 0 || m_pCanvas[newHead_i][newHead_j] == -1)
	{
		//clw modify 20180917��ʹ����Ϸ״̬��������Ϸ����״̬ת������Ϸ����״̬
		// Q:�Ƿ�Ӧ�ü���PlayState.cpp���棬Ȼ������ֻ���޸�һ���������統ǰ����ֵΪ0���� 
		TheGame::Instance()->SetGameOverFlag(true);

		//system("pause");
		//exit(0);  
	}
	else
		m_pCanvas[newHead_i][newHead_j] = 1;

	m_count = 0;
}