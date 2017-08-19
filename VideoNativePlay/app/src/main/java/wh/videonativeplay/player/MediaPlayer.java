package wh.videonativeplay.player;

public class MediaPlayer {

    static {
        System.loadLibrary("video-play");
    }

    public native static void play(String path, Object surface);
}
