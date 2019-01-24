package com.demo.lbs.myndk

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        sample_text.text = stringFromJNI()
        btn.setOnClickListener{
//            copyFileToTarget("xx","bb")
            var c = addOpera(3,2);
            Log.i("LOG_LBS","c=>"+c)
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String
    external fun copyFileToTarget(source:String,target:String):Void
    external fun addOpera(x:Int,y:Int):Int
    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}
