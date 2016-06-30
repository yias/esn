/* esn node
 *
 * Iason Batzianoulis
 *
 *
 */




#include <iostream>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/JointState.h"

#include "ESN.h"
#include "classif.h"
//#include <sstream>
//#include <string>
#include <vector>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "MathLib/MathLib.h"

#define JOINT_CMD_TOPIC "allegroHand_1/joint_cmd"  //"/allegro/joint_cmd"        // define the name of the topic to communicate with the allegro node

#define Surface_Topic "bhand_ja"

#define FTips_Topic "ftips_topic"

#define DIM 16



static int cnt_msgs=0,trained_ESNs=0,class_r=0,samples=0,rows=0;     //
static double numbers[16][60000];
//static ESN* networks;                               //array of ESN networks, one for every time window
bool cl_flag=false;
std::vector<ESN> networks;
std::vector<int> vote_matrix;                            //array containing the votes of each time window
static double buffer[5000][16];

// define the message that will be publish to the allegro node. This message will contain the velocity and the position of the joints
sensor_msgs::JointState allegro_msgs;

// define a subscriber. the subscriber listens the string messages comming for the DAq board
ros::Subscriber sub;

// define a publisher for publishing to the topic "/allegro/joint_cmd" of the allegro node
ros::Publisher joint_state_pub;


//static double MVC[]={0.3111, 0.2920, 0.4033, 0.3329, 0.1910, 0.0863, 0.1643, 0.1726, 0.2590, 0.1304, 0.0861, 0.4524, 0.4273, 0.2849, 0.4396};
static double MVC[]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};


std::vector< std::vector<double> > emg2mm(std::string ll){

    std::vector< std::vector<double> > vtmp;

    int tmp=0;
    int counter=0;
    int cl=0;

    vtmp.push_back(std::vector<double>(16,0));

    for (int i=0;i<(int)ll.size();i++){
        if(ll[i]==' '){
            std::istringstream conv(ll.substr(tmp,i-1));
            conv>>vtmp[cl][counter];
            counter++;
            tmp=i+1;
        }
        if ((ll[i]=='\n')&&(i!=((int)ll.size()-1))){
            vtmp.push_back(std::vector<double>(16,0));
            counter=0;
            cl++;
            tmp=i+1;
        }
    }


    //std::cout<<"size of matrix: ("<<vtmp.size()<<","<<vtmp[0].size()<<")\n";

    return vtmp;
}



int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    return 1;
  }

  return 0;
}

// callback function that listens, stores and processes the string messages streamed from the DAq node. This funtions, also,
// calls the classification function of the class ESN that classifies the signals of the spesific time window to the corresponding
// grasp type


void chatterCallback2(const std_msgs::String::ConstPtr& msg){
     cnt_msgs++;
     std::string seq;//(msg.data);

     std::stringstream ss;
     ss<<msg->data;
     seq=ss.str();
     std::vector< std::vector<double> > datamatrix=emg2mm(seq);

     //std::cout<<"size of matrix: (";
     //std::cout<<datamatrix.size();//<<","<<datamatrix[0].size()<<")\n";

//     for (int i=0;i<(int)datamatrix.size();i++){
//         for (int j=0;j<(int)datamatrix[i].size();j++){
//             std::cout<<datamatrix[i][j]<<" ";
//         }
//         std::cout<<"\n";
//     }


     int nforget=10,ndatapoints=225;

     double **classif_data;

     classif_data=new double*[225];


     for(int i=0;i<225;i++){
         classif_data[i]=new double[DIM];
         for(int j=0;j<DIM;j++){
             //c_win[i][j]=dd[i+(sa-1)*100][j];
             classif_data[i][j]=datamatrix[i][j];
         }
     }



     int utr=networks[cnt_msgs-1].test_esn(classif_data,nforget,ndatapoints);


     vote_matrix.push_back(utr);
     for(int ki=0;ki<vote_matrix.size();ki++) std::cout<<vote_matrix[ki]<<" ";

     class_r=majority_vote(vote_matrix,class_r);

     std::cout<<"classification result" << utr <<"\n";
}



