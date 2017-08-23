package wh.audioplay;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private final String TAG = "audio_play";

//        private final String inFileName = "000_wh" + File.separator + "test.mp3";
    private final String inFileName = "000_wh" + File.separator + "input.mp4";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void onClickPlay(View view) {
        final String inPath = Environment.getExternalStorageDirectory() + File.separator + inFileName;
        File inFile = new File(inPath);
        if (!inFile.exists()) {
            Log.e(TAG, "音频测试文件不存在");
            return;
        }

        new Thread(new Runnable() {
            @Override
            public void run() {
                AudioPlay.play(inPath);
            }
        }).start();
    }
}
