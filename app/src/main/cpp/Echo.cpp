//
// Created by lbs on 2019/3/8.
//
#include <jni.h>
#include <cstdio>
#include <cstdarg>
#include <cerrno>
#include <cstring>

#include <sys/un.h>
#include <sys/socket.h>

#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <cstddef>
#include <sys/endian.h>
#include "LBS_LOG.h"

#include "com_demo_lbs_myndk_EchoServerActivity.h"

#include "com_demo_lbs_myndk_EchoClientActivity.h"
//最大日志消息长度
#define MAX_LOG_MESSAGE_LENGTH 256
//最大数据缓冲区大小
#define MAX_BUFFER_SIZE 80
/**
 * 将给定的消息记录到应用程序
 */
static void LogMessage(JNIEnv* env,jobject obj,const char* format,...) {
    static jmethodID methodID = nullptr;
    //如果方法ID存在
    if (nullptr == methodID) {
        //从对象获取类
        jclass clazz = env->GetObjectClass(obj);
        //从给定方法中获取方法ID
        methodID = env->GetMethodID(clazz, "logMessage", "(Ljava/lang/String;)V");
        //释放引用
        env->DeleteLocalRef(clazz);
    }
    if (nullptr != methodID) {
        char buffer[MAX_BUFFER_SIZE];
        //定义了一个指针arg_ptr, 用于指示可选的参数.
        va_list ap;
        va_start(ap, format);
        //会自动在写入字符的后面加上停止符\0
        vsnprintf(buffer, MAX_LOG_MESSAGE_LENGTH, format, ap);
        va_end(ap);
        jstring message = env->NewStringUTF(buffer);

        if (nullptr != message) {
            env->CallVoidMethod(obj, methodID, message);

            //释放消息引用
            env->DeleteLocalRef(message);
        }
    }
}
    /**
     * 用给定的异常类和异常消息抛出新的异常
     */
    static void ThrowException(JNIEnv* env,const char* className,const char* message){
        jclass clazz = env->FindClass(className);
        if (nullptr != clazz){
            env->ThrowNew(clazz,message);
            env->DeleteLocalRef(clazz);
        }
    }

    static void ThrowErrnoException(JNIEnv* env,const char* className,int errnum){
        char buffer[MAX_LOG_MESSAGE_LENGTH];
        //获取错误号消息,strerror_r linux 友好的获取错误信息，并且是线程安全的
        if (-1 == strerror_r(errnum,buffer,MAX_LOG_MESSAGE_LENGTH)){
            strerror_r(errno,buffer,MAX_LOG_MESSAGE_LENGTH);
        }
        ThrowException(env,className,buffer);
    }
    /**
     * 构造新的TCP socket
     */
    static int NewTcpSocket(JNIEnv* env,jobject obj){
        LogMessage(env,obj,"Construction a new TCP socket...");
        int tcpSocket = socket(PF_INET,SOCK_STREAM | SOCK_CLOEXEC,0);

        if (-1 == tcpSocket){
            //抛出带错误号的异常
            ThrowErrnoException(env,"java/io/IOException",errno);
        }
        return tcpSocket;
    }
    /**
     * 将 socket 绑定到某一端口号
     */
    static void BindSocketToPort(JNIEnv* env,jobject obj,int sd, unsigned short port){
        struct sockaddr_in address{};
        //绑定 socket 的地址,memset内存赋值函数
        memset(&address,0, sizeof(address));
        address.sin_family = PF_INET;
        //绑定到所有地址
        address.sin_addr.s_addr = htonl(INADDR_ANY);
        //将端口转换为网络字节顺序
        address.sin_port = htonl(port);

        LogMessage(env,obj,"Binding to port %hu.",port);

        if (-1 == bind(sd,(struct sockaddr*)&(address), sizeof(struct sockaddr_in))){
            LOGD("bind error %d",errno);
            char * mesg = strerror(errno);
            LOGD("bind error msg %s",mesg);
            ThrowErrnoException(env,"java/io/IOException",errno);
        }

    }
    /**
     * 获取当前绑定的端口号socket
     */
    static unsigned short GetSocketPort(JNIEnv* env,jobject obj,int sd){
        unsigned short port = 0;
        struct sockaddr_in address;
        socklen_t addressLength = sizeof(address);
        //获取socket地址
        if (-1 == getsockname(sd,(struct sockaddr*) &address,&addressLength)){
            ThrowErrnoException(env,"java/io/IOException",errno);
        }else{
            //将端口转换为主机字节顺序
            port = ntohs(address.sin_port);
            LogMessage(env,obj,"Binded to random port %hu.",port);
        }
        return port;
    }

    /**
     * j监听制定的待处理连接的backing的socket，当backlog已满时拒绝新的连接
     */
    static void ListenOnSocket(JNIEnv* env,jobject obj,int sd,int backing){
        LogMessage(env,obj,"Listening on socket with a backlog of %d pending connections",backing);
        if (-1 == listen(sd,backing)){
            ThrowErrnoException(env,"java/io/IOException",errno);
        }
    }
    /**
     * 记录给定地址的IP地址和端口号
     */
    static void LogAddress(JNIEnv* env,jobject obj, const char* message,const struct sockaddr_in* address){
        char ip[INET_ADDRSTRLEN];
        if (nullptr == inet_ntop(PF_INET,&(address->sin_addr),ip,INET_ADDRSTRLEN)){
            //抛出带错误号的异常
            ThrowErrnoException(env,"java/io/IOException",errno);
        }else{
            //将端口转换为主机字节顺序
            unsigned short port = ntohs(address->sin_port);
            //记录地址
            LogMessage(env,obj,"%s %s:%hu.",message,ip,port);
        }
    }
    /**
     * 在给定的socket上阻塞和等待进来的客户连接
     */
    static int AcceptOnSocket(JNIEnv* env,jobject obj,int sd){
        struct sockaddr_in address{};
        socklen_t addressLength = sizeof(address);

        //阻塞和等待进来的客户连接

        LogMessage(env,obj,"Waiting for a client connection...");

        int clientSocket = accept4(sd, (struct sockaddr *) &address, &addressLength, SOCK_CLOEXEC);
        if (-1 == clientSocket){
            ThrowErrnoException(env,"java/io/IOException",errno);
        }else{
            //记录地址
            LogMessage(env,obj,"Client connection from ",&address);
        }
        return clientSocket;
    }
    /**
     * 阻塞并接收来自 socket 的数据放到缓冲区
     */
    static ssize_t ReceiveFromSocket(JNIEnv* env,jobject obj,int sd,char* buffer,size_t bufferSize){
        LogMessage(env,obj,"Receiving from the socket...");
        size_t recvSize = static_cast<size_t>(recv(sd, buffer, bufferSize - 1, 0));

        if (-1 == recvSize){
            ThrowErrnoException(env,"java/io/IOException",errno);
        }else{
            buffer[recvSize] = NULL;
            if (recvSize > 0){
                LogMessage(env,obj,"Received %d bytes:s%",recvSize,buffer);
            }else{
                LogMessage(env,obj,"Client disconnected.");
            }
        }
        return recvSize;
    }
    /**
     * 将数据缓冲区发送到socket
     */
    static ssize_t SendToSocket(JNIEnv* env,jobject obj,int sd, const char* buffer,size_t bufferSize){
        LogMessage(env,obj,"Sendting to the socket...");
        ssize_t sentSize = send(sd,buffer,bufferSize,0);

        //
        if (-1 == sentSize){
            ThrowErrnoException(env,"java/io/IOException",errno);
        }else{
            if (sentSize > 0){
                LogMessage(env,obj,"Sent %d bytes:%s",sentSize,buffer);
            }else{
                LogMessage(env,obj,"Client disconnected.");
            }
        }
        return sentSize;
    }

    JNIEXPORT void JNICALL Java_com_demo_lbs_myndk_EchoServerActivity_nativeStartTcpServer(JNIEnv* env, jobject obj, jint port){
        //构造新的TCP socket
        int serverSocket = NewTcpSocket(env,obj);
        LOGD("nativeStartTcpServer - 1 %d",serverSocket);
        if (nullptr == env->ExceptionOccurred()){
        LOGD("nativeStartTcpServer - 2");
            //将socket绑定到某端口号
            BindSocketToPort(env,obj,serverSocket,(unsigned short) port);
        LOGD("nativeStartTcpServer - 3");
            if (nullptr != env->ExceptionOccurred()){
        LOGD("nativeStartTcpServer - 4");
                goto exit;
            }
        LOGD("nativeStartTcpServer - 5");
            //如果请求了随机端口号
            if (0 == port){
                //获取当前绑定的端口号socket
        LOGD("nativeStartTcpServer - 6");
                GetSocketPort(env,obj,serverSocket);
        LOGD("nativeStartTcpServer - 7");
                if (nullptr != env->ExceptionOccurred()){
        LOGD("nativeStartTcpServer - 8");
                    goto exit;
                }
            }
            //监听有4个等待连接的backlog的socket
            ListenOnSocket(env,obj,serverSocket,4);
        LOGD("nativeStartTcpServer - 9");
            if (nullptr != env->ExceptionOccurred()){
        LOGD("nativeStartTcpServer - 10");
                goto exit;
            }
            //接受 socket 的一个客户连接
            int clientSocket = AcceptOnSocket(env,obj,serverSocket);
        LOGD("nativeStartTcpServer - 11");
            if (nullptr != env->ExceptionOccurred()){
        LOGD("nativeStartTcpServer - 12");
                goto exit;
            }

            char buffer[MAX_BUFFER_SIZE];
            ssize_t recvSize;
            ssize_t sentSize;
            //接收并发送回数据
            while (1){
        LOGD("nativeStartTcpServer - 13");
        LOGD("nativeStartTcpServer - 14");
                //从socket 中接收
                recvSize = ReceiveFromSocket(env,obj,clientSocket,buffer,MAX_BUFFER_SIZE);
                if ((0 == recvSize) || (nullptr != env->ExceptionOccurred())){
                    break;
                }
                sentSize = SendToSocket(env, obj, clientSocket, buffer, static_cast<size_t>(recvSize));
                if ((0 == sentSize) || (nullptr != env->ExceptionOccurred())){
                    break;
                }

            }
            //关闭客户端socket
        LOGD("nativeStartTcpServer - 15");
            close(clientSocket);
        }
        exit:
        LOGD("nativeStartTcpServer - 16");
            if (serverSocket > 0){
                close(serverSocket);
            }
    }
    /**
     * 连接到给定的IP地址和给定的端口号
     */
    static void ConnectToAddress(JNIEnv* env,jobject obj,int sd,const char* ip, unsigned short port){
        LogMessage(env,obj,"Connecting to %s:%uh...",ip,port);
        struct sockaddr_in address;
        memset(&address,0,sizeof(address));
        address.sin_family = PF_INET;
        //将IP 地址字符串转换为网络地址
        if (0 == inet_aton(ip,&(address.sin_addr))){
            ThrowErrnoException(env,"java/io/IOException",errno);
        }else{
            //将端口号转换为网络字节顺序
            address.sin_port = htons(port);
            //转换为地址
            if (-1 == connect(sd,(const sockaddr*) &address, sizeof(address))){
                //
                ThrowErrnoException(env,"java/io/IOException",errno);
            }else{
                LogMessage(env,obj,"Connected.");
            }
        }
    }

    JNIEXPORT void JNICALL Java_com_demo_lbs_myndk_EchoClientActivity_nativestartTcpClient
            (JNIEnv* env, jobject obj, jstring ip, jint port, jstring message){
        //构造新的TCP socket
        int clientSocket = NewTcpSocket(env,obj);
        if (nullptr == env->ExceptionOccurred()){
            //以c字符串形式获取IP地址
            const char* ipAddress = env->GetStringUTFChars(ip,nullptr);
            if (nullptr == ipAddress){
                goto exit;
            }
            //连接到IP地址和端口
            ConnectToAddress(env,obj,clientSocket,ipAddress,(unsigned short) port);
            //释放IP地址
            env->ReleaseStringUTFChars(ip,ipAddress);

            //如果连接成功
            if (nullptr != env->ExceptionOccurred()){
                goto exit;
            }
            //以C字符串形式获取消息
            const char* messageText = env->GetStringUTFChars(message,nullptr);
            if (nullptr  == messageText){
                goto exit;
            }
            //获取消息大小
            jsize messageSize = env->GetStringUTFLength(message);
            //发送消息给socket
            SendToSocket(env, obj, clientSocket, messageText, static_cast<size_t>(messageSize));
            //释放消息文本
            env->ReleaseStringUTFChars(message,messageText);
            //如果发送未成功
            if (nullptr != env->ExceptionOccurred()){
                goto exit;
            }
            char buffer[MAX_BUFFER_SIZE];
            //从socket接收
            ReceiveFromSocket(env,obj,clientSocket,buffer,MAX_BUFFER_SIZE);
        }
        exit:
            if (clientSocket > -1){
                close(clientSocket);
            }
    }