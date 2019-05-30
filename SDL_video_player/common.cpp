#include "common.h"

//////////////////////////////////////////////
//������ο���https://github.com/samirchen/TestFFmpeg/blob/master/tutorial04/tutorial04.c 
//////////////////////////////////////////////

int audio_buff_size = 0;
int out_buffer_size = 0;
int thread_exit = 0;
int thread_pause = 0;

AVFrame *pFrameYUV;

uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;


//int sfp_refresh_thread(void *opaque)
//{
//	thread_exit = 0;
//	thread_pause = 0;
//
//	while (!thread_exit)
//	{
//		if (!thread_pause)
//		{
//			SDL_Event event;
//			event.type = FF_REFRESH_EVENT;
//			SDL_PushEvent(&event);
//		}
//		SDL_Delay(40); //clw note����Ƶһ�㶼��ÿ��25֡
//	}
//	thread_exit = 0;
//	thread_pause = 0;
//	//Break
//	SDL_Event event;
//	event.type = FF_BREAK_EVENT;
//	SDL_PushEvent(&event);
//
//	return 0;
//}

// �����г�ʼ��
void packet_queue_init(PacketQueue* q)
{
	memset(q, 0, sizeof(PacketQueue));
	q->last_pkt = nullptr;
	q->first_pkt = nullptr;
	q->mutext = SDL_CreateMutex();
	q->cond = SDL_CreateCond();
}

// ����packet�������У�����ͷָ��Ķ���
int packet_queue_put(PacketQueue*q, AVPacket *pkt)
{
	AVPacketList *pktl;
	/*clw note:
	ע�⣬�ڵ���packet_queue_putʱ���ݵ���ָ�룬Ҳ�����β�pkt��ʵ��packet�е�data���õ���ͬһ�����ݻ��档
	������ѭ������av_read_frame��ʱ�򣬻Ὣpacket�е�data�ͷŵ����Ա��ڶ�ȡ��һ��֡���ݡ�
	���Ծ���Ҫ��data���õ����ݻ�����д�����֤�ڶ�ȡ��һ��֡���ݵ�ʱ����data���õ����ݿռ�û�б��ͷš�
	�����ַ���������һ��data���õ����ݻ��棬���߸�data���õĻ���ռ��һ�����ü�����
	������õ��ǵ�һ�ַ�����av_dup_packet
	*/
	if (av_dup_packet(pkt) < 0) //��FFmpeg3�ѷ������ú�����pkt��data���õ����ݻ��渴��һ�ݸ����нڵ��е�AVPacket��
		return -1;

	pktl = (AVPacketList*)av_malloc(sizeof(AVPacketList));
	if (!pktl)
		return -1;

	pktl->pkt = *pkt;
	pktl->next = nullptr;

	SDL_LockMutex(q->mutext);

	if (!q->last_pkt) // ����Ϊ�գ��²���Ԫ��Ϊ��һ��Ԫ��
		q->first_pkt = pktl; //��ô���������ͷָ��ָ���һ��Ԫ��
	else // ������в�Ϊ�գ�������β
		q->last_pkt->next = pktl; //��ǰβָ���Ӧ����ڵ��nextָ�򼴽������packet

	q->last_pkt = pktl; //��������βָ�룬ָ������packet

	q->nb_packets++; //nb_packetsָ���Ƕ�����ӵ�е�packet������++����Ϊ������1��packet

	q->size += pkt->size;  // �����е�size����ָ���ǲ����packet���ܴ�С�������+=
							//clw note��Ҳ����д��pkl->pkt->size

	SDL_CondSignal(q->cond); //ͨ�����ǵ���������Ϊһ�����պ�����������ڵȴ�������һ���ź�
							 //�������������Ѿ��������ˣ����žͻ�������������ö��п������ɷ���
	SDL_UnlockMutex(q->mutext);

	return 0;
}

