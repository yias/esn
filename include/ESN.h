/* ESN class
 *
 * Iason Batzianoulis
 *
 * describtion:
 *
 *
 *
 */

#include <iostream>
#include <string.h>
#include <sstream>
#include <fstream>
#include <math.h>
#include <vector>



class ESN
{
int nforgetpoints;
int nInternalUnits;
int nInputs;
int nOutputs;
std::vector< std::vector<float> > internalWeightsUnitSR;//(nInternalUnits, std::vector<float>(nInternalUnits));
int nTotalUnits;
int col_outWeights;

std::vector< std::vector<float> > inputWeights;//(nInternalUnits,std::vector<float>(nInputs));
std::vector< std::vector<float> > outputWeights;//(nOutputs,std::vector<float>(col_outWeights));
std::vector< std::vector<float> > feedbackWeights;//(nInternalUnits, std::vector<float>(nOutputs));
std::vector<float> inputScaling;//(nInputs);
std::vector<float> inputShift;//(nInputs);
std::vector<float>  teacherScaling;//(nOutputs);
std::vector<float>  teacherShift;//(nOutputs);

std::vector<double> feedbackScaling;//(nOutputs);
float leakage;
std::vector<float> timeConstants;//(nInternalUnits);
std::vector< std::vector<float> > internalWeights;//(nInternalUnits,std::vector<float>(nInternalUnits));

public:
    ESN(void);
    ESN(int esn_nb);
    int setinterntalWUSR(std::ifstream& f, int k, int l);

    int setinputWeights(std::ifstream& f, int k, int l); //
    int setoutputWeights(std::ifstream& f, int k, int l); //
    int setfeedbackWeights(std::ifstream& f, int k, int l); //
    int setinputScaling(std::ifstream& f, int k); //
    int setinputShift(std::ifstream& f, int k); //
    int setteacherScaling(std::ifstream& f, int k); //
    int setteacherShift(std::ifstream& f, int k); //

    int setfeedbackScaling(std::ifstream& f, int k); //
    int settimeConstants(std::ifstream& f, int k);
    int setinternalWeights(std::ifstream& f, int k, int l);
    int printall();

    int test_esn(double **EMGsignal, int nforgetpoints, int nDatapoints);

    std::vector< std::vector<double> > compute_statematrix(double **EMGsignal, int nforgetpoints, int nDatapoints);
    std::vector<double> plain_esn(std::vector<double> tState);
    int s_classify(std::vector<std::vector<double> > outputSeq, int npoints);

    ~ESN(void);

};


ESN::ESN(void)
{
    nInternalUnits=1;

    std::cout<<"esn created\n";
}


ESN::~ESN(void)
{
//    std::cout<<"size: "<<sizeof(internalWeightsUnitSR)/(nInternalUnits*nInternalUnits)<<"\n";
//    std::cout<<"size2: "<<nInternalUnits<<"\n";

//    for (int i = 0; i < nInternalUnits; i++){
//        if(internalWeightsUnitSR[i]) delete[] internalWeightsUnitSR[i];
//        //std::cout<<"ok1 "<<i<<"\n";
//    }

//    if(internalWeightsUnitSR) delete[] internalWeightsUnitSR;


//    //delete[] internalWeightsUnitSR;
//    for (int i = 0; i < nInternalUnits; i++){
//        if(inputWeights[i]) delete[] inputWeights[i];
//	}
//    //delete inputWeights;
//	for (int i = 0; i < nOutputs; i++){
//        if(outputWeights[i]) delete[] outputWeights[i];
//	}
//    //delete outputWeights;
//	for (int i = 0; i < nInternalUnits; i++){
//        if(feedbackWeights[i]) delete[] feedbackWeights[i];
//	}
//    //delete feedbackWeights;

//    if(inputScaling) delete[] inputScaling;
//    if(inputShift) delete[] inputShift;
//    if(teacherScaling) delete[] teacherScaling;
//    if(teacherShift) delete[] teacherShift;

//    if(feedbackScaling) delete[] feedbackScaling;
//    if(timeConstants) delete[] timeConstants;

//	for (int i = 0; i < nInternalUnits; i++){
//        if(internalWeights[i]) delete[] internalWeights[i];
//	}
//    //delete internalWeights;

    std::cout << "esn deleted\n";
    //cin.get();
}

