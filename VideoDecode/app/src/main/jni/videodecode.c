#include <jni.h>
#include <android/log.h>

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"video_decode",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"video_decode",FORMAT,##__VA_ARGS__);

JNIEXPORT void JNICALL
Java_wh_videodecode_VideoDecode_decode(JNIEnv *env, jclass type, jstring inPath_,
                                       jstring outPath_) {
    const char *inPath = (*env)->GetStringUTFChars(env, inPath_, 0);
    const char *outPath = (*env)->GetStringUTFChars(env, outPath_, 0);

    LOGI("in path : %s", inPath)
    LOGE("out path : %s", outPath)

    (*env)->ReleaseStringUTFChars(env, inPath_, inPath);
    (*env)->ReleaseStringUTFChars(env, outPath_, outPath);
}