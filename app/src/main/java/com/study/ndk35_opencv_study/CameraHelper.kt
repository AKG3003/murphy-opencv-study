package com.study.ndk35_opencv_study

import android.content.Context
import android.util.Size
import android.view.SurfaceHolder
import android.view.SurfaceView
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.core.content.ContextCompat
import androidx.lifecycle.LifecycleOwner

class CameraHelper : SurfaceHolder.Callback, ImageAnalysis.Analyzer {

//    companion object {
//        private var mInstance: CameraHelper? = null
//        private var mutex = Mutex()
//
//        suspend fun getInstance(): CameraHelper {
//            mutex.withLock {
//                if (mInstance == null) {
//                    mInstance = CameraHelper()
//                }
//            }
//            return mInstance!!
//        }
//    }

    private var mFaceTracker: FaceTracker? = null
    private var mSurfaceView: SurfaceView? = null
    private var mCameraProvider: ProcessCameraProvider? = null


    override fun surfaceCreated(holder: SurfaceHolder) {
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        mFaceTracker?.setSurface(holder.surface)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        mFaceTracker?.setSurface(null)
    }

    override fun analyze(image: ImageProxy) {
        val bytes = Utils.getDataFromImage(image)
        val result = mFaceTracker?.detect(bytes, image.width, image.height, 0)
        image.close()
    }

    fun release() {
        mFaceTracker?.release()
    }

    fun initCamera(context: Context, surfaceView: SurfaceView) {
        // 这里是人脸定位的模型
        val path = Utils.copyAsset2Dir(context, "")
        mFaceTracker?.start()
        mSurfaceView = surfaceView
        mSurfaceView?.holder?.addCallback(this)

        val providerF = ProcessCameraProvider.getInstance(context)
        providerF.addListener({
            try {
                mCameraProvider = providerF.get()
                bindAnalysis(context)
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }, ContextCompat.getMainExecutor(context))
    }

    private fun bindAnalysis(context: Context) {
        // 得到摄像头图像数
        val imageAnalysis = ImageAnalysis.Builder()
            .setTargetResolution(Size(640, 480))
            .setBackpressureStrategy(ImageAnalysis.STRATEGY_KEEP_ONLY_LATEST)
            .build()
        imageAnalysis.setAnalyzer(ContextCompat.getMainExecutor(context), this)
        mCameraProvider?.unbindAll()
        mCameraProvider?.bindToLifecycle(
            context as LifecycleOwner,
            CameraSelector.DEFAULT_BACK_CAMERA,
            imageAnalysis
        )
    }

}