ESN::ESN(int esn_nb){

    //char line[100],line2[100];
    //int tn;

    //std::cout << file <<"\n";
    std::string file="src/esn/ESN_network/ESN";
    std::ostringstream convert;
    convert<<file<<esn_nb<<".txt";

    std::string nfile=convert.str();
    std::ifstream myfile(nfile.c_str());
    std::cout<<nfile.c_str()<<"\n";

    if(myfile.is_open()){


        //myfile>>line;
        //cout<<line<<" ";
        myfile>>nforgetpoints;
        myfile>>nInternalUnits;
        myfile>>nInputs;
        myfile>>nOutputs;
        col_outWeights=nInternalUnits+nInputs;

        setinterntalWUSR(myfile, nInternalUnits, nInternalUnits);
        //std::cout<<"size: "<<sizeof(internalWeightsUnitSR)/sizeof(float)<<"\n";

        //std::cout<<"internal units: "<<nInternalUnits<<"\n";
        myfile>>nTotalUnits;

        setinputWeights(myfile, nInternalUnits, nInputs);
        setoutputWeights(myfile, nOutputs, (nInternalUnits+nInputs));
        setfeedbackWeights(myfile, nInternalUnits, nOutputs);
        setinputScaling(myfile, nInputs);
        setinputShift(myfile, nInputs);
        setteacherScaling(myfile, nOutputs);
        setteacherShift(myfile, nOutputs);

        setfeedbackScaling(myfile, nOutputs);
        settimeConstants(myfile,nInternalUnits);
        myfile>>leakage;
        setinternalWeights(myfile,nInternalUnits, nInternalUnits);


        //myfile>>line2;
        //myfile>>nInputs;

        //std::cout<<nInternalUnits<<"\n";
        //cout<<line2<<"\n";
        //std::cout<<nInputs<<"\n";
        myfile.close();
//        std::cout<<"ok constructor\n";

    }
    else std::cout<<"Unable to open file"<<"\n";


}

int ESN::setinterntalWUSR(std::ifstream& f, int k, int l){
    //internalWeightsUnitSR=std::vector<std::vector<float> >(k,std::vector<float>(l));
    internalWeightsUnitSR.resize(k,std::vector<float>());
    //std::cout<<k<<"\n";
    //std::cout<<l<<"\n";

    //float **tmp= new float*[k];
    //internalWeightsUnitSR=new float*[k];

    for(int i=0;i<k;i++){
        //tmp[i]= new float[l];
        //internalWeightsUnitSR[i]=new float[l];
        double tmp=0;
        for(int j=0;j<l;j++){
            //float temp;
            //f>>tmp[i][j];
            f>>tmp;
            internalWeightsUnitSR[i].push_back(tmp);//[j];
            //cout<<tmp[i][j]<<" ";
            //cout<<tmp[i][j]<<endl;
            //inputWeights[i][j]=temp;
        }
    }

    //std::cout<<"\ninternal weights\n";
    //internalWeightsUnitSR=tmp;

    //std::cout<<tmp[50][50]<<" "<<internalWeightsUnitSR[50][50]<<"\n";

    //std::cout<<internalWeightsUnitSR[99][99]<<" "<<internalWeightsUnitSR[99][99]<<"\n";

//    for(int i=0;i<k;i++){
//        for(int j=0;j<l;j++){
//            std::cout<<internalWeightsUnitSR[i][j]<<" ";
//        }
//        std::cout<<"\n";
//    }

    //std::cout<<"\ninternal units:\n";
    //std::cout << internalWeightsUnitSR[0][1]<<" ";
    //std::cout << internalWeightsUnitSR[1][0] << " \n";

    //float result=internalWeightsUnitSR[0][1]+internalWeightsUnitSR[1][0];
    //std::cout<<"size3: "<<sizeof(internalWeightsUnitSR)<<"\n";
    //std::cout<<result<<"\n";
    return 0;
}


int ESN::setinputWeights(std::ifstream& f, int k, int l){
    //inputWeights=std::vector<std::vector<float> >(k,std::vector<float>(l));
    inputWeights.resize(k,std::vector<float>());
    //float **tmp= new float*[k];
    //inputWeights= new float*[k];
    double tmp=0;
    for(int i=0;i<k;i++){
        //inputWeights[i]=new float[l];
        //tmp[i]= new float[l];
        for(int j=0;j<l;j++){
            //f>>tmp[i][j];
            f>>tmp;
            inputWeights[i].push_back(tmp);//[j];
        }
    }
    //inputWeights=tmp;
    /*for(int i=0;i<k;i++){
        delete tmp[i];
    }
    delete tmp;*/
    return 0;
}


