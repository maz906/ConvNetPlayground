#include "imgretrieval.h"
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
using namespace cv;

imgRetrieval::imgRetrieval()
{
    m_trainFileName="bookList_v2";
    m_testFileName="testList";
    m_path="../../book-data/";
    m_featurePath="../../book-data/features/";
    m_hkmFile="hkmFile";
    m_ivfFile="ivfFile";
    m_hkmOpt.nlevels=7;
    m_hkmOpt.nbranches=10;
    m_hkmOpt.niters=20;
    m_hkmOpt.usekdt=false;
    m_phkm=NULL;
    m_phkm=new Hkms<float>(m_hkmOpt);
    m_numImgForBow=100000;
    m_wt = ivFile::WEIGHT_TFIDF;
    m_norm = ivFile::NORM_L1;
    m_numRet=10;
    m_dist=ivFile::DIST_L1;
}

void imgRetrieval::splitFilename (const string& str,string& file)
{
    size_t found;
    found=str.find_last_of("/\\");
    string folder=str.substr(0,found);
    found=folder.find_last_of("/\\");
    file=str.substr(found+1);
}
void imgRetrieval::readTrainData()
{
    readList(m_path,m_trainFileName,m_trainNames,m_trainBoundingBoxs,m_trainGroundTruth);
}

void imgRetrieval::readTestData()
{
    readList(m_path,m_testFileName,m_testNames,m_testBoundingBoxs,m_testGroundTruth);

}
void imgRetrieval::readList(string& path,string& fileName,vector<string>& names,
                            vector<boundingBox>& boundingBoxs,vector<int>& groundTruthLabels)
{
    boundingBoxs.clear();
    names.clear();
    groundTruthLabels.clear();
    ifstream fin;
    string fileT=path+fileName;
    fin.open(fileT.c_str());
    if (fin.is_open())
    {
        string line;
        while(getline(fin,line))
        {
            std::size_t pos=line.find(" ");
            string image_name=path+line.substr(0,pos);
            names.push_back(image_name);
            string other=line.substr(pos);
            std::stringstream ss(other);
            // now read bounding box;
            boundingBox temp;
            ss>>temp.m_left>>temp.m_top>>temp.m_width>>temp.m_height;
            boundingBoxs.push_back(temp);
            // now read label;
            int label;
            ss>>label;
            groundTruthLabels.push_back(label);
        }
        fin.close();
    }else
    {
        cout<<fileT<<" cannot be opened\n";
        exit(-1);
    }
}

void imgRetrieval::extractTrainFeatures()
{
    for(int i=0;i<m_trainNames.size();i++)
    {
        vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
        extractFeature(m_trainNames.at(i),keypoints,descriptors,m_featurePath);
        cout<<"processing"<< i<<"th img, getting"<<keypoints.size()<<"feature points\n"<<endl;
    }
}
void imgRetrieval::extractImgFeature(cv::Mat &img, vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors)
{
    cv::SURF surf_extractor;
	cout<< "success 1" << endl;
    surf_extractor(img,cv::Mat(),keypoints,descriptors);
	cout<< "success 2" << endl;
    if(keypoints.size()<1)
    {
        cout<<"Extract 0 feature points"<<endl;
        exit(-1);
    }
}

