package com.study.ndk35_opencv_study

import android.view.Surface


class FaceTracker(model: String) {

    private val NV = NativeHelper.instance


    private var mNativeObj = 0L

    init {
        mNativeObj = NV.nativeCreateObject(model)
    }

    // 人脸定位
    fun setSurface(surface: Surface?) {
        NV.nativeSetSurface(mNativeObj, surface)
    }

    fun release() {
        NV.nativeDestroyObject(mNativeObj)
        mNativeObj = 0
    }

    // 开启人脸跟踪
    fun start() {
        NV.nativeStart(mNativeObj)
    }

    fun stop() {
        NV.nativeStop(mNativeObj)
    }

    // 人脸定位 同时显示在Surface中
    fun detect(inputImage: ByteArray, width: Int, height: Int, rotationDegrees: Int): ByteArray {
        return NV.nativeDetect(mNativeObj, inputImage, width, height, rotationDegrees)
    }
}