int ESN::setoutputWeights(std::ifstream& f, int k, int l){
    //outputWeights=std::vector<std::vector<float> >(k,std::vector<float>(l));
    outputWeights.resize(k,std::vector<float>());
    //float **tmp= new float*[k];
    //outputWeights=new float*[k];
    double tmp=0;
    for(int i=0;i<k;i++){
        //tmp[i]= new float[l];
        //outputWeights[i]=new float[l];
        for(int j=0;j<l;j++){
            //f>>tmp[i][j];
            f>>tmp;
            outputWeights[i].push_back(tmp);//[j];
        }
    }
    //outputWeights=tmp;
    /*for(int i=0;i<k;i++){
        delete tmp[i];
    }
    delete tmp;*/
    return 0;
}

int ESN::setfeedbackWeights(std::ifstream& f, int k, int l){
    //feedbackWeights=std::vector<std::vector<float> >(k,std::vector<float>(l));
    feedbackWeights.resize(k,std::vector<float>());
    //float **tmp= new float*[k];
    //feedbackWeights=new float*[k];
    double tmp=0;
    for(int i=0;i<k;i++){
        //tmp[i]= new float[l];
        //feedbackWeights[i]=new float[l];
        for(int j=0;j<l;j++){
        //	f>>tmp[i][j];
            f>>tmp;
            feedbackWeights[i].push_back(tmp);//[j];
        }
    }
    //feedbackWeights=tmp;
    /*for(int i=0;i<k;i++){
        delete tmp[i];
    }
    delete tmp;*/
    return 0;
}

int ESN::setinputScaling(std::ifstream& f, int k){
    //inputScaling.resize(k);
    //float *tmp= new float[k];
    //inputScaling=new float[k];
    double tmp=0;
    for(int i=0;i<k;i++) {
        f>>tmp;
        inputScaling.push_back(tmp);//[i];
    }//f>>tmp[i];
    //inputScaling=tmp;
    //delete tmp;
    //for(int i=0;i<k;i++) std::cout<<inputScaling[i]<<" ";
    //std::cout<<"\n";
    return 0;
}

int ESN::setinputShift(std::ifstream& f, int k){
    //inputShift.resize(k);//=std::vector<float>(k);
    //float *tmp= new float[k];
    //inputShift=new float[k];
    double tmp=0;
    for(int i=0;i<k;i++){
        f>>tmp;
        inputShift.push_back(tmp);//)[i];//f>>tmp[i];
    }
    //inputShift=tmp;
    //delete tmp;
    return 0;
}

int ESN::setteacherScaling(std::ifstream& f, int k){
    //teacherScaling.resize(k);//=std::vector<float>(k);

    //float *tmp= new float[k];
    //teacherScaling=new float[k];
    double tmp=0;
    for(int i=0;i<k;i++){
        f>>tmp;
        teacherScaling.push_back(tmp);
    }// f>>tmp[i
    //teacherScaling=tmp;
    //delete tmp;
    return 0;
}

int ESN::setteacherShift(std::ifstream& f, int k){
    //teacherShift.resize(k);//=std::vector<float>(k);
    //float *tmp= new float[k];
    //teacherShift=new float[k];
    double tmp=0;
    for(int i=0;i<k;i++){
        f>>tmp;
        teacherShift.push_back(tmp);//[i];
    }
    //teacherShift=tmp;
    //delete tmp;
    return 0;
}

int ESN::setfeedbackScaling(std::ifstream& f, int k){
    //feedbackScaling.resize(k);
    //float *tmp= new float[k];
    //feedbackScaling=new float[k];
    double tmp=0;
    for(int i=0;i<k;i++){
        f>>tmp;
        feedbackScaling.push_back(tmp);
    }
    //std::cout<<feedbackScaling.size()<<" size feedback teaching\n";
    return 0;
}

