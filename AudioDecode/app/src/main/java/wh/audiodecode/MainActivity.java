package wh.audiodecode;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private final String TAG = "audio_decode";

//    private final String inFileName = "000_wh" + File.separator + "test.mp3";
//    private final String inFileName = "000_wh" + File.separator + "test2.flac";
    private final String inFileName = "000_wh" + File.separator + "input.mp4";
    private final String outFileName = "000_wh" + File.separator + "test.pcm";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void onClickDecode(View view) {
        String inPath = Environment.getExternalStorageDirectory() + File.separator + inFileName;
        String outPath = Environment.getExternalStorageDirectory() + File.separator + outFileName;

        File inFile = new File(inPath);
        if (!inFile.exists()) {
            Log.e(TAG, "音频测试文件不存在");
            return;
        }

        AudioDecode.decode(inPath, outPath);
    }

}
