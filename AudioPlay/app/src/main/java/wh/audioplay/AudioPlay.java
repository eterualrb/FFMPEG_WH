package wh.audioplay;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;

public class AudioPlay {

    static {
        System.loadLibrary("audio-play");
    }

    public native static void play(String inPath);

    public static AudioTrack createAudioTrack(int sampleRateInHz, int nb_channels) {
        Log.i("audio_play", "createAudioTrack >>> sampleRateInHz : " + sampleRateInHz + "\tnb_channels : " + nb_channels);

        int channelConfig = AudioFormat.CHANNEL_OUT_STEREO;
        if (1 == nb_channels) {
            channelConfig = AudioFormat.CHANNEL_OUT_MONO;
        }
        int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
        int bufferSizeInBytes = AudioTrack.getMinBufferSize(sampleRateInHz, channelConfig, audioFormat);

        AudioTrack audioTrack = new AudioTrack(
                AudioManager.STREAM_MUSIC,
                sampleRateInHz,
                channelConfig,
                audioFormat,
                bufferSizeInBytes,
                AudioTrack.MODE_STREAM);

        return audioTrack;
    }

}
