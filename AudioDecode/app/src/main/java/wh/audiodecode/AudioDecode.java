package wh.audiodecode;

public class AudioDecode {

    static {
        System.loadLibrary("audio-decode");
    }

    public native static void decode(String inPath, String outPath);
}
