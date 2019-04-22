package com.demo.lbs.myndk

import android.os.Bundle
import android.os.Handler
import android.support.v7.app.AppCompatActivity
import android.view.View
import android.widget.Button
import android.widget.EditText
import android.widget.ScrollView
import android.widget.TextView

abstract class AbstractEchoActivity(private val `layotID`:Int) : AppCompatActivity(),View.OnClickListener{

    protected var portEdit:EditText? = null

    protected var startButton: Button? = null

    protected var logScroll:ScrollView? = null

    protected var logView: TextView? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(layotID)
        portEdit = findViewById(R.id.port_edit)
        startButton = findViewById(R.id.start_button)
        logScroll = findViewById(R.id.log_scroll)
        logView = findViewById(R.id.log_view)
        startButton!!.setOnClickListener(this)
        /*startButton!!.setOnClickListener {
            onStartButtonClicked()
        }*/
    }

    override fun onClick(v: View?) {
        if (v == startButton){
            onStartButtonClicked()
        }
    }

    abstract fun onStartButtonClicked()

    protected fun getPort():Int?{
        var port:Int?
        try{
            port = Integer.parseInt(portEdit!!.text.toString())
        }catch (e:Exception){
            port = null
        }

        return port
    }

    /**
     * 记录给定的消息
     */
    protected fun logMessage(message:String){
        runOnUiThread {
            logMessageDirect(message)
        }
    }

    /**
     * 直接记录给定的消息
     */
    protected fun logMessageDirect(message:String){
        logView!!.append(message)
        logView!!.append("\n")
        logScroll!!.fullScroll(View.FOCUS_DOWN)
    }
    protected abstract inner class AbstractEchoTask():Thread(){
        private val handler: Handler
        init {
            handler = Handler()
        }
        //在调用线程中先执行回调
        protected fun onPreExecute(){
            startButton!!.isEnabled = false
            logView!!.text = ""
        }
        @Synchronized
        override fun start() {
            onPreExecute()
            super.start()
        }

        override fun run() {
           onBackground()
            handler.post {
                onPostExecute()
            }
        }
        //新线程中的背景回调
        protected abstract fun onBackground()
        //在调用线程中后执行回调
        protected fun onPostExecute(){
            startButton!!.isEnabled = true
        }
    }

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}