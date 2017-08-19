package wh.videonativeplay;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.View;

import java.io.File;

import wh.videonativeplay.player.MediaPlayer;
import wh.videonativeplay.widget.VideoView;

public class MainActivity extends AppCompatActivity {

    private final String TAG = "native_play";
    private final String inFileName = "000_wh" + File.separator + "input.mp4";

    private VideoView mVideoView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mVideoView = (VideoView) findViewById(R.id.videoView);
    }

    public void onClickPlay(View view) {
        String inPath = Environment.getExternalStorageDirectory() + File.separator + inFileName;
        File inFile = new File(inPath);
        if (!inFile.exists()) {
            Log.e(TAG, "视频测试文件不存在");
            return;
        }

        Surface surface = mVideoView.getHolder().getSurface();
        MediaPlayer.play(inPath, surface);
    }
}
