#include <jni.h>
#include <string>
#include <android/log.h>
#include <stdlib.h>
#include <unistd.h>
#include "com_demo_lbs_myndk_jni_PThreadJni.h"
#include "LBS_LOG.h"
struct NativeWorkerArgs{
    jint id;
    jint iterations;
};

static jmethodID gOnNativeMessage = NULL;

static JavaVM* gVm = NULL;

static jobject gObj = NULL;
//互斥实例
static pthread_mutex_t mutex;

jint JNI_OnLoad(JavaVM* vm,void* reserved){
    LOGD("JNI_OnLoad");
    gVm = vm;
    return JNI_VERSION_1_6;
}
static void* nativeWorkerThread(void* args){
    JNIEnv* env = NULL;
    if (0 == gVm->AttachCurrentThread(&env,NULL)){
        NativeWorkerArgs* nativeWorkerArgs = static_cast<NativeWorkerArgs *>(args);
        LOGD("id %d thread start",nativeWorkerArgs->id);
        Java_com_demo_lbs_myndk_jni_PThreadJni_nativeWorker(env,gObj,gObj,nativeWorkerArgs->id,nativeWorkerArgs->iterations);
        delete nativeWorkerArgs;
        //从JAVA虚拟机中分离当前线程
        gVm->DetachCurrentThread();
    }
    return reinterpret_cast<void *>(1);

}
JNIEXPORT void JNICALL Java_com_demo_lbs_myndk_jni_PThreadJni_nativeInit(
            JNIEnv *env, jobject obj,jobject jobject1){
        LOGD("ni hao a");
        //初始化互斥
        if (0 != pthread_mutex_init(&mutex,NULL)){
            jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
            env->ThrowNew(exceptionClazz,"Unable to find metex");
            goto exit;
        }
        if (gObj == NULL){
            gObj = env->NewGlobalRef(jobject1);
            if (gObj == NULL){
                goto exit;
            }
        }
        if (NULL == gOnNativeMessage){
            jclass jclazz = env->GetObjectClass(jobject1);
            gOnNativeMessage = env->GetMethodID(jclazz,"onNativeMessage","(Ljava/lang/String;)V");
            if (NULL == gOnNativeMessage){
                jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
                env->ThrowNew(exceptionClazz,"Unable to find method");
                goto exit;
            }
        }
        exit:
            return;
    }

    JNIEXPORT void JNICALL Java_com_demo_lbs_myndk_jni_PThreadJni_nativeFree(
            JNIEnv *env, jobject obj){
        if (gObj != NULL){
            env->DeleteGlobalRef(gObj);
            gObj = NULL;
        }
        if (0 != pthread_mutex_destroy(&mutex)){
            jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
            env->ThrowNew(exceptionClazz,"Unable to destory mutex");
        }
    }

    JNIEXPORT void JNICALL Java_com_demo_lbs_myndk_jni_PThreadJni_nativeWorker(
            JNIEnv *env, jobject obj,jobject jobject1,jint id,jint iterations){
        if (0 != pthread_mutex_lock(&mutex)){
            jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
            env->ThrowNew(exceptionClazz,"Unable to lock mutex");
            goto exit;
        }
        for (jint i = 0; i < iterations; i++){
            char message[26];
            sprintf(message,"Worker %d: Iteration %d",id,i);
            LOGD("start..id->%d,ite->%d",id,iterations);
            jstring messageString = env->NewStringUTF(message);

            env->CallVoidMethod(jobject1,gOnNativeMessage,messageString);

            if (NULL != env->ExceptionOccurred()){
                break;
            }
            sleep(1);
            LOGD("end...id->%d,ite->%d",id,iterations);
        }
        if (0 != pthread_mutex_unlock(&mutex)){
            jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
            env->ThrowNew(exceptionClazz,"Unable to unlock mutex");
            goto exit;
        }
        exit:
            return;
    }
    JNIEXPORT void JNICALL Java_com_demo_lbs_myndk_jni_PThreadJni_posixThreads
            (JNIEnv *env, jobject obj, jobject jobject1, jint threads, jint iterations){
        pthread_t* handles = new pthread_t[threads];
        for (jint i = 0; i < threads; i++){
            NativeWorkerArgs* nativeWorkerArgs = new NativeWorkerArgs();
            nativeWorkerArgs->id = i;
            nativeWorkerArgs->iterations = iterations;

            pthread_t thread;
            int result = pthread_create(/*&thread,*/&handles[i],NULL,nativeWorkerThread,(void*)nativeWorkerArgs);
            if (0 != result){
                jclass exceptionClass = env->FindClass("java/lang/RuntimeException");
                env->ThrowNew(exceptionClass,"Unable to create threads");
                goto exit;
            }
        }
        for(jint i = 0;i < threads; i++){
            void* result = NULL;
            if (0 != pthread_join(handles[i],&result)){
                jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
                env->ThrowNew(exceptionClazz,"Unable to join thread");
            }else{
                char message[26];
                sprintf(message, "Worker %d returned %ld", i, reinterpret_cast<long>(result));

                jstring messageString = env->NewStringUTF(message);

                env->CallVoidMethod(gObj,gOnNativeMessage,messageString);

                if (NULL != env->ExceptionOccurred()){
                    goto exit;
                }
            }
        }
        exit:
            return;
    }