// �Ӷ�����ȡ��packet
int packet_queue_get(PacketQueue* q, AVPacket* pkt, bool block)
{
	AVPacketList* pktl;
	int ret;

	SDL_LockMutex(q->mutext);

	while (true)
	{
		if (quit)
		{
			ret = -1;
			break;
		}

		pktl = q->first_pkt;  //pktָ�������ͷָ��
		if (pktl)    //���ͷָ��ǿգ�˵������������packet�ģ�����׼��ȡ��packet
		{
			q->first_pkt = pktl->next;  //�ڰѵ�һ��packetȡ����֮ǰ��Ҫ��������ͷָ��ָ����һ���ڵ�
			if (!q->first_pkt)  //�����ǰ����ֻ��1��packet����ô��ʱ����ͷָ��ָ����һ���ڵ����NULL
				q->last_pkt = nullptr;  //��˰�βָ��дΪNULL���������Ϊ�ա������´�packet�����У��ж�
			                            //q->last_pkt�Ƿ�Ϊ�յ�ʱ����������ݣ�����ɼ�����packet_queue_put

			q->nb_packets--;          //ȡ����1��packet��������packet����������
			q->size -= pktl->pkt.size;  //����������packetռ�ܵ�size��СҲ��Ӧ������pktl->pkt.size

			*pkt = pktl->pkt;  //��packet�ĵ�ַ��Ҳ�����ʼʱ���q->first_pkt��Ӧ��packet�Ķ���д��pkt���ָ����ָ����ڴ�ռ�
			av_free(pktl); //�ͷ������ͷ���
			ret = 1;   //����ȡ��packet�ĸ���Ϊ1������return����
			break;
		}
		else if (!block)  //��������Ƿ������ģ�����ͷָ�뻹�ǿյģ���Ϊ��else������ֱ��return 0���������û��ȡ��packet��
		{
			ret = 0;
			break;
		}
		else  //����һ�������ͷָ�뻹�ǿյģ�ȴ��������״����˵����Ȼ����Ŀǰ����Ϊ0���������ڰ�packet������У�ֻ�ǻ�û��ɣ���ô
		{
			SDL_CondWait(q->cond, q->mutext); //��ע�⺯��SDL_CondWait()����ΰ������ǵ�Ҫ����
											 //��������block�ģ�����һֱ�ȵ������������ݣ���
		}
	}

	SDL_UnlockMutex(q->mutext);

	return ret;
}

double get_audio_clock(VideoState *is) 
{
	double pts;
	int hw_buf_size, bytes_per_sec, n;

	pts = is->audio_clock; // Maintained in the audio thread.
	hw_buf_size = is->audio_buf_size - is->audio_buf_index;
	bytes_per_sec = 0;
	n = is->audio_st->codec->channels * 2;
	if (is->audio_st) {
		bytes_per_sec = is->audio_st->codec->sample_rate * n;
	}
	if (bytes_per_sec) {
		pts -= (double)hw_buf_size / bytes_per_sec;
	}
	return pts;
}

