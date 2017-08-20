#include <jni.h>
#include <android/log.h>

#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"audio_decode",FORMAT,##__VA_ARGS__)
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"audio_decode",FORMAT,##__VA_ARGS__)

#define MAX_AUDIO_FRME_SIZE (44100 * 2)

JNIEXPORT void JNICALL
Java_wh_audiodecode_AudioDecode_decode(JNIEnv *env, jclass type, jstring inPath_,
                                       jstring outPath_) {
    LOGI("audio decode start");

    const char *inPath = (*env)->GetStringUTFChars(env, inPath_, 0);
    const char *outPath = (*env)->GetStringUTFChars(env, outPath_, 0);
    LOGI("in path : %s", inPath);
    LOGI("out path : %s", outPath);

    // 注册FFMPEG组件
    av_register_all();

    AVFormatContext *pAVFormatContext = avformat_alloc_context();
    // 打开输入文件
    if (0 != avformat_open_input(&pAVFormatContext, inPath, NULL, NULL)) {
        LOGE("avformat_open_input error!");
        return;
    }

    // 获取文件流信息
    if (0 > avformat_find_stream_info(pAVFormatContext, NULL)) {
        LOGE("avformat_find_stream_info error!");
        return;
    }

    // 获取音频流index
    int audio_straem_index = -1;
    int i = 0;
    for (; i < pAVFormatContext->nb_streams; ++i) {
        if (AVMEDIA_TYPE_AUDIO == pAVFormatContext->streams[i]->codec->codec_type) {
            audio_straem_index = i;
            break;
        }
    }
    if (-1 == audio_straem_index) {
        LOGE("can't find audio stream!");
        return;
    }

    // 获取编解码器
    AVCodecContext *pAVCodecContext = pAVFormatContext->streams[audio_straem_index]->codec;
    AVCodec *pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);
    if (NULL == pAVCodec) {
        LOGE("avcodec_find_decoder error!");
        return;
    }

    // 打开编解码器
    if (0 != avcodec_open2(pAVCodecContext, pAVCodec, NULL)) {
        LOGE("avcodec_open2 error!");
        return;
    }

    // 编码数据
    AVPacket *pAVPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    // 解码数据
    AVFrame *pAVFrame = av_frame_alloc();

    // 使用avcodec_decode_audio4()解码
    // 得到的音频采样数据格式为AV_SAMPLE_FMT_FLTP（float, planar）
    // 而不是AV_SAMPLE_FMT_S16（signed 16 bits）
    // 需要使用SwrContext对音频采样数据进行转换之后，再进行输出播放
    // http://blog.csdn.net/leixiaohua1020/article/details/10528443

    // 音频重采样Context
    SwrContext *pSwrContext = swr_alloc();

    // 输出的声道布局
    int64_t out_ch_layout = AV_CH_LAYOUT_STEREO; // 立体声
    // 输出采样格式
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16; // 16bit PCM
    // 输出采样率
    int out_sample_rate = 44100;
    // 输入的声道布局
    int64_t in_ch_layout = pAVCodecContext->channel_layout;
    // 输入采样格式
    enum AVSampleFormat in_sample_fmt = pAVCodecContext->sample_fmt;
    // 输入采样率
    int in_sample_rate = pAVCodecContext->sample_rate;

    // 设置param
    swr_alloc_set_opts(pSwrContext,
                       out_ch_layout, out_sample_fmt, out_sample_rate,
                       in_ch_layout, in_sample_fmt, in_sample_rate,
                       0, NULL);

    // 设置完param后需要进行init
    swr_init(pSwrContext);

    FILE *out_file = fopen(outPath, "wb+");

    int got_frame;
    uint8_t *out_buffer = (uint8_t *) av_malloc(MAX_AUDIO_FRME_SIZE);

    // 输出的声道个数
    int out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);

    int index = 0;
    int decode_ret;

    while (0 == av_read_frame(pAVFormatContext, pAVPacket)) {
        if (audio_straem_index != pAVPacket->stream_index) {
            continue;
        }

        decode_ret = avcodec_decode_audio4(pAVCodecContext, pAVFrame, &got_frame, pAVPacket);
        LOGI("audio frame count : %d, \tAVPacket size : %d, \tdecode ret : %d", ++index,
             pAVPacket->size, decode_ret);
        if (0 > decode_ret) {
            // 这里注释掉是因为发现某些文件解码到最后一帧会报错，但是播放pcm没有问题，不知道问题出现在哪里
//            LOGE("avcodec_decode_audio4 error!");
//            return;
        }

        if (0 < got_frame) {
            // 采样数据转化为指定的PCM数据
            swr_convert(pSwrContext,
                        &out_buffer, MAX_AUDIO_FRME_SIZE,
                        pAVFrame->data, pAVFrame->nb_samples);

            int out_buffer_size = av_samples_get_buffer_size(NULL,
                                                             out_channel_nb, pAVFrame->nb_samples,
                                                             out_sample_fmt, 1);
            fwrite(out_buffer, 1, out_buffer_size, out_file);
        }

        av_free_packet(pAVPacket);
    }

    fclose(out_file);

    // 释放资源
    av_free(out_buffer);

    av_frame_free(&pAVFrame);
    av_free(pAVPacket);

    avcodec_close(pAVCodecContext);

    avformat_close_input(&pAVFormatContext);
    avformat_free_context(pAVFormatContext);

    (*env)->ReleaseStringUTFChars(env, inPath_, inPath);
    (*env)->ReleaseStringUTFChars(env, outPath_, outPath);

    LOGI("audio decode end");
}