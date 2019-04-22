package com.demo.lbs.myndk;

public class EchoServerActivity extends AbstractEchoActivity {

    public EchoServerActivity() {
        super(R.layout.activity_echo_server);
    }

    @Override
    public void onStartButtonClicked() {
        Integer port = getPort();
        if (port != null){
            ServerTask serverTask = new ServerTask(port);
            serverTask.start();
        }
    }

    /**
     * 根据给定端口启动TCP服务器
     * @param port 端口号
     * @throws Exception
     */
    private native void nativeStartTcpServer(int port) throws Exception;

    /**
     * 根据给定端口UDP服务
     * @param port 端口号
     * @throws Exception
     */
    private native void nativeStartUdpServer(int port) throws Exception;

    private class ServerTask extends AbstractEchoTask{

        private final int port;

        public ServerTask(int port){
            this.port = port;
        }

        @Override
        protected void onBackground() {
            logMessage("Start server.");
            try {
                nativeStartTcpServer(port);
            } catch (Exception e) {
                e.printStackTrace();
            }
            logMessage("Server terminated");
        }
    }
}
