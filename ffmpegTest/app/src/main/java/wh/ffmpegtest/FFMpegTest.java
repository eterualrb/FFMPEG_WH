package wh.ffmpegtest;

public class FFMpegTest {

    static {
        System.loadLibrary("ffmpeg-test");
    }

    public native static void test();
}
