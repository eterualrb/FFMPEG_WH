#include <jni.h>
#include <android/log.h>

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"video_decode",FORMAT,##__VA_ARGS__)
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"video_decode",FORMAT,##__VA_ARGS__)

JNIEXPORT void JNICALL
Java_wh_videodecode_VideoDecode_decode(JNIEnv *env, jclass type, jstring inPath_,
                                       jstring outPath_) {
    LOGI("video decode start");

    const char *inPath = (*env)->GetStringUTFChars(env, inPath_, 0);
    const char *outPath = (*env)->GetStringUTFChars(env, outPath_, 0);

    LOGI("in path : %s", inPath);
    LOGI("out path : %s", outPath);

    // 注册所有组件
    av_register_all();

    // 分配一块内存用来保存文件封装格式的相关信息
    AVFormatContext *pAVFormatContext = avformat_alloc_context();

    // 打开文件，读取头部信息。注意此时并没有打开解码器
    if (0 != avformat_open_input(&pAVFormatContext, inPath, NULL, NULL)) {
        LOGE("avformat_open_input exec error!");
        return;
    }

    // 获取文件的流信息
    if (0 > avformat_find_stream_info(pAVFormatContext, NULL)) {
        LOGE("avformat_find_stream_info exec error!");
        return;
    }

    LOGI("filename : %s", pAVFormatContext->filename);
    LOGI("format : %s", pAVFormatContext->iformat->name);
    LOGI("duration : %ld", (pAVFormatContext->duration) / 1000000);
    LOGI("bit_rate : %ld", pAVFormatContext->bit_rate);
    LOGI("nb_streams : %d", pAVFormatContext->nb_streams);

    // 获取视频流在文件流中的索引位置
    int video_stream_index = -1;
    int i = 0;
    for (i = 0; i < pAVFormatContext->nb_streams; i++) {
        if (AVMEDIA_TYPE_VIDEO == pAVFormatContext->streams[i]->codec->codec_type) {
            video_stream_index = i;
            break;
        }
    }
    if (-1 == video_stream_index) {
        LOGE("can't find video stream!!!");
        return;
    }

    // 获取视频流的编解码信息
    AVCodecContext *pAVCodecContext = pAVFormatContext->streams[video_stream_index]->codec;
    LOGI("width : %d", pAVCodecContext->width);
    LOGI("height : %d", pAVCodecContext->height);

    // 根据编解码器ID找到编解码器
    AVCodec *pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);
    if (NULL == pAVCodec) {
        LOGE("can't find video codec!!!");
        return;
    }
    LOGI("codec name : %s", pAVCodec->name);

    // 打开编解码器
    if (0 != avcodec_open2(pAVCodecContext, pAVCodec, NULL)) {
        LOGE("can't open codec");
        return;
    }

    // AVPacket用于存储压缩后的编码数据
    AVPacket *pAVPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    // AVFrame用于存储解压后的像素数据(这里使用YUV420格式)
    AVFrame *pAVFrame = av_frame_alloc();
    // 经过sws_scale转化后的像素数据(原始的像素数据中有脏数据，经过转化后可以去除那些脏数据)
    AVFrame *pScaleAVFrame = av_frame_alloc();

    // 原始数据转化(缓冲区配置)
    // 缓冲区分配内存
    uint8_t *out_buffer = (uint8_t *) av_malloc(
            avpicture_get_size(AV_PIX_FMT_YUV420P, pAVCodecContext->width,
                               pAVCodecContext->height));
    // 初始化缓冲区
    avpicture_fill((AVPicture *) pAVFrame, out_buffer, AV_PIX_FMT_YUV420P, pAVCodecContext->width,
                   pAVCodecContext->height);

    // 配置转码缩放信息，提供给sws_scale()方法使用
    struct SwsContext *pSwsContext = sws_getContext(pAVCodecContext->width, // 原始图像宽度
                                                    pAVCodecContext->height, // 原始图像高度
                                                    pAVCodecContext->pix_fmt, // 原始图像像素格式
                                                    pAVCodecContext->width, // 目标图像宽度
                                                    pAVCodecContext->height, // 目标图像高度
                                                    AV_PIX_FMT_YUV420P, // 目标图像像素格式
                                                    SWS_BICUBIC, // 转码缩放使用的算法和选项
                                                    NULL, NULL, NULL);

    FILE *out_file = fopen(outPath, "wb+");
    int frame_count = 0;

    int ret;
    int got_picture;

    // 循环读取压缩数据进行解压缩
    while (0 == av_read_frame(pAVFormatContext, pAVPacket)) {
//        LOGI("decode frame count : %d", ++frame_count);

        int stream_index = pAVPacket->stream_index;
//        LOGI("packet stream index : %d", stream_index);

        // 如果是视频流
        if (video_stream_index == stream_index) {
            // 解码得到像素数据
            ret = avcodec_decode_video2(pAVCodecContext, pAVFrame, &got_picture, pAVPacket);
            if (0 > ret) {
                LOGE("video decode error!");
                return;
            }

            // 为0解码完成，非0正在解码
            if (got_picture) {
                // 像素数据转化，数据是一行一行转化的
                sws_scale(pSwsContext, pAVFrame->data, pAVFrame->linesize, 0,
                          pAVCodecContext->height,
                          pScaleAVFrame->data, pScaleAVFrame->linesize);

                // 像素数据写入文件
                size_t y_size = (size_t) (pAVCodecContext->width * pAVCodecContext->height);
                fwrite(pScaleAVFrame->data[0], 1, y_size, out_file);
                fwrite(pScaleAVFrame->data[1], 1, y_size / 4, out_file);
                fwrite(pScaleAVFrame->data[2], 1, y_size / 4, out_file);
            }
        }

        //释放资源
        av_free_packet(pAVPacket);
    }

    fflush(out_file);
    fclose(out_file);

    av_frame_free(&pScaleAVFrame);
    av_frame_free(&pAVFrame);

    avcodec_close(pAVCodecContext);

    avformat_free_context(pAVFormatContext);

    (*env)->ReleaseStringUTFChars(env, inPath_, inPath);
    (*env)->ReleaseStringUTFChars(env, outPath_, outPath);

    LOGI("video decode end");
}