void chatterCallback(const std_msgs::String::ConstPtr& msg){

    cnt_msgs++;
    // receiving the message and put it in the numbers matrix
    std::string seq;//(msg.data);

    std::stringstream ss;
    ss<<msg->data;
    seq=ss.str();
    double **classif_data;


    //double **data=new double*[150];//[100][15];//=new double*[15];


//    char tmp[15]={0};
//    int j=0,k=0,r=0,ct=0,nforget=20,ndatapoints=150;
//    bool flag=false;

//    std::cout<<"msgs: "<<cnt_msgs<<" ";
//    //ROS_INFO("I started");

//    data[r]=new double[151];


//    for(int i=0;i<seq.size();i++){


//        if((seq[i]==' ')||(!isprint(seq[i]))){

//            //numbers[r][j]=atof(tmp);
//            data[r][j]=atof(tmp);

//            j++;
//            k=0;
//            ct++;

//            for(int h=0;h<15;h++) tmp[h]=NULL;

//            if(!isprint(seq[i])) flag=true;
//        }else{
//            //strcat(tmp,&test[i]);
//            tmp[k]=seq[i];
//            k++;
//        }
//        if(flag) {
//            //std::cout<<r<<" "<<j<<"\n";
//            i++;flag=false;r++;
//            if(i<seq.size()){
//                data[r]=new double[151];
//                j=0;
//            }
//        }//j=rows;}

//    }


//    for(int i=0;i<15;i++){
//        for(int h=0;h<150;h++){
//            buffer[samples+h][i]=data[i][h];
//        }
//    }
//    samples=samples+150;
//    std::cout<<"samples: "<<samples;



    int nforget=10,ndatapoints=150;

    char tmp[15]={0};
    int j=rows,k=0,r=0,ct=0;
    bool flag=false;



    for(int i=0;i<seq.size();i++){

        if((seq[i]==' ')||(!isprint(seq[i]))){

            numbers[r][j]=atof(tmp)/MVC[r];

            //std::cout<<numbers[r][j]*MVC[r]<<" ";

            j++;
            k=0;
            ct++;

            for(int h=0;h<15;h++) tmp[h]=NULL;

            if(!isprint(seq[i])) {flag=true;}
        }else{
            //strcat(tmp,&test[i]);
            tmp[k]=seq[i];
            k++;
        }
        if(flag) {i++;flag=false;r++;j=rows;}

    }




    rows+=ct/r;







    classif_data=sl_win(numbers,cnt_msgs);
    std::cout<<"\navtivity: "<<check_activation(classif_data,0.025,0)<<" ";
    //if(cnt_msgs>0){
    if(check_activation(classif_data,0.025,0)){
        if(cnt_msgs>7){
        if(cnt_msgs<20){

            int utr=networks[cnt_msgs-1].test_esn(classif_data,nforget,ndatapoints);
            std::cout<<" classif result: "<<utr<<"\n";
            vote_matrix.push_back(utr);
            for(int ki=0;ki<vote_matrix.size();ki++) std::cout<<vote_matrix[ki]<<" ";

            class_r=majority_vote(vote_matrix,class_r);

        }
    }
    }
    //ROS_INFO("I finished");

//    for(int i=0;i<150;i++){
//        //for(int cl=0;cl<100;cl++){

//            if(data[i]) delete[] data[i];
//        //}

//    }
//    if(data) delete[] data;//std::cout<<"ok delete\n";

    //if(classif_data) delete classif_data;




}

