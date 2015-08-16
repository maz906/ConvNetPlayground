#ifndef IMGRETRIEVAL_H
#define IMGRETRIEVAL_H
#include <vector>
#include <opencv2/opencv.hpp>
#include "ccData.hpp"
#include "ccHKmeans.hpp"
#include <opencv2/nonfree/nonfree.hpp>
#include "opencv2/core/core.hpp"
#include <opencv2/nonfree/features2d.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "ccMatrix.hpp"
#include "ccInvertedFile.hpp"

using namespace std;
struct boundingBox{
    int m_top;
    int m_left;
    int m_width;
    int m_height;
};
class rankRatio{
public:
    double m_value;
    int m_index;

};
inline bool operator<(const rankRatio& a,const rankRatio& b)
{
    return b.m_value < a.m_value;
}

class imgRetrieval
{
public:
    string m_path;
    string m_featurePath;
    string m_trainFileName;
    string m_testFileName;
    string m_hkmFile;
    string m_ivfFile;
    vector<boundingBox> m_trainBoundingBoxs;
    vector<int>   m_trainGroundTruth;
    vector<string> m_trainNames;

    vector<boundingBox> m_testBoundingBoxs;
    vector<int>   m_testGroundTruth;
    vector<string> m_testNames;

    HkmOptions m_hkmOpt;
    Hkms<float>* m_phkm;
    ivFile m_ivfile;
    int m_numImgForBow;
    int m_numRet;
    ivFile::Weight m_wt;
    ivFile::Norm m_norm;
    ivFile::Dist m_dist;

    imgRetrieval();
    ~imgRetrieval();
    void splitFilename (const string& str,string& file);
    void readTrainData();
    void readTestData();
    void readList(string& path,string& fileName,vector<string>& names,
                  vector<boundingBox>& boundingBoxs,vector<int>& groundTruthLabels);
    void extractTrainFeatures();
    void extractFeature(string& imgName,vector<cv::KeyPoint>& keypoints,cv::Mat& mat,string& featurePath);
    void extractImgFeature(cv::Mat& img,vector<cv::KeyPoint>& keypoints,cv::Mat& mat);
    void getFeatureName(string& imgName, string& featurePath,string& featureFile);
    void selectBOWTrainData(cv::Mat& featuresMat);
    void cvMat2Data(cv::Mat& features,Data<float>& data,bool& cp);
    void buildHKM();
    void loadHKM();
    void buildIVF();
    void loadIVF();
    float testBatch();
};

#endif // IMGRETRIEVAL_H
