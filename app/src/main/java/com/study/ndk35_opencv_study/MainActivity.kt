package com.study.ndk35_opencv_study

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import com.googlecode.tesseract.android.TessBaseAPI
import com.study.ndk35_opencv_study.databinding.ActivityMainBinding
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.io.File
import java.io.FileOutputStream
import java.io.InputStream

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    private var baseApi: TessBaseAPI? = null

    private val coroutineScope = CoroutineScope(Dispatchers.Main)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method
        binding.sampleText.text = stringFromJNI()
        coroutineScope.launch {
            initTess()
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        baseApi?.end()
    }

    public fun recognition(view: View) {
        // 应该吧native的图片传入，这里凑合着实例。
        val result = Bitmap.createBitmap(199, 199, Bitmap.Config.ARGB_8888)
        baseApi?.setImage(result)
        baseApi?.utF8Text // 得到文字识别的结果
        baseApi?.clear()
    }

    private external fun stringFromJNI(): String

    private fun initTess() {
        baseApi = TessBaseAPI()
        try {
            val inputStream = assets.open("eng.traineddata")
            val file = File.createTempFile("eng", "traineddata")
            if (!file.exists()) {
                file.parentFile?.mkdirs()
                val fos = FileOutputStream(file)
                val buffer = ByteArray(1024)
                while (inputStream.read(buffer) != -1) {
                    fos.write(buffer)
                }
                fos.close()
            }
            inputStream.close()
            baseApi?.init("", "eng")
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    companion object {
        // Used to load the 'ndk35_opencv_study' library on application startup.
        init {
            System.loadLibrary("ndk35_opencv_study")
        }
    }
}