int ESN::settimeConstants(std::ifstream& f, int k){
    //timeConstants.resize(k);//=std::vector<float>(k);
    //float *tmp= new float[k];
    //timeConstants=new float[k];
    double tmp=0;
    for(int i=0;i<k;i++){
        f>>tmp;
        timeConstants.push_back(tmp);
    }
    //timeConstants=tmp;
    //delete tmp;
    return 0;
}

int ESN::setinternalWeights(std::ifstream& f, int k, int l){
    //internalWeights=std::vector<std::vector<float> >(k,std::vector<float>(l));
    //std::cout<<"ok1\n";
    internalWeights.resize(k,std::vector<float>());
    //float **tmp= new float*[k];
    //internalWeights=new float*[k];
    //std::cout<<"ok2\n";
    //std::cout<<"\ninternal Weights size: "<<internalWeights.size()<<"\n";
    //internalWeights[0].push_back(0);
    //std::cout<<"ok3\n";
    //std::cout<<" internal Weights size [0]: "<<internalWeights[0].size();
    for(int i=0;i<k;i++){
        //tmp[i]= new float[l];
        //internalWeights[i]=new float[l];
        double tmp=0;
        for(int j=0;j<l;j++){
            //f>>internalWeights[i][j];//f>>tmp[i][j];
            f>>tmp;
            internalWeights[i].push_back(tmp);
        }
    }
    //internalWeights=tmp;
    /*for(int i=0;i<k;i++){
        delete tmp[i];
    }
    delete tmp;*/
    //std::cout<<" internal Weights size [0]: "<<internalWeights[0].size()<<"\n";
    //std::cout<<"ok4\n";
    return 0;
}

int ESN::printall(){
    std::cout<<"number of Internal Units: "<<nInternalUnits<<std::endl;
    std::cout<<"number of Input Units: "<<nInputs<<std::endl;
    std::cout<<"number of Output Units: "<<nOutputs<<std::endl;
//    //std::cin.get();

    std::cout<<"internal Weights UnitSR"<<std::endl;

    for(int i=0;i<nInternalUnits;i++){
        for(int j=0;j<nInternalUnits;j++) std::cout<<internalWeightsUnitSR[i][j]<<" ";
        std::cout<<std::endl;
    }

    //std::cin.get();
    std::cout<<"number of Total Units: "<<nTotalUnits<<std::endl;
    //cin.get();
    std::cout<<"input Weights:"<<std::endl;
    for(int i=0;i<nInternalUnits;i++){
        for(int j=0;j<nInputs;j++) std::cout<<inputWeights[i][j]<<" ";
        std::cout<<std::endl;
    }
    //cin.get();
    std::cout<<"output Weights:"<<std::endl;
    for(int i=0;i<nOutputs;i++){
        for(int j=0;j<(nInputs+nInternalUnits);j++) std::cout<<outputWeights[i][j]<<" ";
        std::cout<<std::endl;
    }
    //cin.get();
    std::cout<<"feedback Weights:"<<std::endl;
    for(int i=0;i<nInternalUnits;i++){
        for(int j=0;j<nOutputs;j++) std::cout<<feedbackWeights[i][j]<<" ";
        std::cout<<std::endl;
    }
    //cin.get();
    std::cout<<"input Scalling:"<<std::endl;
    for(int i=0;i<nInputs;i++) std::cout<<inputScaling[i]<<" ";
    std::cout<<std::endl;
    //cin.get();
    std::cout<<"input Shift:"<<std::endl;
    for(int i=0;i<nInputs;i++) std::cout<<inputShift[i]<<" ";
    std::cout<<std::endl;
    //cin.get();
    std::cout<<"teacher Scaling:"<<std::endl;
    for(int i=0;i<nOutputs;i++) std::cout<<teacherScaling[i]<<" ";
    std::cout<<std::endl;
    //cin.get();
    std::cout<<"teacher Shift:"<<std::endl;
    for(int i=0;i<nOutputs;i++) std::cout<<teacherShift[i]<<" ";
    std::cout<<std::endl;
    //cin.get();
    std::cout<<"feedback Scaling:"<<std::endl;
    for(int i=0;i<nOutputs;i++) std::cout<<feedbackScaling[i]<<" ";
    std::cout<<std::endl;
    //std::cin.get();
    std::cout<<"time Constants:"<<std::endl;
    for(int i=0;i<nInternalUnits;i++) std::cout<<timeConstants[i]<<" ";
    std::cout<<std::endl;
    //std::cin.get();
    std::cout<<"internal Weights:"<<std::endl;
    for(int i=0;i<nInternalUnits;i++){
        for(int j=0;j<nInternalUnits;j++) std::cout<<internalWeights[i][j]<<" ";
        std::cout<<std::endl;
    }
    //std::cin.get();
    std::cout<<"leakage: "<<leakage<<std::endl;


    return 0;
}

