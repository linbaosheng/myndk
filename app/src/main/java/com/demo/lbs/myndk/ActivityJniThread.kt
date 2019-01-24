package com.demo.lbs.myndk

import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.widget.EditText
import kotlinx.android.synthetic.main.activity_jnithread.*
import com.demo.lbs.myndk.jni.PThreadJni
import java.lang.Exception

class ActivityJniThread  : AppCompatActivity() {
    var pThreadJni: PThreadJni? = null;
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_jnithread)
        //初始化原生代码
        pThreadJni = PThreadJni()
        pThreadJni!!.nativeInit(this)
        start_button.setOnClickListener {
            var threads = getNumber(thread_edit,0)
            var iterations = getNumber(iterations_edit,0)
            if (threads > 0 && iterations > 0){
                startThreads(threads,iterations)
            }
        }
    }

    fun getNumber(editText: EditText,defaultValue:Int):Int{
        var value:Int = 0
        try {
            value = Integer.parseInt(editText.text.toString())
        }catch(e:Exception) {
            value = defaultValue
        }
        return value
    }

    fun startThreads(threads:Int,iterations:Int){
//        for (item:Int in 0..(threads-1)){
//            var id = item;
//            Thread(object :Runnable{
//                /**
//                 * When an object implementing interface `Runnable` is used
//                 * to create a thread, starting the thread causes the object's
//                 * `run` method to be called in that separately executing
//                 * thread.
//                 *
//                 *
//                 * The general contract of the method `run` is that it may
//                 * take any action whatsoever.
//                 *
//                 * @see java.lang.Thread.run
//                 */
//                override fun run() {
//                    pThreadJni!!.nativeWorker(this@ActivityJniThread,id,iterations)
//                }
//
//            }).start()
//        }
        pThreadJni!!.posixThreads(this@ActivityJniThread,threads,iterations);
    }

    fun onNativeMessage(message:String){
        runOnUiThread {
            log_view.append(message)
            log_view.append("\n")
        }
    }

    override fun onDestroy() {
        pThreadJni!!.nativeFree()
        super.onDestroy()
    }

//    /**初始化原生代码*/
//    external fun nativeInit():Void
//    /**释放原生资源*/
//    external fun nativeFree():Void
//
//    /**原生Worker*/
//    external fun nativeWorker(id:Int,iterations:Int):Void

//    companion object {
//        init {
//            System.loadLibrary("native-lib")
//        }
//    }
}