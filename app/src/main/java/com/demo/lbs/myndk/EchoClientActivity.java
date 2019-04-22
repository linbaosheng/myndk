package com.demo.lbs.myndk;

import android.os.Bundle;
import android.text.TextUtils;
import android.widget.EditText;
import org.jetbrains.annotations.Nullable;

public class EchoClientActivity extends AbstractEchoActivity{
    /**IP地址*/
    private EditText ipEdit;
    /**消息编辑*/
    private EditText messageEdit;

    public EchoClientActivity() {
        super(R.layout.activity_echo_client);
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ipEdit = findViewById(R.id.ip_edit);
        messageEdit = findViewById(R.id.message_edit);
    }

    @Override
    public void onStartButtonClicked() {
        String ip = ipEdit.getText().toString();
        Integer port = getPort();
        String message = messageEdit.getText().toString();

        if (!TextUtils.isEmpty(ip) && !TextUtils.isEmpty(message)){
            ClientTask clientTask = new ClientTask(ip,port,message);
            clientTask.start();
        }
    }

    /**
     * 根据给定服务器IP地址和端口号启动TCP客户端，并且发送给定消息
     * @param ip
     * @param port
     * @param message
     * @throws Exception
     */
    private native void nativestartTcpClient(String ip,int port,String message) throws Exception;

    private class ClientTask extends AbstractEchoTask{
        private final String ip;

        private final int port;

        private final String message;
        public ClientTask(String ip,int port,String message){
            this.ip = ip;
            this.port = port;
            this.message = message;
        }
        @Override
        protected void onBackground() {
            logMessage("Dtarting client.");

            try {
                nativestartTcpClient(ip,port,message);
            } catch (Exception e) {
                e.printStackTrace();
            }

            logMessage("Client terminated.");
        }
    }
}