int ESN::test_esn(double** EMGsignal, int nforgetpoints, int nDatapoints){
    //std::cout<<"test in\n";
    std::vector< std::vector<double> > stateCollection=compute_statematrix(EMGsignal,nforgetpoints,nDatapoints);

    //std::cout<<"state matrix: \n";
//    for(int i=0;i<nDatapoints-nforgetpoints;i++){
//        for(int j=0;j<nInternalUnits+nInputs;j++){
//            std::cout<<stateCollection[i][j]<<" ";
//        }
//        std::cout<<"\n";
//    }
    //std::cout<<"ok test2\n";///////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector< std::vector<double> > outputSequence(nDatapoints-nforgetpoints,std::vector<double>(nOutputs));//=new double*[nDatapoints-nforgetpoints];
    for(int i=0;i<nDatapoints-nforgetpoints;i++){
        //outputSequence[i]=new double[nOutputs];
        //std::cout<<"state mat size: "<<stateCollection[i].size()<<" outputWeights size: "<<outputWeights.size()<<"\n";
        for(int j=0;j<nOutputs;j++){
            double tmp2=0;
            for(int k=0;k<nInputs+nInternalUnits;k++){
                tmp2=stateCollection[i][k]*outputWeights[j][k]+tmp2;
                //std::cout<<outputWeights[j][k]<<" ";

            }
            //std::cout<<"\n";

            outputSequence[i][j]=tmp2;
        }

        //delete tmp;
    }
//    std::cout<<"output seq";
//    for(int i=0;i<nDatapoints-nforgetpoints;i++){
//		for(int j=0;j<nOutputs;j++){
//                std::cout<<outputSequence[i][j]<<" ";
//		}
//        std::cout<<"\n";
//    }
    //std::cout<<"ok test3\n";
    std::vector< std::vector<double> > repmat(nDatapoints-nforgetpoints,std::vector<double>(nOutputs));//new double*[nDatapoints-nforgetpoints];
    for(int i=0;i<(nDatapoints-nforgetpoints);i++){
        //repmat[i]=new double[nOutputs];
        for(int j=0;j<nOutputs;j++) repmat[i][j]=teacherShift[j];


    }
    //std::cout<<"output seq1:\n";
    for(int i=0;i<(nDatapoints-nforgetpoints);i++){
        for(int j=0;j<nOutputs;j++){
            outputSequence[i][j]=outputSequence[i][j]-repmat[i][j];
            //std::cout<<outputSequence[i][j]<<" ";
        }
        //std::cout<<"\n";
    }
    std::vector< std::vector<double> > invDiagTeacherScalling(nOutputs,std::vector<double>(nOutputs));//=new double*[nOutputs];
    for(int i=0;i<nOutputs;i++){
        //invDiagTeacherScalling[i]=new double[nOutputs];
        for(int j=0;j<nOutputs;j++){
            invDiagTeacherScalling[i][j]=0;
            if(i==j){
                invDiagTeacherScalling[i][j]=1/teacherScaling[i];
            }
        }
    }



    for(int i=0;i<nDatapoints-nforgetpoints;i++){
        for(int j=0;j<nOutputs;j++){
            float tmp2=0;
            for(int k=0;k<nOutputs;k++){
                tmp2=tmp2+outputSequence[i][k]*invDiagTeacherScalling[k][j];
            }
            outputSequence[i][j]=tmp2;
        }
    }


    int classification_result=s_classify(outputSequence,nDatapoints-nforgetpoints);

    //free memory
//    for(int i=0;i<(nDatapoints-nforgetpoints);i++){
//        delete[] stateCollection[i];
//        delete[] outputSequence[i];
//        delete[] repmat[i];
//    }
//    if(stateCollection) delete[] stateCollection;
//    if (outputSequence)delete[] outputSequence;
//    if(repmat) delete[] repmat;

//    for(int i=0;i<nOutputs;i++){
//        if(invDiagTeacherScalling[i]) delete invDiagTeacherScalling[i];
//    }
//    if(invDiagTeacherScalling) delete invDiagTeacherScalling;

    //std::cout<<"test out\n";
    return classification_result+1;
}

