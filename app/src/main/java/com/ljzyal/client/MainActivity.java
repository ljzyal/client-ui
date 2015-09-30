package com.ljzyal.client;

import android.app.Activity;
import android.app.NotificationManager;
import android.app.Notification;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.AsyncTask;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import org.apache.http.util.EncodingUtils;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;

public class MainActivity extends Activity {

    private UserLoginTask mAuthTask = null;

    private EditText mUserView;
    private EditText mPasswordView;
    private TextView mLoginTextView;
    private View mLoginFormView;

    private String usercfg = "";
    private String passcfg = "";
    private boolean readcfg(){
        String text="";
        try{
            FileInputStream fin = openFileInput("config.cfg");
            int length = fin.available();
            if(length == 0)
                return false;
            byte [] buffer = new byte[length];
            fin.read(buffer);
            text = EncodingUtils.getString(buffer, "UTF-8");
            int i = 0;
            for (;i<text.length();i++)
                if(text.charAt(i)=='\n')
                    break;
                else usercfg += text.charAt(i);
            i++;
            for (;i<text.length();i++)
                passcfg+=text.charAt(i);
            fin.close();
        }
        catch(Exception e){
            e.printStackTrace();
        }
        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mUserView = (EditText) findViewById(R.id.user);

        mPasswordView = (EditText) findViewById(R.id.password);
        mPasswordView.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView textView, int id, KeyEvent keyEvent) {
                if (id == R.id.login || id == EditorInfo.IME_NULL) {
                    attemptLogin();
                    return true;
                }
                return false;
            }
        });

        Button mLoginButton = (Button) findViewById(R.id.login_button);
        mLoginButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                attemptLogin();
            }
        });

        mLoginFormView = findViewById(R.id.login_form);
        mLoginTextView = (TextView) findViewById(R.id.login_text);

        if(readcfg()){
            mUserView.setText(usercfg);
            mPasswordView.setText(passcfg);
        }
    }

    public void attemptLogin() {
        if (mAuthTask != null) {
            return;
        }

        mUserView.setError(null);
        mPasswordView.setError(null);

        String user = mUserView.getText().toString();
        String password = mPasswordView.getText().toString();

        boolean cancel = false;
        View focusView = null;

        if (TextUtils.isEmpty(password)) {
            mPasswordView.setError(getString(R.string.error_invalid_password));
            focusView = mPasswordView;
            cancel = true;
        }

        if (TextUtils.isEmpty(user)) {
            mUserView.setError(getString(R.string.error_field_required));
            focusView = mUserView;
            cancel = true;
        }

        if (cancel) {
            focusView.requestFocus();
        } else {
            showProgress(true);
            mAuthTask = new UserLoginTask(user, password);
            mAuthTask.execute((Void) null);
        }
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event){
        if(KeyEvent.KEYCODE_BACK==keyCode){
            Intent home = new Intent(Intent.ACTION_MAIN);
            home.addCategory(Intent.CATEGORY_HOME);
            startActivity(home);
            return false;
        }
        return super.onKeyDown(keyCode, event);
    }

    public int not_id = 0;
    public void mynotify(String message){
        NotificationManager notificationManager = (NotificationManager)
                this.getSystemService(NOTIFICATION_SERVICE);

        Notification notification =new Notification(R.mipmap.ic_launcher,
                message, System.currentTimeMillis());

        notification.flags |= Notification.FLAG_ONLY_ALERT_ONCE;
        notification.flags |= Notification.FLAG_AUTO_CANCEL;
        notification.flags |= Notification.FLAG_SHOW_LIGHTS;

        notification.defaults = Notification.DEFAULT_LIGHTS;

        CharSequence contentTitle ="客户端";
        CharSequence contentText = message;
        Intent notificationIntent =new Intent(MainActivity.this, MainActivity.class);
        PendingIntent contentItent = PendingIntent.getActivity(this, not_id, notificationIntent, 0);
        notification.setLatestEventInfo(this, contentTitle, contentText, contentItent);

        notificationManager.notify(not_id++, notification);
    }

    public void update_text(String buf){
        String xx = mLoginTextView.getText().toString();
        xx = xx + buf + "\n";
        mLoginTextView.setText(xx);
    }

    private static final int GetMessage = 100;
    String buf_main="";
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (msg.what == GetMessage) {
                update_text(buf_main);
            }
        }
    };

    public void showProgress(final boolean show) {
        mLoginTextView.setVisibility(show ? View.VISIBLE : View.GONE);
        mLoginFormView.setVisibility(show ? View.GONE : View.VISIBLE);
        mLoginTextView.setText("");
    }

    public class UserLoginTask extends AsyncTask<Void, Void, Boolean> {

        private final String mUser;
        private final String mPassword;

        UserLoginTask(String user, String password) {
            mUser = user;
            mPassword = password;
        }

        public String getMacID()
        {
            String str ="";

            WifiManager wifi = (WifiManager) getSystemService(Context.WIFI_SERVICE);
            WifiInfo info = wifi.getConnectionInfo();
            str = info.getMacAddress();

            return str;
        }

        private void savecfg() {
            try{
                FileOutputStream fout =openFileOutput("config.cfg", MODE_PRIVATE);
                fout.write(mUser.getBytes());
                fout.write("\n".getBytes());
                fout.write(mPassword.getBytes());
                fout.write("\n".getBytes());
                fout.write(getMacID().getBytes());
                fout.close();
            }
            catch(Exception e){
                e.printStackTrace();
            }
        }

        public void copy_my_assets(String file) {
            String filename = "/data/data/com.ljzyal.client/files/"+file;
            Log.d("client_DEBUG","copy start");
            File dir = new File("/data/data/com.ljzyal.client/files");
            try {
                InputStream is = getResources().getAssets().open("armeabi-v7a/"+file);
                FileOutputStream fos = new FileOutputStream(filename);
                byte[] buffer = new byte[10000];
                int count = 0;
                while ((count = is.read(buffer)) > 0) {
                    fos.write(buffer, 0, count);
                }
                fos.close();
                is.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
            Log.d("client_DEBUG","copy end");
        }

        public void login_android() {
            Log.d("client_DEBUG", "login start");

            Process process = null;
            DataOutputStream os = null;
            BufferedReader in = null;
            try {
                Log.d("client_DEBUG","su start");
                process = Runtime.getRuntime().exec("su");
                os = new DataOutputStream(process.getOutputStream());
                in = new BufferedReader(new InputStreamReader(process.getInputStream()));
                os.writeBytes("export LD_LIBRARY_PATH=/data/data/com.ljzyal.client/files" + "\n");
                os.writeBytes("chmod 0777 /data/data/com.ljzyal.client/files/login" + "\n");
                os.writeBytes("/data/data/com.ljzyal.client/files/login" + "\n");
                os.writeBytes("exit\n");
                os.flush();
                String buf = null;
                while(true){
                    if((buf = in.readLine())==null){
                        break;
                    }
                    buf = new String(buf.getBytes("ISO8859-1"),"GB2312");
                    mynotify(buf);
                    buf_main = buf;
                    Message msg = new Message();
                    msg.what = GetMessage;
                    handler.sendMessage(msg);
                }
                process.waitFor();
            } catch (Exception e) {
                Log.d("client_DEBUG", "login_root_error" + e.getMessage());
                return;
            } finally {
                try {
                    if (os != null)
                        os.close();
                    if (in != null)
                        in.close();
                    process.destroy();
                } catch (Exception e) {
                }
            }
            Log.d("client_DEBUG", "login_root success");
        }

        @Override
        protected Boolean doInBackground(Void... params) {
            savecfg();
            copy_my_assets("login");
            copy_my_assets("libclient_core.so");
            login_android();
            return true;
        }

        @Override
        protected void onPostExecute(final Boolean success) {
            mAuthTask = null;
            showProgress(false);

            if (success) {
                finish();
            } else {
                mPasswordView.setError(getString(R.string.error_incorrect_password));
                mPasswordView.requestFocus();
            }
        }

        @Override
        protected void onCancelled() {
            mAuthTask = null;
            showProgress(false);
        }
    }
}