void imgRetrieval::extractFeature(string& imgName,vector<cv::KeyPoint>& keypoints,cv::Mat& descriptors,string& featurePath)
{
    cv::Mat img=cv::imread(imgName);
    string featureFile;
    getFeatureName(imgName,featurePath,featureFile);
    extractImgFeature(img,keypoints,descriptors);
    cv::FileStorage fs;
    if(fs.open(featureFile, cv::FileStorage::WRITE))
    {
        fs<<"keypoints"<<keypoints;
        fs<<"descriptors"<<descriptors;
        fs.release();
    }else
    {
        cout<<"open "<<featureFile<<" failed!"<<endl;
        exit(0);
    }

}
void imgRetrieval::getFeatureName(string& imgName,string& featurePath,string& featureFile)
{
    string file;
    splitFilename(imgName, file);
    featureFile=featurePath+file;
    int len=featureFile.size();
    featureFile[len-1]='l';
    featureFile[len-2]='m';
    featureFile[len-3]='x';
}
void imgRetrieval::buildHKM()
{
    cv::Mat features;
    bool cp=false;
    selectBOWTrainData(features);
    cout<<"pack features for BoW"<<endl;
    /*  if (!features.isContinuous() )
    {
        features = features.clone();

    }*/
    Data<float> data;                                            \
    cvMat2Data(features,data,cp);
    m_phkm->create(data);
    cout<<"BOW building finished\n";
    m_phkm->saveFile(m_hkmFile);                                                       \
}
void imgRetrieval::loadHKM()
{
    m_phkm->loadFromFile(m_hkmFile);
}
void imgRetrieval::selectBOWTrainData(cv::Mat& featuresMat)
{
    int num;
    int num_img=m_trainNames.size();
    num=std::min(m_numImgForBow,num_img);
    vector<int> order;
    for (int i=0; i<num_img; ++i)
    {
        order.push_back(i); // 1 2 3 4 5 6 7 8 9
    }
    std::random_shuffle (order.begin(), order.end());
    int dim;
    int num_feature=0;
    for(int i=0;i<num;i++)
    {
        string imgName=m_trainNames.at(order.at(i));
        string file;
        splitFilename(imgName, file);
        string featureFile;
        getFeatureName(imgName,m_featurePath,featureFile);
        cv::Mat mat;
        cv::FileStorage fs;
        if(fs.open(featureFile, cv::FileStorage::READ))
        {
            fs["descriptors"]>>mat;
            fs.release();
            if(i==0)
            {
                dim=mat.cols;
            }
            num_feature+=mat.rows;
        }
    }
    featuresMat.create(num_feature,dim,CV_32FC1);
    cout<<featuresMat.isContinuous()<<endl;
    assert(featuresMat.isContinuous());
    int startInx=0;
    int endInx;
    //copy data
    for(int i=0;i<num;i++)
    {
        string imgName=m_trainNames.at(order.at(i));
        cv::Mat mat;
        cv::FileStorage fs;
        string featureFile;
        getFeatureName(imgName,m_featurePath,featureFile);
        if(fs.open(featureFile, cv::FileStorage::READ))
        {
            fs["descriptors"]>>mat;
            endInx=startInx+mat.rows;
            mat.copyTo(featuresMat.rowRange(startInx,endInx));
            fs.release();
            startInx=endInx;
        }else
        {
            cout<<"open file "<<featureFile<<" failed\n";
        }
    }
}
void imgRetrieval::cvMat2Data(cv::Mat &features, Data<float> &data, bool &cp)
{
    data.type = DATA_FIXED;
    assert(features.rows>0&&features.cols>0);
    data.npoints =features.rows;// (uint)mxGetN(points);
    data.ndims =features.cols; //(uint)mxGetM(points);
    data.setFixed((float*) features.data, cp);
}
void imgRetrieval::buildIVF()
{
    Data<float> docs;
    docs.type = DATA_VAR;
    docs.npoints = m_trainNames.size();
    docs.allocate();
    uint nn;
    for(uint i=0;i<m_trainNames.size();i++)
    //for(uint i=0;i<10;i++)
    {
        cout<<"Index "<<i<<"th image in list v2"<<endl;
        string featureFile;
		
        getFeatureName(m_trainNames.at(i),m_featurePath,featureFile);
        cv::Mat mat;
        cv::FileStorage fs;
        if(fs.open(featureFile, cv::FileStorage::READ))
        {
            fs["descriptors"]>>mat;
            fs.release();
        }else
        {
            cout<<"Open file failed"<<endl;
        }
		
        //get word assignment
        Data<float> points;
        bool isCopy=false;
		
        cvMat2Data(mat,points,isCopy);

        nn = points.npoints;                                                       
        if(nn==0)
        {
            cout<<"nn==0"<<endl;
            exit(1);
        }

        /*compute*/
        uint* classId=new uint[nn];
        float* classId_f=new float[nn];
		assert(classId!=NULL);
        m_phkm->getLeafIds(points, classId);
        for(int k=0;k<nn;k++)
        {
            classId_f[k]=classId[k];
            //cout<<classId_f[k]<<" ";
        }
		
        //cout<<endl;
	
        pair<float*, uint> pv;
        pv.first=classId_f;
        pv.second=nn;
        docs.setVarPoint(pv.first, pv.second, i, true);
        delete[] classId;
        delete[] classId_f;
    }
    uint nwords=pow(m_phkm->opt.nbranches ,m_phkm->opt.nlevels);
    ivFillFile(m_ivfile,docs,nwords,0);
    m_ivfile.computeStats(m_wt,m_norm);
    m_ivfile.save(m_ivfFile);
}
void imgRetrieval::loadIVF()
{
    m_ivfile.load(m_ivfFile);
}

imgRetrieval::~imgRetrieval()
{
    m_phkm->clear();
    m_phkm=NULL;
}
float imgRetrieval::testBatch()
{
    readTestData();
    //
    uint num=m_testNames.size();
    int sum=0;
    for(int i=0;i<num;i++)
    {
        string name=m_testNames.at(i);
        int label=m_testGroundTruth.at(i);
        cout<<"Read image"<<name<<endl;
        //cv::Mat img=cv::imread(name); //ZF
        vector<int> predictL=predictImg(name);//  ZF
        /*cout<<predictL<<endl;
        if(predictL==label)
        {
            sum++;
        }*/
    }
    return sum*1.0/num;
}