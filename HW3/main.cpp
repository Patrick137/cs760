#include "header.h"

int main(int argc, const char* argv[]){
	ifstream trainFile,testFile;
	string singleline;
        if(argc != 4){
                cerr << "Invalid argument" << endl;
                return -1;
        }
        char type = *argv[3];

	trainFile.open(argv[1]);
	testFile.open(argv[2]);
	//If it cannot open the training set or the testing set, report an error and exit
	if(!trainFile){
		cerr<< "Error: Cannot open the training set!"<<endl;
		exit(-1);
	}
	if(!testFile){
		cerr<< "Error: Cannot open the test set!"<<endl;
		exit(-1);
	}

	Data traindata;
	Data testdata;
        //Read the attribute and instances
	while(getline(trainFile,singleline)){
                if(singleline[0] == '%')
                        continue;
		if(singleline[0] == '@'){
			traindata.parse_header(singleline);
		}else if(singleline.size() != 0)
			traindata.parse_data(singleline);
	}
	//Read the testing set*/

	while(getline(testFile,singleline)){
                if(singleline[0] == '%')
                        continue;
		if(singleline[0] == '@'){
			testdata.parse_header(singleline);
		}else if(singleline.size() != 0)
			testdata.parse_data(singleline);
	}
	//cout << "Load Data Completed" << endl;
        //traindata.printData();
        if(type == 'n'){
        	Bayes* naiveBayes = new Bayes(traindata);
	        naiveBayes->naiveBayes();
        	naiveBayes->displayBayes();

                Bayes* tan = new Bayes(traindata);
                tan->TANBayes();
                /*tan->calculateMutualInfo();
                tan->printMutualInfo();
                tan->Prim();*/
	        //cout << "Begin Test" << endl;
        	naiveBayes->testNaiveBayes(testdata.getData());
        }else if(type == 't'){
                Bayes* tan = new Bayes(traindata);
                tan->TANBayes();
        	tan->displayBayes();
	        //cout << "Begin Test" << endl;
                tan->testTANBayes(testdata.getData());
        }else
                cerr<<"Invalid argument" << endl;

}
