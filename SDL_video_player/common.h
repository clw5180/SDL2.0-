#pragma once

#include <iostream>
#include <stdio.h>
#include <assert.h>

#include <SDL.h>
#include <SDL_thread.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/time.h> //for av_gettime()
}

using namespace std;

#define MAX_AUDIO_FRAME_SIZE 192000 /*176400*/
#define SDL_AUDIO_BUFFER_SIZE 1024
#define FILENAME "1.mp4"

#define MAX_AUDIOQ_SIZE (5 * 16 * 1024)  //TODO����ô���ģ�
#define MAX_VIDEOQ_SIZE (5 * 256 * 1024)

#define AV_SYNC_THRESHOLD 0.01
#define AV_NOSYNC_THRESHOLD 10.0

//Refresh Event
#define FF_ALLOC_EVENT  (SDL_USEREVENT + 1)
#define FF_REFRESH_EVENT  (SDL_USEREVENT + 2)
#define FF_QUIT_EVENT (SDL_USEREVENT + 3)

#define VIDEO_PICTURE_QUEUE_SIZE 1

typedef struct PacketQueue
{
	AVPacketList *first_pkt; // ��ͷ
	AVPacketList *last_pkt; // ��β

	int nb_packets; //���ĸ���
	int size; // ռ�ÿռ���ֽ���
	SDL_mutex* mutext; // �����ź���
	SDL_cond* cond; // ��������
}PacketQueue;

typedef struct VideoPicture //clw note������ÿһ֡ͼƬ�������ߡ����Ƿ��Ѿ��������Ϣ
{
	SDL_Texture *bmp;  //clw note��VideoState ���и������������洢 VideoPicture��
	                    //����������Ҫ�Լ������ͷ��� SDL_Overlay ���ڴ棬
	                    //ע�⣬�����allocated �����������������û��������¡�
	int width, height; // Source height & width.
	int allocated;
	double pts;
} VideoPicture;

//������������Ƶ��Ϣ
typedef struct VideoState
{
	AVFormatContext *pFormatCtx; //��ʽ��Ϣ

	/* video */
	int videoStreamIndex; //��Ƶ���������
	double frame_timer;
	double frame_last_pts;
	double frame_last_delay;
	double video_clock; // pts of last decoded frame / predicted pts of next decoded frame.
	AVStream *video_st; //��Ӧ����Ƶ������
	PacketQueue videoq; //��Ƶ���ݶ���
	VideoPicture pictq[VIDEO_PICTURE_QUEUE_SIZE];  //��Ƶ���ݻ��������洢��������Ƶ֡
	int pictq_size, pictq_rindex, pictq_windex; //TODO�����庬�岻��

	/* audio */
	int audioStreamIndex; // ��Ƶ���������
	AVStream* audio_st; //��Ӧ����Ƶ������
	PacketQueue audioq; //��Ƶ���ݶ���
	uint8_t audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
	unsigned int audio_buf_size;
	unsigned int audio_buf_index;
	AVFrame audio_frame;
	AVPacket audio_pkt;
	uint8_t *audio_pkt_data;
	int audio_pkt_size;
	double audio_clock;

	///* sdl */
	SDL_mutex *pictq_mutex; //��Ƶ��Ⱦ������ݻ����� pictq ����
	                        //���ţ���ʼ����Ƶ��Ⱦ������ݻ����� pictq ��������Ϊ�¼�ѭ���������ǵ�
	                        //��Ⱦ����ʱ����Ⱦ�߼��ͻ�� pictq ��ȡ���ݣ�ͬʱ�����߼��ֻ��� pictq 
	                        //д�����ݣ����ǲ�֪��˭���ȵ�������������Ҫͨ������������ֹ�̴߳���
	SDL_cond *pictq_cond;

	SDL_Thread *parse_tid;
	SDL_Thread *video_tid;
	SDL_Thread *video_refresh_tid;

	char fileName[128];
	int quit; //�˳���־

	AVIOContext *io_context;
	struct SwsContext *sws_ctx; //clw note:SwsContext(software scale)��Ҫ������Ƶͼ���ת���������ʽת��.
								//        SwrContext(software resample)��Ҫ������Ƶ�ز��������������ת��������ת����
}VideoState;

extern int quit;
extern uint8_t *audio_buff;
extern int screen_w;
extern int screen_h;
extern SDL_Window *screen;
extern SDL_Rect sdlRect;
extern SDL_Renderer *sdlRenderer;
extern SDL_mutex *screen_mutex;
//extern VideoState *global_video_state;

int sfp_refresh_thread(void *opaque);
// �����г�ʼ��
void packet_queue_init(PacketQueue* q);
// ����packet�������У�����ͷָ��Ķ���
int packet_queue_put(PacketQueue*q, AVPacket *pkt);
// �Ӷ�����ȡ��packet
/*static*/ int packet_queue_get(PacketQueue* q, AVPacket* pkt, bool block);

double get_audio_clock(VideoState *is);
// ��packet����Ϊframe
int audio_decode_frame(AVCodecContext* aCodecCtx, uint8_t* _audio_buff, int buf_size, VideoState *is, double *pts_ptr); //TODO��ֻҪһ��is�����Ϳ����ˣ�ʣ�¶�ɾ����ע����Ƶ��ʼ���ĵط���buf_sizeд�˾Ϳ���
// ��Ƶ����ص���������һ��ʱ��ͻ���ã�ȡ��������Ƶ���ݣ�����У�û���򷵻أ�����Ƶ�豸
void audio_callback(void* userdata, Uint8* stream, int len);
// ��ָ���ĺ�����֮�����еĻص�����
Uint32 sdl_refresh_timer_cb(Uint32 interval, void *opaque); //TODO cb�ĳ�callback
//ϵͳ��ָ������ʱ����ûص�����sdl_refresh_timer_cb��������һ��FF_REFRESH_EVENT�¼���
//�Ӷ������������Խ����¼���������ˢ����Ƶ����video_refresh_timer
void schedule_refresh(VideoState *is, int delay);
// ���Ӻ�������ʾ��Ƶ
void video_display(VideoState *is);
// ˢ����Ƶ
void video_refresh_timer(void *userdata); 
// ����ͼƬ
void alloc_picture(void *userdata);
// ���װ�̣߳�packet������
int decode_thread(void *arg);

double synchronize_video(VideoState *is, AVFrame *src_frame, double pts);

// These are called whenever we allocate a frame buffer. We use this to store the global_pts in a frame at the time it is allocated.
int our_get_buffer(struct AVCodecContext *c, AVFrame *pic, int flags);
// ��Ƶ�����߳�
int video_thread(void *arg);