int audio_decode_frame(AVCodecContext* aCodecCtx, uint8_t* _audio_buff, int buf_size, VideoState *is, double *pts_ptr)
{
	static AVPacket pkt;
	static uint8_t* audio_pkt_data = nullptr;
	static int audio_pkt_size = 0;
	static AVFrame frame;

	int len1;
	int data_size = 0;
	static int audioCnt = 0;
	double pts;
	int n;
	/************************************************************************/
	/* clw note:
	�°汾��ʹ�������°汾��FFMPEG��⣨2014.5.7����FFMPEG���°汾�е���Ƶ���뷽��
	�����˱Ƚϴ�ı仯��������ɰ����������°��������ʹ�õĻ��������������
	����������һ���󡣾����о����֣��°���avcodec_decode_audio4()����������
	��Ƶ�������ݸ�ʽΪAV_SAMPLE_FMT_FLTP��float, planar����������
	AV_SAMPLE_FMT_S16��signed 16 bits��������޷�ֱ��ʹ��SDL���в��š�
	���Ľ��������ʹ��SwrContext����Ƶ�������ݽ���ת��֮���ٽ���������ţ�
	����Ϳ��Եõ�����ˡ�
	���ߣ�������  https ://blog.csdn.net/leixiaohua1020/article/details/10528443
																			*/
																			/************************************************************************/

	SwrContext* swr_ctx = nullptr;

	while (true)
	{
		while (audio_pkt_size > 0)
		{
			int got_frame = 0;
			/************************************************************************/
			/* clw note:
			��Ҫע����ǣ�һ�����ϵĴ���ֻ��int ret = avcodec_decode_audio4(pCodecCtx_A,
			pFrame_A, &frameFinished, packet);
			Ȼ�����swr_convert����ת���������PCM��ʽ�������ķ�������ADPCMѹ������Ƶ�ļ���
			����ĳЩ�ļ�����Ч����������ֶ��ѴӶ�����ȥ����ٲ��ŵ�Ч����
			���ַ���ʵ���������뻯�ˣ�int nRead=av_read_frame(pFormatCtx, packet)���루demux��
			��������Ƶ֡�п��ܲ���ֹ1֡�����Խ�����������и���ѭ����ͨ��
			int ret = avcodec_decode_audio4(pCodecCtx_A, pFrame_A, &frameFinished, packet);
			����ķ���ֵ�жϽ����˶������ݣ�Ȼ��ı�������ݵ�ָ���������ʣ�µ���Ƶ֡��
			ֱ��������ɣ�
			���ߣ������ǵ���  https://blog.csdn.net/xjb2006/article/details/78980628
																					*/
																					/************************************************************************/
			len1 = avcodec_decode_audio4(aCodecCtx, &frame, &got_frame, &pkt);
			if (len1 < 0) // ��������
			{
				audio_pkt_size = 0;
				break;
			}

			audio_pkt_data += len1;
			audio_pkt_size -= len1;
			data_size = 0;
			if (got_frame)
			{
				data_size = out_buffer_size;
				assert(data_size <= buf_size); //clw note��������size�ĺ��壿
				memcpy(_audio_buff, frame.data[0], data_size); //clw note��frame.linesize[0]��8192��data_size��4096�����ĸ�������û����													 
				printf("audio index:%5d\t pts:%lld\t packet size:%d\n", audioCnt, pkt.pts, pkt.size);
				audioCnt++;
				/************************************************************************/
				/*
					�����������
					http://bbs.csdn.net/topics/390700255?page=1
					ע��һ�㣬ffmpeg�µİ汾����ƵҲ����Ƶ������Ϊpacked��plannar�洢��ʽ��
					�������޸ĺ�����������
																						*/
																						/************************************************************************/
				if (frame.channels > 0 && frame.channel_layout == 0)
					frame.channel_layout = av_get_default_channel_layout(frame.channels);
				else if (frame.channels == 0 && frame.channel_layout > 0)
					frame.channels = av_get_channel_layout_nb_channels(frame.channel_layout);

				AVFrame wanted_frame;
				wanted_frame.format = AV_SAMPLE_FMT_S16;
				wanted_frame.sample_rate = frame.sample_rate;
				wanted_frame.channel_layout = av_get_default_channel_layout(frame.channels);
				wanted_frame.channels = frame.channels;

				if (swr_ctx)
				{
					swr_free(&swr_ctx);
					swr_ctx = nullptr;
				}
				swr_ctx = swr_alloc_set_opts(nullptr, wanted_frame.channel_layout, (AVSampleFormat)wanted_frame.format, wanted_frame.sample_rate,
					frame.channel_layout, (AVSampleFormat)frame.format, frame.sample_rate, 0, nullptr);
				if (!swr_ctx || swr_init(swr_ctx) < 0)
				{
					cout << "swr_init failed:" << endl;
					break;
				}
				int dst_nb_samples = (int)av_rescale_rnd(swr_get_delay(swr_ctx, frame.sample_rate) + frame.nb_samples,
					wanted_frame.sample_rate, wanted_frame.format, AVRounding(1));

				/*clw note��
				ʹ��swr_convertת��������Ҫ��PCM��ʽ��������ڶ�������_audio_buff�С�
				��������PCM��ʽ�ļ�������������´��루�ο����񣩣�
				#if OUTPUT_PCM
					//Write PCM
					fwrite(out_buffer, 1, out_buffer_size, pFile);
				#endif
				��ע������
				out_bufferָ��192000*2�ֽڵ��ڴ�ռ䣻
				1����Ҫд�����ݵĵ��ֽ����������ǰ�1���ֽ�д�룬��Ϊ
					out_buffer=(uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE*2);
					uint8_t��ʵ����unsigned char��ռ��1���ֽڵĿռ䣻
				out_buffer_size��СΪ1024*2*2=4096��Ӧ�þ��ǻص�����ÿ�δ���Stream
					���ֽڴ�С��Ҳ������ν��audio_len
				*/
				int len2 = swr_convert(swr_ctx, &_audio_buff, dst_nb_samples, (const uint8_t**)frame.data, frame.nb_samples);
				if (len2 < 0)
				{
					cout << "swr_convert failed\n";
					break;
				}
				return wanted_frame.channels * len2 * av_get_bytes_per_sample((AVSampleFormat)wanted_frame.format);


				if (data_size <= 0)
					continue; // No data yet,get more frames

				pts = is->audio_clock;
				*pts_ptr = pts;
				n = 2 * is->audio_st->codec->channels;
				is->audio_clock += (double)data_size / (double)(n * is->audio_st->codec->sample_rate);

				return data_size; // we have data,return it and come back for more later
			}
		}
		if (pkt.data)
			av_free_packet(&pkt);

		if (quit)
			return -1;

		if (packet_queue_get(&is->audioq, &pkt, true) < 0)
			return -1;

		audio_pkt_data = pkt.data; //TODO��ԭ����pkt->dataָ�����
		audio_pkt_size = pkt.size;

		// If update, update the audio clock w/pts. ��Ƶʱ��
		if (pkt.pts != AV_NOPTS_VALUE) 
		{
			is->audio_clock = av_q2d(is->audio_st->time_base) * pkt.pts;
		}
	}
}

// �����Ļص�����
void audio_callback(void* userdata, Uint8* stream, int len)
{
	if (len <= 0)  
		return;   //TODO��������ΪӦ�ü���������

	VideoState *is = (VideoState *)userdata;

	//AVCodecContext* aCodecCtx = (AVCodecContext*)userdata;
	int len1, audio_size;
	double pts;

	static unsigned int audio_buf_size = 0;
	static unsigned int audio_buf_index = 0;

	SDL_memset(stream, 0, len);

	while (len > 0)
	{
		if (audio_buf_index >= audio_buf_size)
		{
			audio_size = audio_decode_frame(is->audio_st->codec, audio_buff, audio_buff_size/*sizeof(audio_buff)*/, is, &pts); //clw note������4096����1024*2*2
			if (audio_size < 0)
			{
				audio_buf_size = 1024;
				memset(audio_buff, 0, audio_buf_size);
			}
			else
				audio_buf_size = audio_size;

			audio_buf_index = 0;
		}
		len1 = audio_buf_size - audio_buf_index;
		if (len1 > len)
			len1 = len;

		SDL_MixAudio(stream, audio_buff + audio_buf_index, len, SDL_MIX_MAXVOLUME);


		//memcpy(stream, (uint8_t*)(audio_buff + audio_buf_index), audio_buf_size);
		len -= len1;
		stream += len1;
		audio_buf_index += len1;
	}
}

