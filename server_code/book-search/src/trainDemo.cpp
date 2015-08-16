#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <assert.h>
#include <opencv2/opencv.hpp>
#include "imgretrieval.h"
using namespace std;
imgRetrieval* loadImgRetrievalSys()
{
    imgRetrieval* pIr=new imgRetrieval();
    pIr->readTrainData();
    pIr->loadHKM();
    pIr->buildIVF();
    return pIr;
}
void searchImg(imgRetrieval* pIr,string imgFile,vector<int>& Ids)
{

     pIr->predictImg(imgFile,Ids);
}
void getImgName(imgRetrieval* pIr,int id, string& imgFile)
{
    imgFile=pIr->m_trainNames.at(id);
}

int main()
{
    imgRetrieval ir;
    ir.readTrainData();  //must have it.
    ir.extractTrainFeatures(); // have it or not
    //ir.buildHKM();    //buildHKM or loadHKM
    ir.loadHKM();   //make buildHKM correct
    ir.buildIVF();    //build IVF or load IVF
    //ir.loadIVF();   //not correct
    float acc=ir.testBatch();
    cout<<"The accuracy is "<<acc<<endl;
}
