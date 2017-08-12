#include <jni.h>

#include <android/log.h>
#include "libavformat/avformat.h"

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"ffmpeg_test",FORMAT,##__VA_ARGS__);

JNIEXPORT void JNICALL
Java_wh_ffmpegtest_FFMpegTest_test(JNIEnv *env, jclass type) {

    LOGI("注册FFMPEG组件")
    av_register_all();
}