Uint32 sdl_refresh_timer_cb(Uint32 interval, void *opaque) 
{
	SDL_Event event;
	event.type = FF_REFRESH_EVENT;
	event.user.data1 = opaque;
	SDL_PushEvent(&event);
	return 0; // 0 means stop timer.
}

// Schedule a video refresh in 'delay' ms.
void schedule_refresh(VideoState *is, int delay) 
{
	int ret = SDL_AddTimer(delay, sdl_refresh_timer_cb, is); //�����ʱ���ᴥ��һ���¼����� main �������¼������߼�ȥ�� picture queue ȡ��һ֡��������ʾ����
}

void video_display(VideoState *is) //�������ǵ���Ļ����������ߴ磨�����Լ����õ��� 640x480������������û�Ӧ���ǿ��Ըı�ģ���
                                   //����������Ҫ�ܹ���̬�ؼ�������Ҫ��ʾͼ��ĳߴ硣
{
	VideoPicture *vp;

	vp = &is->pictq[is->pictq_rindex];
	if (vp->bmp)
	{
		//clw note���ɰ����������
		/*
		�������ǵ���Ļ����������ߴ磨�����Լ����õ��� 640x480������������û�Ӧ���ǿ��Ըı�ģ���
		����������Ҫ�ܹ���̬�ؼ�������Ҫ��ʾͼ��ĳߴ硣���ȣ�������Ҫ�������Ƶ�� aspect ratio��
		����Ⱥ͸߶ȵı���(width/height)��������һЩ codec �к���ֵ� sample aspect ration����
		������(������)�Ŀ�߱�(width/height)�����������ǵ� AVCodecContext �еĿ�Ⱥ͸߶���������
		Ϊ��λ����ʾ�ģ���ô��ʱ�� actual aspect ratio Ӧ���� aspect ratio ���� sample aspect ratio��
		�е� codec �� aspect ratio ֵ�� 0�����ʾ����ÿ�����صĳߴ��� 1x1��

		�����������ǷŴ���Ƶ�������������ǵ���Ļ�������е� & -3 λ�������Խ���ֵ��������ӽ� 4 �ı�����
		Ȼ�����ǽ���Ƶ���У������� SDL_DisplayYUVOverlay()������Ҫȷ������ͨ�� screen_mutex ��������
																						        */
		/*if (is->video_st->codec->sample_aspect_ratio.num == 0) 
		{
			aspect_ratio = 0;
		}
		else 
		{
			aspect_ratio = av_q2d(is->video_st->codec->sample_aspect_ratio) * is->video_st->codec->width / is->video_st->codec->height;
		}
		if (aspect_ratio <= 0.0) 
		{
			aspect_ratio = (float)is->video_st->codec->width / (float)is->video_st->codec->height;
		}
		h = screen_h;
		w = ((int)rint(h * aspect_ratio)) & -3;
		if (w > screen_w)
		{
			w = screen_w;
			h = ((int)rint(w / aspect_ratio)) & -3;
		}
		x = (screen_w - w) / 2;
		y = (screen_h - h) / 2;

		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;*/


		//clw modify 20190301��main����������SDL_WINDOWEVENT�¼���SDL_GetWindowSize������
		//                     һ�����ڴ�С�ı������޸Ķ�Ӧ��screen_w��screen_h��Ȼ����������и���
		sdlRect.x = 0;
		sdlRect.y = 0;
		sdlRect.w = screen_w;
		sdlRect.h = screen_h;

		//SDL Begin---------------------------
		SDL_LockMutex(screen_mutex);
		SDL_UpdateTexture(vp->bmp, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]); //clw note��֮ǰ�����������Ǵ�.YUV�ļ��ж���������������һ��buffer;����pFrameYUV->linesize[0]����Ƶ�Ŀ���640*480����Ƶ����ֵ����640��
		SDL_RenderClear(sdlRenderer);
		SDL_RenderCopy(sdlRenderer, vp->bmp, NULL, &sdlRect);
		SDL_RenderPresent(sdlRenderer);
		SDL_UnlockMutex(screen_mutex);
		//SDL End-----------------------
	}
}

