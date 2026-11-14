package com.smith.ffhack;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Switch;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.graphics.Color;
import android.content.Intent;
import android.net.Uri;
import android.provider.Settings;

public class MainActivity extends Activity {
    static { System.loadLibrary("native-lib"); }
    public native void initHack();
    public native void setAimbot(boolean enable);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!Settings.canDrawOverlays(this)) {
            Intent intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION, Uri.parse("package:" + getPackageName()));
            startActivityForResult(intent, 123);
        }
        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setBackgroundColor(Color.parseColor("#1E1E1E"));
        layout.setPadding(60, 60, 60, 60);

        TextView title = new TextView(this);
        title.setText("FF Hack Panel");
        title.setTextColor(Color.GREEN);
        title.setTextSize(28f);
        layout.addView(title);

        Switch aimSwitch = new Switch(this);
        aimSwitch.setText("Enable Headshot Hack");
        aimSwitch.setTextColor(Color.WHITE);
        aimSwitch.setTextSize(18f);
        aimSwitch.setOnCheckedChangeListener((buttonView, isChecked) -> setAimbot(isChecked));
        layout.addView(aimSwitch);
        setContentView(layout);
        initHack();
    }
}
