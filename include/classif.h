//#include "ESN.h"(
#include <iostream>

#include <string>
#include <sstream>
#include <vector>
//#include <cmath>
#include <sys/stat.h>


bool file_exist (const std::string& name) {

    struct stat buffer;

    return (stat (name.c_str(), &buffer) == 0);
}


int count_esn(){
    int esn_nets=0;
    std::string base("src/esn/ESN_network/ESN");

    std::ostringstream convert;
    convert<<base<<esn_nets+1<<".txt";
    std::string filename=convert.str();


    while(file_exist(filename)){

        esn_nets++;
        convert.str("");
        convert.clear();
        convert<<base<<esn_nets+1<<".txt";
        filename=convert.str();


    }
    return esn_nets;
}


int mv_algorithm(std::vector<int> v,int cl){

    int nb_votes=v.size();
    //for(int i=0;i<nb_votes;i++) std::cout<<v[i]<<" ";
    //std::cout<<"\n";
    /*
    int win=v[0],buffer=1;
    int i=1;
    while(i<nb_votes){
        if(win==v[i]) buffer++;
        else{
            buffer--;
            if(buffer==0) win=v[i];buffer=1;
        }
        i++;
    }
    if(win>0){
        if(buffer/v.size()>1/3){
            return win;
        }else return cl;
    }else{
        return 0;
    }
    */

    std::vector<int> classes(5,0);

    std::vector<int>::iterator result;
    for(int j=0;j<nb_votes;j++){
        for(int k=1;k<6;k++){
            if (v[j]==k){
                classes[k-1]++;
            }
        }
    }


    result=std::max_element(classes.begin(),classes.end());
    std::cout<<" majority vote: "<<std::distance(classes.begin(),result)+1<<"\n";

    return std::distance(classes.begin(),result)+1;



}


int majority_vote(std::vector<int> votes,int cl){
    //std::cout<<"in maj vote";

    int sz_array=votes.size();

    if(sz_array<1) return 0;
    else{
        if(sz_array<2){
            return votes[0];
        }
        else{
            return mv_algorithm(votes,cl);
        }
    }

    //std::cout<<"out maj vote";

    //return 0;
}


double** sl_win(double dd[15][60000],int sa){



    //std::cout<<"in sl_win\n";
    double **c_win=new double*[150];


    for(int i=0;i<150;i++){
        c_win[i]=new double[15];
        for(int j=0;j<15;j++){
            //c_win[i][j]=dd[i+(sa-1)*100][j];
            c_win[i][j]=dd[j][i+(sa-1)*100];
        }
    }



    return c_win;


}

double average_value(std::vector<double> signal){
    int size_signal=signal.size();
    double sum=0;
    for(int i=0;i<size_signal;i++){
        sum=sum+signal[i];
    }
    return sum/size_signal;
}

int check_activation(double **tw, double threshold, int muscle){
    std::vector<double> signal;
    for(int i=0;i<150;i++){
        signal.push_back(tw[muscle][i]);
    }
    if(average_value(signal)>=threshold){
        return 1;
    }else{
        return 0;
    }
}