void video_refresh_timer(void *userdata)   	//����PTS����ˢ��ʱ����
{
	/*
	���ڼ���������һ�� get_audio_clock �������������� audio clock���ǵ������õ����ֵ��
	������ôȥ��������Ƶ��ͬ��������أ����ֻ�Ǽ򵥵ĳ���������ȷ�� packet �����������
	һ���ܺõķ���������Ҫ�����ǵ�����һ��ˢ�µ�ʱ���������Ƶ���������Ǿͼӿ�ˢ�£����
	��Ƶ���������Ǿͼ���ˢ�¡���Ȼ���ǵ�������ˢ��ʱ�䣬�������� frame_timer �����Ե�ʱ��
	��һ�±Ƚϡ�frame_timer ��һֱ�ۼ��ڲ��Ź��������Ǽ������ʱ��������֮����� 
	frame_timer ���ǲ�����һ֡��Ӧ�ö��ϵ�ʱ��㡣���Ǽ򵥵��� frame_timer ���ۼ��¼���� 
	delay��Ȼ��͵��Ե�ʱ�ӱȽϣ����õõ���ֵ����Ϊʱ����ȥˢ�¡�����߼���Ҫ�ú��Ķ�һ��
	����Ĵ���

	*/


	VideoState *is = (VideoState *)userdata;
	VideoPicture *vp;
	double actual_delay, delay, sync_threshold, ref_clock, diff;

	if (is->video_st) 
	{
		if (is->pictq_size <= 0) //TODO��ԭΪ==0
		{
			schedule_refresh(is, 1);
		}
		else  //�� pictq ����������ʱ��ȡ�� VideoPicture��������ʾ��һ֡ͼ��� timer��
			  //���� video_display() ������Ƶ��ʾ���������Ӷ��еļ����������¶��е� size��
		{
			vp = &is->pictq[is->pictq_rindex];

			delay = vp->pts - is->frame_last_pts; // The pts from last time.
			if (delay <= 0 || delay >= 1.0) {
				// If incorrect delay, use previous one.
				delay = is->frame_last_delay;
			}
			// Save for next time.
			is->frame_last_delay = delay;
			is->frame_last_pts = vp->pts;

			// Update delay to sync to audio.
			ref_clock = get_audio_clock(is);
			diff = vp->pts - ref_clock;

			// Skip or repeat the frame. Take delay into account FFPlay still doesn't "know if this is the best guess."
			sync_threshold = (delay > AV_SYNC_THRESHOLD) ? delay : AV_SYNC_THRESHOLD;
			if (fabs(diff) < AV_NOSYNC_THRESHOLD) {
				if (diff <= -sync_threshold) {
					delay = 0;
				}
				else if (diff >= sync_threshold) {
					delay = 2 * delay;
				}
			}
			is->frame_timer += delay;
			// Computer the REAL delay.
			actual_delay = is->frame_timer - (av_gettime() / 1000000.0);
			if (actual_delay < 0.010) {
				// Really it should skip the picture instead.
				actual_delay = 0.010;
			}
			schedule_refresh(is, (int)(actual_delay * 1000 + 0.5));

			// Now, normally here goes a ton of code about timing, etc. we're just going to guess at a delay for now. You can increase and decrease this value and hard code the timing - but I don't suggest that ;) We'll learn how to do it for real later..
			// ֮ǰΪschedule_refresh(is, 80);

			// Show the picture!
			video_display(is);

			// Update queue for next picture!
			if (++is->pictq_rindex == VIDEO_PICTURE_QUEUE_SIZE) 
			{
				is->pictq_rindex = 0;
			}
			SDL_LockMutex(is->pictq_mutex);
			is->pictq_size--;
			SDL_CondSignal(is->pictq_cond);
			SDL_UnlockMutex(is->pictq_mutex);

			SDL_LockMutex(is->pictq_mutex);
			SDL_CondSignal(is->pictq_cond);
			SDL_UnlockMutex(is->pictq_mutex);

		}
	}
	else 
	{
		schedule_refresh(is, 1);
	}
}

void alloc_picture(void *userdata)
{
	VideoState *is = (VideoState *)userdata;
	VideoPicture *vp;

	vp = &is->pictq[is->pictq_windex];
	if (vp->bmp)
	{
		SDL_DestroyTexture(vp->bmp); //TODO����δȷ���Ƿ���ȷ
	}


	//clw note�������������Ŀ�͸�Ӧ��������֡�Ŀ�͸ߣ�Ҳ������Ļ�Ŀ�͸ߣ�Ҳ������Ƶ�Ŀ�͸�
	//          SDL_CreateTexture��SDL1.2��ΪSDL_CreateYUVOverlay��ͬ����Ӧ��ϵ����SDL_Overlay��������SDL_Texture��
	//          SDL_SetVideoMode()��������SDL_CreateWindow()��SDL_Surface��������SDL_Window�ȣ�
	SDL_LockMutex(screen_mutex);
	vp->bmp = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, is->video_st->codec->width, is->video_st->codec->height);
	SDL_UnlockMutex(screen_mutex);

	vp->width = is->video_st->codec->width;  //TODO��ȷ����Ƶ�ߴ粻�ᷢ���ı䣿
	vp->height = is->video_st->codec->height;

	SDL_LockMutex(is->pictq_mutex);
	vp->allocated = 1;
	SDL_CondSignal(is->pictq_cond);
	SDL_UnlockMutex(is->pictq_mutex);

}