std::vector<std::vector<double> > ESN::compute_statematrix(double **EMGsignal, int nforgetpoints, int nDatapoints){
    //std::cout<<"ok csm1\n";///////////////////////////////////////////////////////////////////////////////////////
    std::vector<double> totalstate(nInternalUnits+nInputs+nOutputs);
    for(int i=0;i<nInternalUnits+nInputs+nOutputs;i++) totalstate[i]=0;

    std::vector<std::vector<double> > stateCollectMat(nDatapoints-nforgetpoints);//= new double*[nDatapoints-nforgetpoints];
    std::vector<double> netOut(nOutputs);
    std::vector<double> in(nInputs);
    std::vector<double> internalState;// = new double[nInternalUnits];
    std::vector<double> tin(nInternalUnits + nInputs);
    int collectdata=0;

    std::vector<double> tmp(nInternalUnits+nInputs,0);

    for(int i=0;i<nDatapoints;i++){

        for(int j=0;j<nInternalUnits+nInputs;j++) tmp[j]=0;
        //for(int j=0;j<nInternalUnits+nInputs;j++) std::cout<<tmp[j]<<" ";


//        for(int j=0;j<nInputs;j++) std::cout<<EMGsignal[i][j]<<" ";
//        std::cout<<"\n";

        //for(int j=0;j<nInputs;j++) std::cout<<inputScaling[j]<<" ";
        //std::cout<<"\n";
        for(int j=0;j<nInputs;j++){
            in[j]=inputScaling[j]*EMGsignal[i][j]+inputShift[j];
            //std::cout<<in[j]<<" ";
        }

        //std::cout<<"\n";

        for(int j=nInternalUnits;j<nInternalUnits+nInputs;j++){
            totalstate[j]=in[j-nInternalUnits];
        }


//        std::cout<<"ok csm2.4 \n";
        internalState=plain_esn(totalstate);

//        std::cout<<"ok csm2.5 ";
//        std::cout<<"nInternalUnits+nInputs (j): "<<nInternalUnits+nInputs<<" in size: "<<in.size()<<" internal units: "<<nInternalUnits<<" ti size: "<<tin.size()<<"\n";
//        std::cout<<"\nok "<<i<<":";
        for(int j=0;j<nInternalUnits+nInputs;j++){
            if(j<nInternalUnits) {
                tin[j]=internalState[j];
            }else {
                tin[j]=in[j-nInternalUnits];
            }
        }



        for(int j=0;j<nOutputs;j++){
            double tnumber=0;
            for(int k=0;k<nInternalUnits+nInputs;k++){
                tnumber=tnumber+outputWeights[j][k]*tin[k];
            }
            netOut[j]=tnumber;
        }

        for(int j=0;j<nInternalUnits;j++) totalstate[j]=internalState[j];

        for(int j=nInternalUnits+nInputs;j<nInternalUnits+nInputs+nOutputs;j++) totalstate[j]=netOut[j-(nInternalUnits+nInputs)];

        //std::cout<<"\n"<<tmp.size()<<"\n"<<nInternalUnits+nInputs<<"\n";




        for(int j=0;j<nInternalUnits+nInputs;j++) {tmp[j]=totalstate[j];}

//        std::cout<<"size tmp: "<<tmp.size()<<"\n";
//        std::cout<<"size statecollectMatrix: "<<stateCollectMat.size()<<"\n";
//        std::cout<<"forget points: "<<nforgetpoints<<"\n";
//        std::cout<<"collectdata: "<<collectdata<<"\n";
//        std::cout<<"size statecollectMatrix[collectdata]: "<<stateCollectMat[collectdata].size()<<"\n";
        if(i>nforgetpoints-1){
            //std::cout<<"ok2 "<<i<<"\n";
            stateCollectMat[collectdata]=tmp;//.push_back(tmp);
            collectdata=collectdata+1;
            //std::cout<<" ok\n";
        }

    }


//    delete netOut;
//    delete in;
//    delete internalState;
//    delete tin;

    //std::cout<<"ok csm3\n";
    return stateCollectMat;
}

