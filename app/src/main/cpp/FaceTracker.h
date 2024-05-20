//
// Created by 王一 on 2024/5/20.
//

#ifndef NDK35_OPENCV_STUDY_FACETRACKER_H
#define NDK35_OPENCV_STUDY_FACETRACKER_H

#include <opencv2/opencv.hpp>
#include <jni.h>
#include <android/native_window_jni.h>

using namespace cv;

class CascadeDetectorAdapter : public DetectionBasedTracker::IDetector {
public:
    CascadeDetectorAdapter(cv::Ptr<cv::CascadeClassifier> detector) :
            IDetector(),
            Detector(detector) {
        CV_Assert(detector);
    }

    void detect(const cv::Mat &Image, std::vector<cv::Rect> &objects) {
        Detector->detectMultiScale(Image, objects, scaleFactor, minNeighbours, 0, minObjSize,
                                   maxObjSize);
    }

    virtual ~CascadeDetectorAdapter() {}

private:
    CascadeDetectorAdapter();

    cv::Ptr<cv::CascadeClassifier> Detector;
};

class FaceTracker {
public:
    FaceTracker(const char *model);

    ~FaceTracker();

    void setANativeWindow(ANativeWindow *window);

    void darw(Mat img);

    Ptr<DetectionBasedTracker> tracker;
    pthread_mutex_t mutex;
    ANativeWindow *window = 0;
};


#endif //NDK35_OPENCV_STUDY_FACETRACKER_H
