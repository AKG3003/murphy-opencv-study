package com.study.ndk35_opencv_study

import android.view.Surface

class NativeHelper private constructor() {

    companion object {
        // Used to load the 'ndk35_opencv_study' library on application startup.
        init {
            System.loadLibrary("ndk35_opencv_study")
        }

        val instance = NativeHelper()
    }


    external fun nativeCreateObject(model: String): Long

    external fun nativeDestroyObject(thiz: Long)

    external fun nativeSetSurface(thiz: Long, surface: Surface?)

    external fun nativeStart(thiz: Long)

    external fun nativeStop(thiz: Long)

    external fun nativeDetect(
        thiz: Long,
        inputImage: ByteArray,
        width: Int,
        height: Int,
        rotationDegrees: Int
    ): ByteArray
}