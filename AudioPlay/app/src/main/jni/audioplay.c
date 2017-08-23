#include <jni.h>
#include <android/log.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"audio_play",FORMAT,##__VA_ARGS__)
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"audio_play",FORMAT,##__VA_ARGS__)

#define MAX_AUDIO_FRAME_SIZE (44100 * 2)

JNIEXPORT void JNICALL
Java_wh_audioplay_AudioPlay_play(JNIEnv *env, jclass type, jstring inPath_) {
    const char *inPath = (*env)->GetStringUTFChars(env, inPath_, 0);

    LOGI("in path : %s", inPath);

    // 注册FFMPEG组件
    av_register_all();

    AVFormatContext *pAVFormatContext = avformat_alloc_context();

    // 打开输入文件
    if (0 != avformat_open_input(&pAVFormatContext, inPath, NULL, NULL)) {
        LOGE("avformat_open_input error!");
        return;
    }

    // 获取流信息
    if (0 > avformat_find_stream_info(pAVFormatContext, NULL)) {
        LOGE("avformat_find_stream_info error!");
        return;
    }

    // 获取audio stream index
    int audio_stream_index = -1;
    int i = 0;
    for (; i < pAVFormatContext->nb_streams; ++i) {
        if (AVMEDIA_TYPE_AUDIO == pAVFormatContext->streams[i]->codec->codec_type) {
            audio_stream_index = i;
            break;
        }
    }
    if (-1 == audio_stream_index) {
        LOGE("can't find audio stream!");
        return;
    }

    // 获取音频流编解码器context
    AVCodecContext *pAVCodecContext = pAVFormatContext->streams[audio_stream_index]->codec;
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

    // 音频采样数据转化设置
    SwrContext *pSwrContext = swr_alloc();

    // 输出声道布局
    int64_t out_ch_layout = AV_CH_LAYOUT_STEREO; //立体声
    // 输出采样格式
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16; // 16bit PCM
    // 输出采样率
    int out_sample_rate = 44100;
    // 输入声道布局
    int64_t in_ch_layout = pAVCodecContext->channel_layout;
    // 输入采样格式
    enum AVSampleFormat in_sample_fmt = pAVCodecContext->sample_fmt;
    // 输入采样率
    int in_sample_rate = pAVCodecContext->sample_rate;

    // 音频采样数据转化参数设置
    swr_alloc_set_opts(pSwrContext,
                       out_ch_layout, out_sample_fmt, out_sample_rate,
                       in_ch_layout, in_sample_fmt, in_sample_rate,
                       0, NULL);

    // swr init
    swr_init(pSwrContext);

    // 输出声道个数
    int out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);

    int got_frame;
    int decode_ret;

    uint8_t *out_buffer = av_malloc(MAX_AUDIO_FRAME_SIZE);

    // 由于java层中的native方式static
    // 所以native层传入的第2个参数为jclass type，指的就是AudioPlay.java这个类
    // 我们要调用Java方法去创建AudioTrack对象
    // 获取createAudioTrack的方法id
    jmethodID jCreateAudioTrackMID = (*env)->GetStaticMethodID(env, type, "createAudioTrack",
                                                               "(II)Landroid/media/AudioTrack;");
    // 调用java层的createAudioTrack方法，获取AudioTrack对象
    jobject jAudioTrack = (*env)->CallStaticObjectMethod(env, type, jCreateAudioTrackMID,
                                                         out_sample_rate, out_channel_nb);

    // 调用AudioTrack.play()方法
    jclass jAudioTrackCls = (*env)->GetObjectClass(env, jAudioTrack);
    jmethodID jPlayMID = (*env)->GetMethodID(env, jAudioTrackCls, "play", "()V");
    (*env)->CallVoidMethod(env, jAudioTrack, jPlayMID);

    // 获取AudioTrack的write(byte[] audioData,int offsetInBytes,int sizeInBytes)方法id
    jmethodID jWriteMID = (*env)->GetMethodID(env, jAudioTrackCls, "write", "([BII)I");

    int frame_count = 0;
    // 解码音频数据并播放
    while (0 == av_read_frame(pAVFormatContext, pAVPacket)) {
        LOGI("frame count : %d", ++frame_count);

        // 如果不是音频数据就继续解码
        if (audio_stream_index != pAVPacket->stream_index) {
            continue;
        }

        decode_ret = avcodec_decode_audio4(pAVCodecContext, pAVFrame, &got_frame, pAVPacket);
        if (0 > decode_ret) {
            // 这里注释掉是因为发现某些文件解码到最后一帧会报错，但是播放pcm没有问题，不知道问题出现在哪里
//            LOGE("avcodec_decode_audio4 error!");
//            return;
        }

        if (0 < got_frame) {
            // 将解码得到的采样数据转化为PCM格式
            swr_convert(pSwrContext,
                        &out_buffer, MAX_AUDIO_FRAME_SIZE,
                        pAVFrame->data, pAVFrame->nb_samples);

            int out_buffer_size = av_samples_get_buffer_size(NULL, out_channel_nb,
                                                             pAVFrame->nb_samples, out_sample_fmt,
                                                             1);

            // 创建jbyteArray数组
            jbyteArray jAudioData = (*env)->NewByteArray(env, out_buffer_size);
            // 获取jbyteArray数组的指针
            jbyte *sample_bytep = (*env)->GetByteArrayElements(env, jAudioData, NULL);
            // 将out_buffer缓冲区中的内容拷贝到jbyteArray中
            memcpy(sample_bytep, out_buffer, out_buffer_size);
            // jni数据同步
            (*env)->ReleaseByteArrayElements(env, jAudioData, sample_bytep, 0);

            // 调用AudioTrack的write方法写入PCM数据
            (*env)->CallIntMethod(env, jAudioTrack, jWriteMID, jAudioData, 0, out_buffer_size);

            // 释放局部引用，不释放的话会溢出，解码过程中会因为溢出报错
            (*env)->DeleteLocalRef(env, jAudioData);
        }

        av_free_packet(pAVPacket);
    }

    // 调用AudioTrack.stop()方法
    jmethodID jStopMID = (*env)->GetMethodID(env, jAudioTrackCls, "stop", "()V");
    (*env)->CallVoidMethod(env, jAudioTrack, jStopMID);

    // 释放资源
    jmethodID JReleaseMID = (*env)->GetMethodID(env, jAudioTrackCls, "release",
                                                "()V"); // 获取AudioTrack的release方法id
    (*env)->CallVoidMethod(env, jAudioTrack, JReleaseMID); // 释放AudioTrack

    av_free(out_buffer);

    swr_free(&pSwrContext);

    av_frame_free(&pAVFrame);
    av_free(pAVPacket);

    avcodec_close(pAVCodecContext);

    avformat_close_input(&pAVFormatContext);
    avformat_free_context(pAVFormatContext);

    (*env)->ReleaseStringUTFChars(env, inPath_, inPath);
}