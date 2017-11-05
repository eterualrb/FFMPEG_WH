# FFMPEG_WH

## 1. fmpegTest - [FFMPEG Android Studio开发环境搭建](https://eterualrb.github.io/2017/03/12/FFMPEG-Android-Studio%E5%BC%80%E5%8F%91%E7%8E%AF%E5%A2%83%E6%90%AD%E5%BB%BA/)

* 使用cmake方式编译so，配合LLDB可以进行native层代码调试
* 支持java代码和c/c++代码相互跳转
* java native方法对应的jni方法自动生成

## 2. VideoDecode - [FFMPEG 视频解码](https://eterualrb.github.io/2017/08/15/FFMPEG-%E8%A7%86%E9%A2%91%E8%A7%A3%E7%A0%81/)

* 使用FFMPEG进行视频解码并将yuv数据写入到文件
* 测试画面，使用YUV Player播放yuv文件

![](./img/01.gif)

## 3. VideoNativePlay - [FFMPEG ANativeWindow播放视频](https://eterualrb.github.io/2017/08/17/FFMPEG-ANativeWindow%E6%92%AD%E6%94%BE%E8%A7%86%E9%A2%91/)

* 使用ANativeWindow进行native层播放处理
* 使用开源库libyuv进行原始像素数据格式转换
* 测试画面

![](./img/02.gif)

## 4. AudioDecode - [FFMPEG 音频解码](https://eterualrb.github.io/2017/08/19/FFMPEG-%E9%9F%B3%E9%A2%91%E8%A7%A3%E7%A0%81/)

* 使用FFMPEG进行音频解码并将pcm数据写入到文件
* 测试画面，使用audition播放pcm文件

![](./img/03.gif)

## 5. AudioPlay - [FFMPEG 使用AudioTrack播放PCM数据](https://eterualrb.github.io/2017/08/21/FFMPEG-%E4%BD%BF%E7%94%A8AudioTrack%E6%92%AD%E6%94%BEPCM%E6%95%B0%E6%8D%AE/)

* 使用Android API的AudioTrack来播放解码后的pcm数据
* 具体测试结果请运行demo

## 6. easyPlayer - [FFmpeg 音视频同步](http://www.jianshu.com/p/27279255f67e)

* 这部分内容是参考别人的blog[FFmpeg 音视频同步](http://www.jianshu.com/p/27279255f67e)完成的
* 音视频解码多线程处理，使用生产者消费者模式来处理同步问题
* OpenSL ES进行音频数据播放
* 具体测试结果请运行demo

## 7. 其它音视频内容总结

* openSL-ES-study

  * android平台下使用OpenSL ES播放PCM数据


  * blog地址：[Android OpenSL ES播放PCM数据](https://eterualrb.github.io/2017/03/26/Android-OpenSL-ES%E6%92%AD%E6%94%BEwav%E6%96%87%E4%BB%B6/)
  * code地址：https://github.com/eterualrb/openSL-ES-study

* WHLive

  * android native层进行音视频推流


  * blog地址：[Android直播 native层音视频推流](https://eterualrb.github.io/2017/07/02/Android%E7%9B%B4%E6%92%AD-native%E5%B1%82%E9%9F%B3%E8%A7%86%E9%A2%91%E6%8E%A8%E6%B5%81/)
  * code地址：https://github.com/eterualrb/LIVE_WH

* Simple_H264_Analyzer

  * H.264视频码流解析
  * code地址：https://github.com/eterualrb/Simple_H264_Analyzer

