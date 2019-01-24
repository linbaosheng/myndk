#include <jni.h>
#include <string>
#include <android/log.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;
const char *LOG_TAG = "LOG_LBS";

extern "C" {
//    #define LOG_TAG "LBSLBS"
    #define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
    JNIEXPORT jstring JNICALL Java_com_demo_lbs_myndk_MainActivity_stringFromJNI(
            JNIEnv *env, jobject obj) {
        string hello = "Hello from C++";
        jstring javaString = env->NewStringUTF("lbs");
        jintArray javaArray;
        javaArray = env->NewIntArray(10);
        if (0 != javaArray) {
            jint nativeArray[10];
            env->GetIntArrayRegion(javaArray, 0, 10, nativeArray);
        }
        LOGD("%s","xsdf");
        /*int* dynamicIntArray = (int*)malloc(sizeof(int) * 16);
        if (NULL == dynamicIntArray){

        }else{
            *dynamicIntArray = 0;
            dynamicIntArray[8] = 8;

            free(dynamicIntArray);
            dynamicIntArray = NULL;
        }
        int* newDynamicIntArray = (int*)realloc(dynamicIntArray, sizeof(int) *32);
        if (NULL == newDynamicIntArray){

        }else{
            dynamicIntArray = newDynamicIntArray;
        }
        int* dynamicInt = new int;
        if (NULL == dynamicInt){

        } else{
            *dynamicInt = 0;
            delete dynamicInt;
            dynamicInt = 0;
        }*/
        /*char data[] = {'h','e','l','l','o','w'};
        size_t count = sizeof(data)/ sizeof(data[0]);
        FILE* stream = fopen("/data/data/om.demo.lbs.myndk/test.txt","a");
        if (count != fwrite(data, sizeof(char),count,stream)){

        }
        if(EOF == fputs("hello\n",stream)){
            
        }
        if (NULL == stream){

        }else{

        }*/
        /*int result;
        result = system("mkdir /data/data/com.demo.lbs.mysdk/temp");
        if (-1 == result || 127 == result){
            LOGD("result is error");
        }else{
            LOGD("result is success");
        }*/
        FILE* stream;
        stream = popen("ls","r");
        if(NULL == stream){
            LOGD("unable to execute the command.");
        }else{
            char buffer[1024];
            int statues;
            while (NULL != fgets(buffer,1024,stream)){
                LOGD("reand:%s",buffer);
            }
            statues = pclose(stream);
            LOGD("process exited whith statues %d",statues);
        }
        uid_t uid;
        uid = getuid();
        
        return env->NewStringUTF(hello.c_str());
    }

    JNIEXPORT jstring JNICALL Java_com_demo_lbs_myndk_MainActivity_copyFileToTarget(
        JNIEnv *env, jobject obj,jstring source,jstring target) {
        char* rtn = NULL;
        jclass classz = env->FindClass("java/lang/String");
        jstring strencode = env->NewStringUTF("GB2312");
        jmethodID mid = env->GetMethodID(classz,"getBytes","(Ljava/lang/String;)[B");
        jbyteArray barr = static_cast<jbyteArray>(env->CallObjectMethod(source, mid, strencode));
        jsize alen = env->GetArrayLength(barr);
        jbyte* ba = env->GetByteArrayElements(barr,JNI_FALSE);
        if (alen > 0){
            rtn = (char*)malloc(alen+1);
            memcpy(rtn,ba,alen);
            rtn[alen] = 0;
        }
        env->ReleaseByteArrayElements(barr,ba,0);
        LOGD("%s",rtn);
        return NULL;
    }

JNIEXPORT jint JNICALL Java_com_demo_lbs_myndk_MainActivity_addOpera(
        JNIEnv *env, jobject obj,jint x,jint y) {
    jint c = x + y;
    return c;
}
}