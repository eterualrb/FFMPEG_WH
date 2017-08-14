package wh.videodecode;

public class VideoDecode {

    static {
        System.loadLibrary("video-decode");
    }

    public native static void decode(String inPath, String outPath);
}
