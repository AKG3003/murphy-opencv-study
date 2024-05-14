#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>

#define DEFAULT_CARD_WIDTH 640
#define DEFAULT_CARD_HEIGHT 480
#define FIX_IDCARD_SIZE Size(DEFAULT_CARD_WIDTH, DEFAULT_CARD_HEIGHT)
#define FIX_TEMPLATE_SIZE Size(640, 480)

#define NELEM(x) (sizeof(x) / sizeof((x)[0]))

using namespace cv;
using namespace std;

JavaVM *g_javaVM = nullptr;

extern "C" JNIEXPORT jstring JNICALL
stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_org_opencv_android_Utils_nBitmapToMat2(JNIEnv *env, jobject instance, jobject bitmap,
                                            jlong m_addr, jboolean needUnPremultiplyAlpha);
extern "C"
JNIEXPORT void JNICALL
Java_org_opencv_android_Utils_nMatToBitmap2(JNIEnv *env, jobject instance, jlong m_addr, jobject bitmap);

jobject createBitmap(JNIEnv *env, Mat srcData, jobject config) {
    //获取Bitmap类
    jclass bitmapCls = env->FindClass("android/graphics/Bitmap");
    //获取Bitmap.createBitmap方法
    jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls, "createBitmap",
                                                            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    //调用createBitmap方法
    jobject bitmap = env->CallStaticObjectMethod(bitmapCls, createBitmapFunction, srcData.cols,
                                                 srcData.rows, config);
    return bitmap;
}


void handleImage2(Mat &src) {
    Mat src_image = src;
    Mat dst_img;
    imshow("src", src_image);

    Mat dst;

    // 降噪，进行无损压缩
    resize(src_image, src_image, FIX_IDCARD_SIZE);

    // 灰度化 提升速度 2^24 -> 2^8）
    cvtColor(src_image, dst, COLOR_BGR2GRAY);

    //二值化
    threshold(dst, dst, 100, 255, THRESH_BINARY);

    // 膨胀
    Mat element = getStructuringElement(MORPH_RECT, Size(20, 20));
    // 腐蚀
    erode(dst, dst, element);
    dilate(dst, dst, element);

    // 查找轮廓
    std::vector<std::vector<Point>> contours;// 轮廓 通过两个点一个向量确定一个矩形，但是有很多矩形，所以是双层。
    std::vector<Rect> hierarchy;// 轮廓的层次结构
    // ContourApproximationModes: 轮廓逼近方法
    // RetrievalModes: 轮廓检索模式
    findContours(dst, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    for (int i = 0; i < contours.size(); i++) {
        Rect rect = boundingRect(contours[i]);
        // 绘制矩形轮廓像素点厚度，如果是负值则进行填充
        // 对符合条件的图片进行筛选。TODO:根据不同任务规则不同
        if (rect.width > rect.height * 9) {
            hierarchy.push_back(rect);
            rectangle(dst, rect, Scalar(0, 0, 255), 2);
            dst_img = src_image(rect);
        }
    }

    if (hierarchy.size() > 1) {
        // 如果有多个区域，添加一个规则用来筛选，这里是找最下面的矩形块
        int lowPoint = 0;
        Rect finalRect;
        for (int i = 0; i < hierarchy.size(); ++i) {
            Rect rect = hierarchy[i];
            Point p = rect.tl();
            if (rect.tl().y > lowPoint) {
                lowPoint = rect.tl().y;
                finalRect = rect;
            }
        }
        dst_img = src_image(finalRect);
    } else if (hierarchy.size() == 1) {
        // 如果只有一个区域
        Rect rect = hierarchy[0];
        dst_img = src_image(rect);
    } else {
        // 如果没有找到
    }
}

void do_start(JNIEnv *env, jobject instance, jstring path) {
    const char *path_ = env->GetStringUTFChars(path, 0);
    char *global_path = new char[strlen(path_) + 1];
    strcpy(global_path, path_);

    Mat src = imread(global_path);
    handleImage2(src);
    waitKey(0);

    env->ReleaseStringUTFChars(path, path_);
}


static const JNINativeMethod gMethods[] = {
        {"stringFromJNI", "()Ljava/lang/String;", (void *) stringFromJNI},
};

// 通过JNI_OnLoad函数动态注册
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    ::g_javaVM = vm;
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    jclass clazz = env->FindClass("com/study/ndk35_opencv_study/MainActivity");
    if (clazz == nullptr) {
        return JNI_ERR;
    }

    // 第一个车参数，你实现在Java中那个类里面就是那个类的全路径。
    // 第二个参数，是一个数组，里面是JNINativeMethod结构体，这个结构体里面是方法名，方法签名，方法指针。
    // 第三个参数，是数组的长度。一般通过NELEM(gMethods)来获取数组的长度。
    if (0 > env->RegisterNatives(clazz, gMethods, NELEM(gMethods))) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}