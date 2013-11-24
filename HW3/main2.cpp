#include "header.h"

int main(int argc, const char* argv[]){
	ifstream trainFile,testFile;
	string singleline;

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
	vector<int> index;
        for(int i = 0; i < traindata.getData().size(); i ++)
                index.push_back(i);

        vector<int> training_size;
        training_size.push_back(25);
        training_size.push_back(50);
        training_size.push_back(100);

        vector<vector<double> >test_NB_accuracy;
	vector<vector<double> >test_TB_accuracy;
        //For each size of training set, draw the instances randomly
        for(int i = 0; i < training_size.size(); i ++){
                int part_training_size = training_size[i];
                vector<double> part_test_NB_accuracy;
		vector<double> part_test_TB_accuracy;
		for(int k = 0; k < 4; k ++){
                        vector<vector<double> > part_training_data;
			random_shuffle(index.begin(),index.end());
			for(int j = 0; j < training_size[i] ; j ++){
				 part_training_data.push_back((traindata.getData())[index[j]]);
                        }
			Bayes* naiveBayes = new Bayes(Data(part_training_data,traindata.getAttr()));
	    		naiveBayes->naiveBayes();
	        	//naiveBayes->displayBayes();
        		int correct_NB_number = naiveBayes->testNaiveBayes(testdata.getData());
			part_test_NB_accuracy.push_back(correct_NB_number);
			Bayes* tan = new Bayes(Data(part_training_data,traindata.getAttr()));
              		tan->TANBayes();
                	int correct_TB_number = tan->testTANBayes(testdata.getData());
			part_test_TB_accuracy.push_back(correct_TB_number);
		}
        	test_NB_accuracy.push_back(part_test_NB_accuracy);
	        test_TB_accuracy.push_back(part_test_TB_accuracy);
        }

        //Print the accuracy of each pretiction
	cout << "Naive Bayes Classifier" << endl;
        for(int i = 0; i < test_NB_accuracy.size(); i ++){
                cout << "Training Size: " << training_size[i] << endl;
                for(int j = 0; j < test_NB_accuracy[i].size(); j++)
                        cout << test_NB_accuracy[i][j] << " ";
                cout << endl;
        }
	cout << "TAN Bayes Classifier" << endl;
        for(int i = 0; i < test_TB_accuracy.size(); i ++){
                cout << "Training Size: " << training_size[i] << endl;
                for(int j = 0; j < test_TB_accuracy[i].size(); j++)
                        cout << test_TB_accuracy[i][j] << " ";
                cout << endl;
        }

}
