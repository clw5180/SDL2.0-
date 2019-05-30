//==============================================================================
/*
	Texture Bank class for loading multiple textures

	3/18/2014
    SDLTutorials.com
    Tim Jones
*/
//==============================================================================
#ifndef __TEXTUREBANK_H__
#define __TEXTUREBANK_H__

#include <map>
#include <string>
#include <vector>
#include <SDL_image.h>
#include <SDL_ttf.h>

//#include "Log.h"
//#include "Game.h"

using namespace std;

//============================================================
//�����ƣ�    TextureManager
//˵����      �����������
//            �ɰ��map���ݳ�ԱΪmap<std::string, Texture*>����;
//���ڣ�      2018-09-14
//�޸ļ�¼��  ����Texture�࣬
//============================================================
class TextureManager 
{
public:
	static TextureManager* Instance()
	{
		if (pInstance == NULL)
		{
			pInstance = new TextureManager();
		}
		return pInstance;
	}

	bool load(string fileName, string id, SDL_Renderer* pRenderer);

	// load image and text    
	bool AddImage(SDL_Renderer* Renderer, std::string ID, std::string Filename);//����ͼƬ��TextureManager��  
	bool AddText(SDL_Renderer* Renderer, std::string ID, std::string Filename);//�����ı���TextureManager��
	bool LoadFromRenderedText(SDL_Renderer* Renderer, std::string ID, std::string textureText, SDL_Color textColor); //addText���Ӻ�����Ҳ���Ե���ʹ�ã�����Game��

	// Free up resources
	//void CleanUp();

	// render
	void draw(string id, int x, int y, int width, int
		height, SDL_Renderer* pRenderer, SDL_RendererFlip flip =
		SDL_FLIP_NONE);
	void drawClip(string id, int x, int y, int diameter, SDL_Renderer* pRenderer, SDL_Rect *clip /*src_clip*/, SDL_RendererFlip flip = SDL_FLIP_NONE);//add by clw��רΪ����4��Բ�ľ������д����
	void drawFrame(string id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer*
		pRenderer, SDL_RendererFlip flip = SDL_FLIP_NONE); 
	void drawFrameEx(string id, int src_width, int src_height,
		int dest_x, int dest_y, int dest_width, int dest_height,
		int currentRow, int currentFrame, SDL_Renderer*
		pRenderer, SDL_RendererFlip flip = SDL_FLIP_NONE);

	void RenderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int diameter, SDL_Rect *clip /*src_clip*/ );  //clw modify 20180819
	
	void clearFromTextureMap(std::string id);

	//=========================================================
	//Get��������
	//=========================================================
	SDL_Texture* GetTextureFromList(std::string ID) 
	{
		if (m_textureMap.find(ID) == m_textureMap.end()) 
			return 0;
		return m_textureMap[ID];
	}

private:
	//=========================================================
	//�ӳ�ʽ����
	//=========================================================
	//void free();  ////GetTextureFromList rid of preexisting texture

	//=========================================================
	//���ݳ�Ա����
	//=========================================================
	std::map<std::string, SDL_Texture*> m_textureMap;

	static TextureManager* pInstance;

	TTF_Font* m_pFont = NULL;
};

typedef TextureManager TheTextureManager;

#endif