int main(int argc, char** argv){



    ros::init(argc,argv,"esn");                                                                 // initialize the node with name esn

    ros::NodeHandle n;                                                                          // define the node

    sub= n.subscribe("daq_pub",1000, chatterCallback2);

    joint_state_pub = n.advertise<sensor_msgs::JointState>(JOINT_CMD_TOPIC, 10);

    ros::Publisher bhand_pub=n.advertise<sensor_msgs::JointState>(Surface_Topic, 10);


    // define vectors for storing velocity and positions data
    std::vector<double> joint_velocity(16,0);
    //MathLib::Vector joint_velocity(16,0);




    std::vector<std::vector<double> > positions(6,std::vector<double>(16));


    int nb_nets=18;//count_esn();
    trained_ESNs=nb_nets;






    for (int i=0;i<nb_nets;i++){

        networks.push_back(ESN(i+1));

    }



    // setting the 6 predefined positions (5 for the classes + 1 the initial)


    //double pos0[]={-0.13545258083595238, -0.06772629041797619, 0.03133339517371638, 0.11326179105286713, 0.17885776565166714, -0.13882558088298133, 0.21063497662104522, 0.11627973846336673, 0.06346565877962382, -0.031155868855451695, 0.27694105649290396, 0.18018921303865224, 0.8270063536360213, 0.6893346938217604, 0.11042136996063222, 0.6846302463877462};
    //double pos0[]={-0.20619681866442818, -0.33783258365768987, -0.1599512127564785, -0.0710105273058728, 0.03497268469814236, -0.17033650237496242, 0.4372473218859118, -0.12284821223915995, -0.250400871912334, -0.1647444233496249, -0.057873579754286336, 0.09408894868028148, -0.3216776886956038, -0.6953705886427595, -0.34298084688736563, 0.044292816407038166};

    double pos0[]={0.0, 0.0, 0.0, -2.7478019859472618e-17, 0.0, 0.0, 0.0, -2.7478019859472618e-17, 0.0, 0.0, 0.0, -2.7478019859472618e-17, 0.821205, 0.0, 0.0, 0.0};

    positions[0].assign(pos0,pos0+16);




    //double pos1[]={-0.21107879241670693, 0.36455029455652455, 1.2880422021694002, 0.23468979274590965, -0.1566669758685819, 0.2911431619540785, 1.0361323565518166, 0.7356690628888423, -0.12000779114692506, 0.41283745312451803, 1.2725086493212407, 0.24551889816005523, 0.7708192739052494, 0.709306404626537, 0.6502789038035303, 0.7225321153372558};

    //double pos1[]={-0.0889406854506057, 0.3989904002998728, 0.4672492696726431, 1.4051208090649583, 0.054500579707257385, 0.2583895562342447, 1.3724559665042568, 0.6522316933044418, -0.2804915828581976, 0.408133005690504, 0.7914123268239525, 1.1355470947800386, -1.1146877523839385, -0.5297385337018112, -0.3247843992652357, 1.8943833422024219};
    double pos1[]={0.0, 0.6366369240000003, 0.7728741899999998, 0.7656254100000002, 0.0, 0.5352607260000002, 0.8123945939999999, 0.80434827, 0.0, 0.5865746039999999, 0.8123945939999999, 0.9586917449999999, 1.342557216, 0.03739903200000002, 0.068395536, 0.7969718789999998};

    positions[1].assign(pos1,pos1+16);

    //double pos2[]={0.0018640263417791611, 0.6730910357005419, 0.8407646433015341, 0.7409060892776504, -0.03985465845042112, 0.6022580347129337, 1.2690468861150797, 0.33366071517846985, 0.0755374484216222, -0.05618707973077186, 0.29522626727416623, 0.17495218664984413, 1.1052788575159103, 0.18933181842928337, 0.21391921350894183, 1.0931183047147794};
    //double pos2[]={-0.26717710898834646, 0.5760729027688931, 0.5975535872789197, 0.8703227752926036, 0.07296331680678432, 0.8090761954912883, 1.3036645181766926, 0.5355081390454134, -0.24276724022695267, -0.3977477160720201, 0.13252339658458515, 0.21826860830642655, -1.0893014888720889, -0.5153589019223719, 0.042251263746994325, 1.5219331264831188};

    double pos2[]={0.0, 0.788075442, 0.7987861530000002, 0.8428884750000004, 0.0, 0.750349908, 0.7728741900000002, 0.80434827, 0.0851499, 0.019976418000000017, -0.08035641899999998, 0.058202595000000044, 1.366224453, 0.11711185200000002, 0.272908845, 0.7969718789999998};

    positions[2].assign(pos2,pos2+16);

    //double pos3[]={0.05290284284287525, 1.4092039143850459, 0.3357022678385137, 0.6694517461761159, -0.030800816218922333, 1.2552885964495668, 0.7490722999178259, 0.7200467468815502, -0.15302768634415592, -0.12932792285582087, 0.13492000188115832, 0.2651355563283026, 1.225730464458497, -0.14424013359005414, -0.14086713354302519, 0.9900642769621316};
    //double pos3[]={-0.22696739790139595, 0.9140830127448477, 0.690311088572216, 0.8213255114515513, -0.011716737005469012, 1.0331144091413171, 1.3002027549705313, 0.7289230627947844, -0.18018921303865224, -0.37484682101587613, 0.16137142330259596, 0.08370365906179758, -1.1642175951797846, -0.466184111763055, -0.05316913232027227, 1.461751704591392};

    double pos3[]={0.0, 1.1154472560000002, 0.6548722290000002, 0.8428884750000004, 0.0, 1.25329743, 0.35287668900000013, 0.8686428300000004, 0.0851499, 0.019976418000000017, -0.08035641899999998, 0.058202595000000044, 1.366224453, 0.16129911600000005, 0.2600246880000001, 0.7969718789999998};

    positions[3].assign(pos3,pos3+16);

    //double pos4[]={0.010207763300219216, 1.1446897001706695, 0.7236860364059762, 0.6442430089825311, -0.06027018505085955, 1.2412640173066567, 0.7046907203516554, 0.8612689330611047, -0.15808718641469935, 1.4165712565930302, 0.5385260864559129, 0.3579818207807313, 1.1500154897186101, 0.20850466080186905, 0.1681174233966539, 0.9261548023868461};
    //double pos4[]={-0.2413470296808352, 1.0738566991830616, 0.3270922414026766, 0.5245902704721354, -0.02041552660043843, 1.401215230063135, 0.5870795345013035, 0.6401599036624434, -0.2595434773029651, 1.1201023050910113, 0.6722034041092184, 0.6310172982718123, -1.2417965962614508, -0.5147375598084455, 0.08201715903828309, 1.1961723324674274};

    double pos4[]={0.0, 1.1154472560000002, 0.6548722290000002, 0.8428884750000004, 5e-324, 1.2790437659999998, 0.35287668900000013, 0.9844461000000003, 5e-324, 1.1783827440000003, 0.549546624, 0.7142993549999999, 1.366224453, 0.16129911600000005, 0.2600246880000001, 0.7969718789999998};

    positions[4].assign(pos4,pos4+16);


    //double pos5[]={-0.1331447386985115, -0.0627555535065651, 0.035594026812068746, 0.10944497521017647, 0.1765499235142263, -0.1365177387455405, 0.21844613462469123, 0.11663479109989609, 0.4786997172007151, 1.1044799890837194, 1.0907216994182065, 0.18560376574572507, 0.9202964338841115, 0.5715459816531439, 0.49201419107056626, 1.0455412514198448};
    double pos5[]={-0.026185131944040596, -0.19740926591032643, -0.47275258553884825, -0.03878950054083302, 0.19749802906945874, -0.10545063304922113, 0.18684644997357783, 0.030090710945863602, -0.6759314567927768, 0.8175086956088607, 0.8309119326378442, 1.1716737005469013, -1.0514883830817117, -0.7566171684440748, 0.6349228772736352, 1.063293883246313};
    positions[5].assign(pos5,pos5+16);



    std::vector<std::vector<double> > bhand_joinAngles(6,std::vector<double>(4));

    double bja0[]={0.0,0.0,0.0,0.0};
    bhand_joinAngles[0].assign(bja0,bja0+4);

    double bja1[]={3.9119,1.5874,2.9227,-0.4453};
    bhand_joinAngles[1].assign(bja1,bja1+4);

    double bja2[]={3.2216,1.2050,2.8998,-0.0715};
    bhand_joinAngles[2].assign(bja2,bja2+4);

    double bja3[]={1.7821,1.0097,1.4966,-0.0364};
    bhand_joinAngles[3].assign(bja3,bja3+4);

    double bja4[]={1.6408,1.0488,1.6260,0.1500};
    bhand_joinAngles[4].assign(bja4,bja4+4);

    double bja5[]={0.0,0.0,0.0,0.0};
    bhand_joinAngles[5].assign(bja5,bja5+4);

    std::vector<double> bhand_joint_velocity(4,0);


    sensor_msgs::JointState bhand_msgs;

    bhand_msgs.velocity.resize(4);
    bhand_msgs.position.resize(4);

    bhand_msgs.velocity=bhand_joint_velocity;


    // resize the vectors of position and velocity according to the numbers of DOF of the allegro hand
    allegro_msgs.velocity.resize(16);

    allegro_msgs.position.resize(16);




    // setting the velocity of the joint to maximum
    allegro_msgs.velocity=joint_velocity;

    std::cout<<"ok\n";


    ros::Rate loop_rate(1);                                                // setting the sleeing time to 10 msec

    while(ros::ok()){

        //std::cout<<"majority class: "<<class_r<<"\n";
        allegro_msgs.position=positions[class_r];                           // setting the joint position of the allegro hand

        bhand_msgs.position=bhand_joinAngles[class_r];

        joint_state_pub.publish(allegro_msgs);                              // publish the message

        bhand_pub.publish(bhand_msgs);



        // checking if any key is hitten for resetting the prosedure
        if(kbhit()){
            printf("reset\n");
            fflush(stdin);                                                  // flushing the stream from the console
            cnt_msgs=0;                                                     // reset number of messages received (indicates a new trial)
            class_r=0;
            vote_matrix.clear();
            for(int k=0;k<vote_matrix.size();k++){
                std::cout<<vote_matrix[k]<<" ";
            }
            std::cout<<"\n";
        }

        // checking if ctlr+C is pressed
        //if(ros::isShuttingDown()) break;

        ros::spinOnce();                                                    // spin the machine

        loop_rate.sleep();                                                  // sleep for 10 msec

    }


    //delete networks[0];
    std::cout<<"\nbye!\n";
    return 0;
}