int decode_thread(void *arg) //clw note����������SDL_CreateThread���õģ����ﴫ��VideoState*���Ͷ���is
{
	VideoState *is = (VideoState*)arg;
	uint8_t *out_buffer;
	struct SwsContext *img_convert_ctx;

	// ���ļ�����ȡ����Ϣ
	AVFormatContext* pFormatCtx = nullptr;
	// ��ȡ�ļ�ͷ������ʽ�����Ϣ�����AVFormatContext�ṹ����
	if (avformat_open_input(&pFormatCtx, FILENAME, nullptr, nullptr) != 0)
		return -1; // ��ʧ��

	is->pFormatCtx = pFormatCtx; //clw note������VideoState���Ͷ���

	// ����ļ�������Ϣ
	if (avformat_find_stream_info(pFormatCtx, nullptr) < 0)
		return -1; // û�м�⵽����Ϣ stream infomation

	// �ڿ���̨����ļ���Ϣ
	av_dump_format(pFormatCtx, 0, FILENAME, 0);

	//������Ƶ������Ƶ����Ӧ��Index
	int audioStreamIndex = -1;
	int videoStreamIndex = -1;
	for (int i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audioStreamIndex = i;
		}
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStreamIndex = i;
		}
	}
	if (audioStreamIndex == -1 || videoStreamIndex == -1)
	{
		SDL_Event event;
		event.type = FF_QUIT_EVENT;
		event.user.data1 = is;
		SDL_PushEvent(&event);
		printf("û�в��ҵ���Ƶ������Ƶ��!!!\n");
		return -1; // û�в��ҵ���Ƶ������Ƶ��
	}

	AVCodecContext* pCodecCtx = nullptr;
	AVCodec* pCodec = nullptr;

	//  ������Ƶ��
	// ��1���õ�ָ����Ƶ��������������ĵ�ָ�룬Ȼ��򿪽�����
	pCodecCtx = pFormatCtx->streams[videoStreamIndex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);//�ҳ���������������H.264��MPEG2��
	if (pCodec == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) //�򿪽�����
	{
		printf("Could not open codec.\n");
		return -1;
	}

	pFrameYUV = av_frame_alloc();
	out_buffer = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	///////////////////////////////////////////////////
	//clw note��������Ƶ����Ϣ
	is->videoStreamIndex = videoStreamIndex;
	is->video_st = pFormatCtx->streams[videoStreamIndex];
	
	is->frame_timer = (double)av_gettime() / 1000000.0;
	is->frame_last_delay = 40e-3;

	packet_queue_init(& is->videoq);
	is->video_tid = SDL_CreateThread(video_thread, "video_thread", is);
	is->sws_ctx = img_convert_ctx;
	///////////////////////////////////////////////////


	//  ������Ƶ��
	// ��2���õ�ָ����Ƶ��������������ĵ�ָ��
	pCodecCtx = pFormatCtx->streams[audioStreamIndex]->codec; // codec context
	// �ҵ�audioStreamIndex��decoder
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id); // 3. ���ݶ�ȡ��������Ϣ������Ӧ�Ľ���������
	if (!pCodec)
	{
		cout << "Unsupported codec!" << endl;
		return -1;
	}

	// Open codec
	avcodec_open2(pCodecCtx, pCodec, nullptr);

	/************************************************************************/
	/* ѧϰ����                                                  */
	/************************************************************************/
	//Out Audio Param
	uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
	//AAC:1024  MP3:1152
	int out_nb_samples = pCodecCtx->frame_size; //clw note��һ֡�����Ĳ����������AAC����һ����1024
	AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
	int out_sample_rate = 44100; //clw note�������ʣ������İ���22050,44100��48000Hz
	int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
	//Out Buffer Size����СΪ1024*2*2 = 4096
	out_buffer_size = av_samples_get_buffer_size(nullptr, out_channels, out_nb_samples, out_sample_fmt, 1);
	audio_buff_size = MAX_AUDIO_FRAME_SIZE * 2; //clw note��Ҳ�г�3/2��
	audio_buff = (uint8_t*)av_malloc(audio_buff_size);
	/************************************************************************/

	// Set audio settings from codec info
	SDL_AudioSpec wanted_spec, spec; //clw note��spec��ʱû�õ�
	wanted_spec.freq = out_sample_rate;
	wanted_spec.format = AUDIO_S16SYS;  //clw note��.format ����SDL���ǽ�Ҫ���ĸ�ʽ��
									   //��S16SYS�е�S��ʾ�з��ŵ�signed��
									  //16��ʾÿ��������16λ���ģ�SYS��ʾ���/С�˵�˳��
									  //����ʹ�õ�ϵͳ��ͬ�ġ���Щ��ʽ����avcodec_decode_audio
									  //Ϊ���Ǹ�������������Ƶ�ĸ�ʽ��
	wanted_spec.channels = out_channels;  //˫����ֵΪ2
	wanted_spec.silence = 0;              //0����Ǿ���ģʽ
	wanted_spec.samples = out_nb_samples; //���ǵ�������Ҫ����������ʱ����������SDL�������������������ĳߴ�
										 //һ���ȽϺ��ʵ�ֵ��512��8192֮�䣻һ��ȡ1024��
	wanted_spec.userdata = is;
	wanted_spec.callback = audio_callback; //�����SDL�����ص��������еĲ��������ǽ��ûص������õ�����������������
	//����ص��������𲻶ϵĲ�������,��ô�������������ȡ��������?��ʱ������Ҫ����һ���ڴ������������,
	//�ص��������ϵĶ�ȡ���ݴ���������ڴ���, ����ڴ���Ƕ���PacketQueue.
	//PacketQueueͨ��С����AVPacketList����Ƶ֡AVPacket���һ��˳����У�nb_packetsΪAVPacket������,sizeΪAVPacket.size���ܴ�С

	//�����ĳ����ܹ�����ͬ����Ƶ��ʽ����һ��SDL_AudioSpec��ָ����ΪSDL_OpenAudio() �ĵڶ�����������ȡ��Ӳ����������Ƶ��ʽ������ڶ���������NULL����Ƶ���ݽ�������ʱ��ת����Ӳ����ʽ��
	if (SDL_OpenAudio(&wanted_spec, /*&spec*/ NULL) < 0)
		//clw note�������Ǹ����⣬���д��spec����Ҫ��Ƶ�ʽ�Ϊһ�룿��
	{
		cout << "Open audio failed:" << SDL_GetError() << endl;
		return -1;
	}

	///////////////////////////////////////////////////
	//clw note��������Ƶ����Ϣ  TODO
	is->audioStreamIndex = audioStreamIndex;
	is->audio_st = pFormatCtx->streams[audioStreamIndex];
	//is->audio_buf_size = 0;
	//is->audio_buf_index = 0;
	//memset(&is->audio_pkt, 0, sizeof(is->audio_pkt));
	///////////////////////////////////////////////////


	/************************************************************************/
	/* ѧϰ����                                                  */
	/************************************************************************/
	//FIX:Some Codec's Context Information is missing
	int64_t in_channel_layout = av_get_default_channel_layout(pCodecCtx->channels);
	//Swr
	struct SwrContext *au_convert_ctx;
	au_convert_ctx = swr_alloc();
	au_convert_ctx = swr_alloc_set_opts(au_convert_ctx, out_channel_layout, out_sample_fmt/*out������AV_SAMPLE_FMT_S16*/, out_sample_rate,
		in_channel_layout, pCodecCtx->sample_fmt /*in��������������AV_SAMPLE_FMT_FLTP*/, pCodecCtx->sample_rate, 0, NULL);
	swr_init(au_convert_ctx);
	/************************************************************************/
	packet_queue_init(&is->audioq);
	SDL_PauseAudio(0);  //clw note��������Ƶ�����֣��ᷴ����һ��ʱ���ص�audio_callback


	AVPacket *packet = (AVPacket *)malloc(sizeof(AVPacket));
	av_init_packet(packet);
	int count_read_frame = 0;
	int count_audioStream = 0;
	int count_videoStream = 0;

	//clw modify 20190301 
	while (1)
	{
		if (is->quit) 
		{
			break;
		}

		// Seek stuff goes here.   TODO
		if (is->audioq.size > MAX_AUDIOQ_SIZE || is->videoq.size > MAX_VIDEOQ_SIZE) 
		{
			SDL_Delay(10);
			continue;
		}
		if (av_read_frame(is->pFormatCtx, packet) < 0) //clw note����ȡ1֡ѹ�����ݴ���packet��֮��packet_queue_put
		{
			if (is->pFormatCtx->pb->error == 0) //TODO:???
			{
				SDL_Delay(100);// No error; wait for user input.  //TODO����ӳ�100msûӰ��ô
				continue;
			}
			else
				break; 
		}
			
		//else
		//{
			if (packet->stream_index == audioStreamIndex)
			{
				packet_queue_put(&is->audioq, packet);//�������Ƶ���򽫶���Packet����packet_queue_put���뵽���У�
				count_audioStream++;
			}
			else if (packet->stream_index == videoStreamIndex)
			{
				packet_queue_put(&is->videoq, packet);  //TODO���ĳ�&is->videoq��F5崵�����
				count_videoStream++;
			}
			else
				av_free_packet(packet);//���������Ƶ�������av_free_packet�ͷ��Ѷ�ȡ����AVPacket���ݡ�
			count_read_frame++;
		//}
	}

	swr_free(&au_convert_ctx);//clw note������ָ��������ĵ����÷����޸�һ��ָ���ֵ
							  //Free the given SwrContext and set the pointer to NULL.

	SDL_CloseAudio(); //Close SDL
	SDL_Quit();
	av_free(audio_buff);
	avcodec_close(pCodecCtx);

	// All done - wait for it.
	while (!is->quit) {
		SDL_Delay(100);
	}

	return 0;
}

