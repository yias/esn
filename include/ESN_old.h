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
#include <new>

//using namespace std;

class ESN
{
int nforgetpoints;
int nInternalUnits;
int nInputs;
int nOutputs;
float **internalWeightsUnitSR;
int nTotalUnits;

float **inputWeights;
float **outputWeights;
float **feedbackWeights;
float *inputScaling;
float *inputShift;
float *teacherScaling;
float *teacherShift;

float *feedbackScaling;
float leakage;
float *timeConstants;
float **internalWeights;

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
	double** compute_statematrix(double **EMGsignal, int nforgetpoints, int nDatapoints);
	double *plain_esn(double *tState);
    int s_classify(double **outputSeq, int npoints);

	~ESN(void);

};


ESN::ESN(void)
{
    nInternalUnits=1;
    internalWeightsUnitSR=new(std::nothrow) float;
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

    if(internalWeightsUnitSR) delete[] internalWeightsUnitSR;


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

    std::cout << "\nesn deleted\n";
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

	}
    else std::cout<<"Unable to open file"<<"\n";


}

int ESN::setinterntalWUSR(std::ifstream& f, int k, int l){
    //std::cout<<k<<"\n";
    //std::cout<<l<<"\n";
	
    //float **tmp= new float*[k];
    internalWeightsUnitSR=new float*[k];

	for(int i=0;i<k;i++){
        //tmp[i]= new float[l];
        internalWeightsUnitSR[i]=new float[l];
		for(int j=0;j<l;j++){
			//float temp;
            //f>>tmp[i][j];
            f>>internalWeightsUnitSR[i][j];
			//cout<<tmp[i][j]<<" ";
			//cout<<tmp[i][j]<<endl;
			//inputWeights[i][j]=temp;
		}
	}

    //std::cout<<"\ninternal weights\n";
    //internalWeightsUnitSR=tmp;

    //std::cout<<tmp[50][50]<<" "<<internalWeightsUnitSR[50][50]<<"\n";

    //std::cout<<internalWeightsUnitSR[99][99]<<" "<<internalWeightsUnitSR[99][99]<<"\n";
	

	
    //std::cout<<"\ninternal units:\n";
    //std::cout << internalWeightsUnitSR[0][1]<<" ";
    //std::cout << internalWeightsUnitSR[1][0] << " \n";

    //float result=internalWeightsUnitSR[0][1]+internalWeightsUnitSR[1][0];
    //std::cout<<"size3: "<<sizeof(internalWeightsUnitSR)<<"\n";
    //std::cout<<result<<"\n";
	return 0;
}