std::vector<double> ESN::plain_esn(std::vector<double> tState){

    std::vector<std::vector<double> > tmp(nInternalUnits,std::vector<double>(nInternalUnits+nInputs+nOutputs));//=new double*[nInternalUnits];
    std::vector<std::vector<double> > diagfeedback(nOutputs, std::vector<double>(nOutputs));//=new double*[nOutputs];
//    std::cout<<"ok csm2\n";
//    std::cout<<feedbackScaling.size()<<" ";
//    std::cout<<"ok csm2\n";
    for(int i=0;i<nOutputs;i++){

        //diagfeedback[i]=new double[nOutputs];
        for(int j=0;j<nOutputs;j++){

            diagfeedback[i][j]=0;
            if(j==i) diagfeedback[i][j]=feedbackScaling[i];

        }
    }

//    std::cout<<"\n"<<internalWeights.size()<<" "<<internalWeights[0].size();
//    for(int i=0;i<100;i++){
//        for(int j=0;j<100;j++){
//            std::cout<<internalWeights[i][j]<<" ";
//        }
//        std::cout<<"\n";
//    }


    std::vector<std::vector<double> > feedS(nInternalUnits,std::vector<double>(nOutputs));//=new double*[nInternalUnits];
    for(int i=0;i<nInternalUnits;i++){
        //feedS[i]=new double[nOutputs];
        for(int j=0;j<nOutputs;j++){
            float tmp2=0;
            for(int k=0;k<nOutputs;k++){
                tmp2=tmp2+feedbackWeights[i][k]*diagfeedback[k][j];
            }
            feedS[i][j]=tmp2;
        }
    }

    //std::cout<<"feedS: "<<internalWeights[1].size()<<", tmp2: "<<tmp[0].size()<<"\n";
    for(int i=0;i<nInternalUnits;i++){
        //tmp[i]=new double[nInternalUnits+nInputs+nOutputs];
        for(int j=0;j<nInternalUnits+nInputs+nOutputs;j++){
            //  std::cout<<j<<" ";
            if(j<nInternalUnits) tmp[i][j]=internalWeights[i][j];
            if((j>=nInternalUnits)&&(j<nInternalUnits+nInputs)) tmp[i][j]=inputWeights[i][j-nInternalUnits];
            if(j>=nInternalUnits+nInputs) tmp[i][j]=feedS[i][j-(nInternalUnits+nInputs)];
        }
    }
    //std::cout<<"ok\n";

    std::vector<double> internalState(nInternalUnits);//=new double[nInternalUnits];
    for(int i=0;i<nInternalUnits;i++){
        double tmp2=0;
        for(int j=0;j<nInternalUnits+nInputs+nOutputs;j++){
            tmp2=tmp2+tmp[i][j]*tState[j];
        }
        internalState[i]=tanh(tmp2);
    }

    //std::cout<<"plain ok1 \n";
//    for(int i=0;i<nInternalUnits;i++){
//        //if(tmp[i]) delete[] tmp[i];
//        if(feedS[i]) delete[] feedS[i];
//    }
//    if(tmp) delete tmp;
//    if(feedS) delete feedS;
//    for(int i=0;i<nOutputs;i++) {if(diagfeedback[i]) delete[] diagfeedback[i];}
//    if(diagfeedback) delete diagfeedback;
    return internalState;
}

int ESN::s_classify(std::vector<std::vector<double> > outputSeq, int npoints){

    double *classes=new double[nOutputs];
    for(int i=0;i<nOutputs;i++){
        classes[i]=0;
    }
    //std::cout<<"output seq2:\n";
    for(int i=0;i<npoints;i++){
        for(int j=0;j<nOutputs;j++){
        //    std::cout<<outputSeq[i][j]<<" ";
            classes[j]=classes[j]+outputSeq[i][j];
        }
        //std::cout<<"\n";
    }
    double c_max=-100;
    int index=-100;
    for(int i=0;i<nOutputs;i++){

        if(classes[i]>c_max){
            c_max=classes[i];
            index=i;
        }

    }
//    for(int i=0;i<5;i++) std::cout<<classes[i]<<" ";
//    std::cout<<" index: "<<index<<"\n";
    if(classes) delete[] classes;
    return index;
}