double synchronize_video(VideoState *is, AVFrame *src_frame, double pts) 
{
	double frame_delay;

	if (pts != 0) 
	{
		// If we have pts, set video clock to it.
		is->video_clock = pts;
	}
	else 
	{
		// If we aren't given a pts, set it to the clock.
		pts = is->video_clock;
	}
	// Update the video clock.
	frame_delay = av_q2d(is->video_st->codec->time_base); //TODO��av_q2d�ĺ��壿
	// If we are repeating a frame, adjust clock accordingly.
	frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
	is->video_clock += frame_delay;
	return pts;
}


int our_get_buffer(struct AVCodecContext *c, AVFrame *pic, int flags) 
{
	int ret = avcodec_default_get_buffer2(c, pic, 0);
	uint64_t *pts = (uint64_t *)av_malloc(sizeof(uint64_t));
	*pts = global_video_pkt_pts;
	pic->opaque = pts;
	return ret;
}

int video_thread(void *arg)
{
	static int videoCnt = 0;
	VideoState *is = (VideoState*)arg;
	AVPacket pkt1;
	AVPacket *packet = &pkt1;
	int frameFinished;
	AVFrame *pFrame;
	double pts;

	pFrame = av_frame_alloc();  //TODO��frame��free������
	
	while (1)
	{
		if (packet_queue_get(&is->videoq, packet, 1) < 0)
		{
			// Means we quit getting packets.
			break;
		}

		pts = 0;

		// Save global pts to be stored in pFrame in first call.
		global_video_pkt_pts = packet->pts;


		//����һ֡ѹ������packet���õ�AVFrame*���͵�pFrame
		int ret = avcodec_decode_video2(is->video_st->codec, pFrame, &frameFinished, packet);
		if (ret < 0)
		{
			printf("Decode Error.\n");
			return -1;
		}
		if (packet->dts == AV_NOPTS_VALUE && pFrame->opaque && *(uint64_t*)pFrame->opaque != AV_NOPTS_VALUE) 
		{
			pts = *(uint64_t *)pFrame->opaque;  //TODO��warning C4244: ��=��: �ӡ�uint64_t��ת������double�������ܶ�ʧ����
		}
		else if (packet->dts != AV_NOPTS_VALUE) 
		{
			pts = packet->dts;  //TODO��warning C4244: ��=��: �ӡ�int64_t��ת������double�������ܶ�ʧ����
		}
		else 
		{
			pts = 0;  //�������޷����� PTS ʱ��������Ϊ 0��
		}
		pts *= av_q2d(is->video_st->time_base);



		if (frameFinished)  //TODO��֮���滻Ϊgot_picture
		{
			pts = synchronize_video(is, pFrame, pts);

			//����ֱ��ȥ����queue_picture����
			VideoPicture *vp;
			// Wait until we have space for a new pic.
			SDL_LockMutex(is->pictq_mutex);
			while (is->pictq_size >= VIDEO_PICTURE_QUEUE_SIZE && !is->quit) 
			{
				SDL_CondWait(is->pictq_cond, is->pictq_mutex);  //clw note���ȵȴ�������֮������alloc����
			}
			SDL_UnlockMutex(is->pictq_mutex);

			if (is->quit)  //TODO���Ƿ��Ҫ��
			{
				break;
			}

			// windex is set to 0 initially.
			vp = &is->pictq[is->pictq_windex];

			// Allocate or resize the buffer!
			if (!vp->bmp)
			{
				SDL_Event event;
				event.type = FF_ALLOC_EVENT;
				event.user.data1 = is;
				SDL_PushEvent(&event);

				// Wait until we have a picture allocated.
				SDL_LockMutex(is->pictq_mutex);
				while (!vp->allocated && !is->quit)
				{
					SDL_CondWait(is->pictq_cond, is->pictq_mutex);
				}
				SDL_UnlockMutex(is->pictq_mutex);

				if (is->quit)  //TODO���Ƿ��Ҫ��
				{
					break;
				}
			}
			else // We have a place to put our picture on the queue.
			{
				//��Ƶ�������ݸ�ʽת��
				sws_scale(is->sws_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, is->video_st->codec->height, pFrameYUV->data, pFrameYUV->linesize);
				vp->pts = pts;
				// ����������Ҫ֪ͨ��Ƶ��ʾ���֣�������ͼƬ���������Ѿ���ͼƬ�ˣ�
				// �����������is->pictq_size++��Ȼ����video_refresh_timer���ж�is->pictq_size>0����ʾ��Ƶ
				if (++is->pictq_windex == VIDEO_PICTURE_QUEUE_SIZE) 
				{
					is->pictq_windex = 0;
				}
				SDL_LockMutex(is->pictq_mutex);
				is->pictq_size++;  //��д������������ is->pictq_size ��ֵ��������������Ҫ��������
				SDL_UnlockMutex(is->pictq_mutex);

				printf("videoCnt index:%5d\t pts:%lld\t packet size:%d\n", videoCnt, packet->pts, packet->size);
				videoCnt++;
			}
		}
		//else
		//{
		//	printf("clw note��frame hasn't finish yet!\n");
		//}
		av_packet_unref(packet); //TODO���Ƿ��Ҫ��
	}
	av_free_packet(packet);
	return 0;
}

