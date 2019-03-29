#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <SDL.h>

//˵����ͨ��Ӧ�þ�����������Ŀ��ʹ��ȫ�ֱ�����
//����ֻ����ʱ��һЩ����д��ȫ�ֱ�������ʽ����֤Դ���뾡���ܼ򵥡�
 
extern const int g_moveVelocity;

// ���ڿ��
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

// ��ͼ���
extern const int MAP_WIDTH;
extern const int MAP_HEIGHT;

extern SDL_Window *g_pWindow;
extern SDL_Renderer *g_pRenderer;
extern SDL_Texture *g_pMapTexture;  
extern SDL_Texture *g_pSpriteSheetTexture;


#endif   /* defined(__GLOBAL_H__)*/