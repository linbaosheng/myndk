package com.demo.lbs.myndk

class KotlinDemo {
    private val h: String

    init {
        h = String()
    }

    protected abstract inner class AbstractE : Thread() {

        protected fun onPreExecute() {
            h
        }

        @Synchronized
        override fun start() {
        }
    }

    companion object {
        init {
            System.loadLibrary("Echo")
        }
    }
}
