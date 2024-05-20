//
// Created by 王一 on 2024/5/20.
//

#include "FaceTracker.h"

FaceTracker::FaceTracker(const char *model) {
    pthread_mutex_init(&mutex, 0);
    Ptr<CascadeDetectorAdapter> mainDetector = makePtr<CascadeDetectorAdapter>(
            makePtr<CascadeClassifier>(model));
    Ptr<CascadeDetectorAdapter> trackingDetector = makePtr<CascadeDetectorAdapter>(
            makePtr<CascadeClassifier>(model));
    DetectionBasedTracker::Parameters params;
    tracker = makePtr<DetectionBasedTracker>(mainDetector, trackingDetector, params);
}

FaceTracker::~FaceTracker() {
    pthread_mutex_destroy(&mutex);
    if (window) {
        ANativeWindow_release(window);
        window = 0;
    }
}

void FaceTracker::setANativeWindow(ANativeWindow *window) {
    pthread_mutex_lock(&mutex);
    if (this->window) {
        ANativeWindow_release(this->window);
    }
    this->window = window;
    pthread_mutex_unlock(&mutex);
}

// I420数据
//void FaceTracker::detect(char *img, int width, int height, int rotatinDegress) {
void FaceTracker::darw(Mat img) {
    pthread_mutex_lock(&mutex);
    do {
        if (!window) {
            break;
        }
        ANativeWindow_setBuffersGeometry(window, img.cols, img.rows, WINDOW_FORMAT_RGBA_8888);
        ANativeWindow_Buffer buffer;
        if (ANativeWindow_lock(window, &buffer, 0)) {
            break;
        }
        uint8_t *dst_data = static_cast<uint8_t *>(buffer.bits);
        int dst_linesize = buffer.stride * 4;
        uint8_t *src_data = img.data;
        int src_linesize = img.cols * 4;
        for (int i = 0; i < img.rows; ++i) {
            memcpy(dst_data + i * dst_linesize, src_data + i * src_linesize, src_linesize);
        }
        ANativeWindow_unlockAndPost(window);
    } while (0)
}
