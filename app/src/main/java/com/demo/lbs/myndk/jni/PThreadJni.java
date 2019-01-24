package com.demo.lbs.myndk.jni;

public class PThreadJni {
    static {
        System.loadLibrary("native-lib");
    }
    /**初始化原生代码*/
    public native void nativeInit(Object object);
    /**释放原生资源*/
    public native void nativeFree();

    /**原生Worker*/
    public native void nativeWorker(Object object,int id,int iterations);

    public native void posixThreads(Object object,int threads,int iterations);
}
