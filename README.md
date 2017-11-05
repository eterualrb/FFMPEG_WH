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

