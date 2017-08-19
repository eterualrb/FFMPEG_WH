#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <stdlib.h>
#include <unistd.h>

#include "libavformat/avformat.h"
#include "libyuv.h"

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"native_play",FORMAT,##__VA_ARGS__)
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"native_play",FORMAT,##__VA_ARGS__)

JNIEXPORT void JNICALL
Java_wh_videonativeplay_player_MediaPlayer_play(JNIEnv *env, jclass type, jstring path_,
                                                jobject surface) {
    const char *path = (*env)->GetStringUTFChars(env, path_, 0);
    LOGI("path : %s", path);

    // 注册所有组件
    av_register_all();

    AVFormatContext *pAVFormatContext = avformat_alloc_context();

    // 打开输入文件
    if (0 != avformat_open_input(&pAVFormatContext, path, NULL, NULL)) {
        LOGE("avformat_open_input error!");
        return;
    }

    // 获取文件流信息
    if (0 > avformat_find_stream_info(pAVFormatContext, NULL)) {
        LOGE("avformat_find_stream_info error!");
        return;
    }

    // 视频流index
    int video_stream_index = -1;
    int i = 0;
    for (; i < pAVFormatContext->nb_streams; ++i) {
        if (AVMEDIA_TYPE_VIDEO == pAVFormatContext->streams[i]->codec->codec_type) {
            video_stream_index = i;
            break;
        }
    }
    if (-1 == video_stream_index) {
        LOGE("can't find video stream!");
        return;
    }

    AVCodecContext *pAVCodecContext = pAVFormatContext->streams[video_stream_index]->codec;
    // 获取编解码器
    AVCodec *pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);
    if (NULL == pAVCodec) {
        LOGE("avcodec_find_decoder error!å");
        return;
    }

    // 打开解码器
    if (0 != avcodec_open2(pAVCodecContext, pAVCodec, NULL)) {
        LOGE("avcodec_open2 error!");
        return;
    }

    // 编码数据
    AVPacket *pAVPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    // 解码数据(像素数据)
    AVFrame *pYUVFrame = av_frame_alloc();
    AVFrame *pARGBFrame = av_frame_alloc();

    // 获取native window
    ANativeWindow *pANativeWindow = ANativeWindow_fromSurface(env, surface);
    // 绘制时的缓冲区
    ANativeWindow_Buffer window_buffer;
    // 设置native window的缓冲大小
    ANativeWindow_setBuffersGeometry(
            pANativeWindow,
            pAVCodecContext->width,
            pAVCodecContext->height,
            WINDOW_FORMAT_RGBA_8888);

    int got_picture = -1;

    // 解码
    while (0 == av_read_frame(pAVFormatContext, pAVPacket)) {
        // 如果不是视频流，就继续下一帧数据的读取
        if (video_stream_index != pAVPacket->stream_index) {
            continue;
        }

        // 如果是视频流，就进行视频解码
        if (0 > avcodec_decode_video2(pAVCodecContext, pYUVFrame, &got_picture, pAVPacket)) {
            LOGE("avcodec_decode_video2 error!");
            return;
        }

        if (0 < got_picture) {
            ANativeWindow_lock(pANativeWindow, &window_buffer, NULL);

            // 将pARGBFrame使用到的buffer直接指定为window_buffer的bits
            // 这里的写法很巧妙
            avpicture_fill(
                    (AVPicture *) pARGBFrame,
                    window_buffer.bits,
                    PIX_FMT_ARGB,
                    pAVCodecContext->width,
                    pAVCodecContext->height);

            // 使用libyuv将yuv420p转化为argb
            I420ToARGB(pYUVFrame->data[0], pYUVFrame->linesize[0],
                       pYUVFrame->data[2], pYUVFrame->linesize[2],
                       pYUVFrame->data[1], pYUVFrame->linesize[1],
                       pARGBFrame->data[0], pARGBFrame->linesize[0],
                       pAVCodecContext->width, pAVCodecContext->height);

            ANativeWindow_unlockAndPost(pANativeWindow);

            usleep(16 * 1000);
        }

        // 清空AVPacket中的数据
        av_free_packet(pAVPacket);
    }

    // 释放资源
    ANativeWindow_release(pANativeWindow);

    av_frame_free(&pARGBFrame);
    av_frame_free(&pYUVFrame);
    av_free(pAVPacket);

    avcodec_close(pAVCodecContext);
    avformat_close_input(&pAVFormatContext);
    avformat_free_context(pAVFormatContext);

    (*env)->ReleaseStringUTFChars(env, path_, path);
}