int ESN::setinputWeights(std::ifstream& f, int k, int l){
    //float **tmp= new float*[k];
    inputWeights= new float*[k];
	for(int i=0;i<k;i++){
        inputWeights[i]=new float[l];
        //tmp[i]= new float[l];
		for(int j=0;j<l;j++){
            //f>>tmp[i][j];
            f>>inputWeights[i][j];
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
    //float **tmp= new float*[k];
    outputWeights=new float*[k];
	for(int i=0;i<k;i++){
        //tmp[i]= new float[l];
        outputWeights[i]=new float[l];
		for(int j=0;j<l;j++){
            //f>>tmp[i][j];
            f>>outputWeights[i][j];
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
    //float **tmp= new float*[k];
    feedbackWeights=new float*[k];
	for(int i=0;i<k;i++){
        //tmp[i]= new float[l];
        feedbackWeights[i]=new float[l];
		for(int j=0;j<l;j++){
        //	f>>tmp[i][j];
            f>>feedbackWeights[i][j];
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
    //float *tmp= new float[k];
    inputScaling=new float[k];

    for(int i=0;i<k;i++) f>>inputScaling[i];//f>>tmp[i];
    //inputScaling=tmp;
	//delete tmp;
    //for(int i=0;i<k;i++) std::cout<<inputScaling[i]<<" ";
    //std::cout<<"\n";
	return 0;
}

int ESN::setinputShift(std::ifstream& f, int k){

    //float *tmp= new float[k];
    inputShift=new float[k];
    for(int i=0;i<k;i++) f>>inputShift[i];//f>>tmp[i];
    //inputShift=tmp;
	//delete tmp;
	return 0;
}

int ESN::setteacherScaling(std::ifstream& f, int k){

    //float *tmp= new float[k];
    teacherScaling=new float[k];
    for(int i=0;i<k;i++) f>>teacherScaling[i];// f>>tmp[i];
    //teacherScaling=tmp;
	//delete tmp;
	return 0;
}

int ESN::setteacherShift(std::ifstream& f, int k){
    //float *tmp= new float[k];
    teacherShift=new float[k];
    for(int i=0;i<k;i++) f>>teacherShift[i];//f>>tmp[i];
    //teacherShift=tmp;
	//delete tmp;
	return 0;
}

int ESN::setfeedbackScaling(std::ifstream& f, int k){
    //float *tmp= new float[k];
    feedbackScaling=new float[k];
    for(int i=0;i<k;i++) f>>feedbackScaling[i];//f>>tmp[i];
    //feedbackScaling=tmp;
	//delete tmp;
	return 0;
}

int ESN::settimeConstants(std::ifstream& f, int k){
    //float *tmp= new float[k];
    timeConstants=new float[k];
    for(int i=0;i<k;i++) f>>timeConstants[i];// f>>tmp[i];
    //timeConstants=tmp;
	//delete tmp;
	return 0;
}

int ESN::setinternalWeights(std::ifstream& f, int k, int l){
    //float **tmp= new float*[k];
    internalWeights=new float*[k];
	for(int i=0;i<k;i++){
        //tmp[i]= new float[l];
        internalWeights[i]=new float[l];
		for(int j=0;j<l;j++){
            f>>internalWeights[i][j];//f>>tmp[i][j];
		}
	}
    //internalWeights=tmp;
	/*for(int i=0;i<k;i++){
		delete tmp[i];
	}
	delete tmp;*/
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

int ESN::test_esn(double **EMGsignal, int nforgetpoints, int nDatapoints){
    std::cout<<"test in\n";
	double **stateCollection=compute_statematrix(EMGsignal,nforgetpoints,nDatapoints);
    //std::cout<<"state matrix: \n";
//    for(int i=0;i<nDatapoints-nforgetpoints;i++){
//        for(int j=0;j<nInternalUnits+nInputs;j++){
//            std::cout<<stateCollection[i][j]<<" ";
//        }
//        std::cout<<"\n";
//    }
    //std::cout<<"ok test2\n";///////////////////////////////////////////////////////////////////////////////////////////////////
	double **outputSequence=new double*[nDatapoints-nforgetpoints];
	for(int i=0;i<nDatapoints-nforgetpoints;i++){
		outputSequence[i]=new double[nOutputs];
		
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
	double **repmat=new double*[nDatapoints-nforgetpoints];
	for(int i=0;i<(nDatapoints-nforgetpoints);i++){
		repmat[i]=new double[nOutputs];
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
	double **invDiagTeacherScalling=new double*[nOutputs];
	for(int i=0;i<nOutputs;i++){
		invDiagTeacherScalling[i]=new double[nOutputs];
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
	for(int i=0;i<(nDatapoints-nforgetpoints);i++){
        delete[] stateCollection[i];
        delete[] outputSequence[i];
        delete[] repmat[i];
	}
    if(stateCollection) delete[] stateCollection;
    if (outputSequence)delete[] outputSequence;
    if(repmat) delete[] repmat;

	for(int i=0;i<nOutputs;i++){
        if(invDiagTeacherScalling[i]) delete invDiagTeacherScalling[i];
	}
    if(invDiagTeacherScalling) delete invDiagTeacherScalling;

    std::cout<<"test out\n";
	return classification_result;
}

double** ESN::compute_statematrix(double **EMGsignal, int nforgetpoints, int nDatapoints){
    //std::cout<<"ok csm1\n";///////////////////////////////////////////////////////////////////////////////////////
	double *totalstate= new double[nInternalUnits+nInputs+nOutputs];
	for(int i=0;i<nInternalUnits+nInputs+nOutputs;i++) totalstate[i]=0;

	double **stateCollectMat= new double*[nDatapoints-nforgetpoints];
	double *netOut = new double[nOutputs];
	double *in = new double[nInputs];
    double *internalState;// = new double[nInternalUnits];
	double *tin = new double[nInternalUnits + nOutputs];
	int collectdata=0;
    //std::cout<<"ok csm2\n";
	for(int i=0;i<nDatapoints-1;i++){
        double *tmp=new double[nInternalUnits+nInputs];
		for(int j=0;j<nInternalUnits+nInputs;j++) tmp[j]=0;
		
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
        //std::cout<<"ok csm2.4 \n";////////////////////////////////////////////////////////////////////////////////
		internalState=plain_esn(totalstate);
        //std::cout<<"ok csm2.5 ";//////////////////////////////////////////////////////////////////////////////////
		
		
		for(int j=0;j<nInternalUnits+nInputs;j++){
			if(j<nInternalUnits) tin[j]=internalState[j];
			else tin[j]=in[j-nInternalUnits];
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

		

		for(int j=0;j<nInternalUnits+nInputs;j++) tmp[j]=totalstate[j];
		if(i>=nforgetpoints-1){
			stateCollectMat[collectdata]=tmp;
			collectdata=collectdata+1;
		}
	}

//    delete netOut;
//    delete in;
//    delete internalState;
//    delete tin;

    //std::cout<<"ok csm3\n";
	return stateCollectMat;
}

double* ESN::plain_esn(double *tState){
	double **tmp=new double*[nInternalUnits];
	double **diagfeedback=new double*[nOutputs];
	for(int i=0;i<nOutputs;i++){
		diagfeedback[i]=new double[nOutputs];
		for(int j=0;j<nOutputs;j++){
			diagfeedback[i][j]=0;
			if(j==i) diagfeedback[i][j]=feedbackScaling[i];
		}
	}

	
	double **feedS=new double*[nInternalUnits];
	for(int i=0;i<nInternalUnits;i++){
		feedS[i]=new double[nOutputs];
		for(int j=0;j<nOutputs;j++){
			float tmp2=0;
			for(int k=0;k<nOutputs;k++){
				tmp2=tmp2+feedbackWeights[i][k]*diagfeedback[k][j];
			}
			feedS[i][j]=tmp2;
		}
	}

	
	for(int i=0;i<nInternalUnits;i++){
		tmp[i]=new double[nInternalUnits+nInputs+nOutputs];
		for(int j=0;j<nInternalUnits+nInputs+nOutputs;j++){
			if(j<nInternalUnits) tmp[i][j]=internalWeights[i][j];
			if((j>=nInternalUnits)&&(j<nInternalUnits+nInputs)) tmp[i][j]=inputWeights[i][j-nInternalUnits];
			if(j>=nInternalUnits+nInputs) tmp[i][j]=feedS[i][j-(nInternalUnits+nInputs)];
		}
	}

	
	double *internalState=new double[nInternalUnits];
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

int ESN::s_classify(std::vector<std::vector<double> > **outputSeq, int npoints){

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
    //for(int i=0;i<5;i++) std::cout<<classes[i]<<" ";
    //std::cout<<"\n";
    if(classes) delete[] classes;
	return index;
}
