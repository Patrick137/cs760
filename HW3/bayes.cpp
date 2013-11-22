#include "header.h"

void displayVector(vector<vector<double> > data){
        for(int i = 0; i < data.size(); i ++){
                cout << "Line " << i << endl;
                for(int j = 0; j < data[0].size() ; j ++)
                        cout << data[i][j] << " ";
                cout << endl;
        }
}

void displayVector(vector<double> data){
        for(int i = 0; i < data.size(); i ++)
                cout << data[i] << " ";
        cout << endl;
}

Bayes::Bayes(Data data){
	dataset = data.getData();
	attr_table = data.getAttr();
	prob_y = new double[2];
	int* sum = new int[2];
	sum[0] = 0; sum[1] = 0;
	for(int i = 0; i < dataset.size(); i ++){
		sum[(int)dataset[i][attr_table.size()-1]]++;
	}
	prob_y[0] = (double)(sum[0]+1)/(sum[0] + sum[1]+2);
	prob_y[1] = (double)(sum[1]+1)/(sum[0] + sum[1]+2);


	vector<long double> mutual_dim1(attr_table.size(),0);
	vector<vector<long double> > mutual_info_temp (attr_table.size(),mutual_dim1);
	mutual_info = mutual_info_temp;
}

void Bayes::addVertex(const string &name){
	vmap::iterator itr = network.begin();
	itr = network.find(name);
	if(itr == network.end()){
		vertex *v;
		v = new vertex(name);
		network[name] = v;
		return;
	}
}

vertex* Bayes::getVertex(const string &name){
	return (network.find(name)->second);
}

void Bayes::addEdge(const string& from, const string& to, vector<vector<double> > cost){
	vertex *f = (network.find(from)->second);
	vertex *t = (network.find(to)->second);
	pair<vector<vector<double> >,vertex *> edge = make_pair(cost,t);
	f->adj.push_back(edge);
        t->parent.push_back(f);
	return;
}

int*** allocThreeDimension(int a, int b, int c){
        int ***mutual_table = (int***)malloc(a*sizeof(int**));
        for(int i = 0; i < a; i ++){
                mutual_table[i] = (int**)malloc(b*sizeof(int*));
                for(int j = 0; j < b; j++){
                        mutual_table[i][j] = (int*)malloc(c*sizeof(int));
                        for(int k = 0; k < c; k++)
                                mutual_table[i][j][k] = 0;
                }
        }
        return mutual_table;
}

int get2DFrom3D(int*** table,int indx1, int indx3, int length){
        int sum = 0;
        for(int i = 0; i < length; i ++){
                sum += table[indx1][i][indx3];
        }
        return sum;
}
int get3DFrom3D(int*** table,int indx1, int indx2, int length){
        int sum = 0;
        for(int i = 0; i < length; i ++){
                sum += table[indx1][indx2][i];
        }
        return sum;
}

void Bayes::printMutualInfo(){
        for(int i = 0; i < mutual_info.size(); i ++){
                cout << "Line " << i << endl;
                for(int j = 0; j < mutual_info[0].size() ; j ++)
                        cout << mutual_info[i][j] << " ";
                cout << endl;
        }
}
void Bayes::calculateMutualInfo(){
        int classindx = attr_table.size()-1;
        for(int i = 0; i < attr_table.size(); i ++){
                for(int j = i;j < attr_table.size(); j ++){
                        int ***mutual_table = allocThreeDimension(2,attr_table[i].value.size(),
                                        attr_table[j].value.size());
                        int count_class[2];
                        count_class[0] = 0;
                        count_class[1] = 0;
                        for(int k = 0; k < dataset.size(); k ++){
                                vector<double> instance = dataset[k];
                                mutual_table[(int)instance[classindx]][(int)instance[i]][(int)instance[j]]++;
                                count_class[(int)instance[classindx]]++;
                        }

                        long double mutual_ij = 0;
                        for(int r = 0; r < attr_table[i].value.size(); r ++){
                                for(int s = 0; s < attr_table[j].value.size(); s++){
                                        for(int t = 0; t < 2; t++){
                                                if(i == 0 && j == 0)
                                                cout << "Value : " << r << " " << s << " " << t << endl;
                                                int ijy = mutual_table[t][r][s];
                                                int ijy_total = dataset.size();
                                                long double pijy =(long double)(ijy+1)/
                                                        (ijy_total+2*attr_table[i].value.size()*attr_table[j].value.size());
                                                if(i == 0 && j ==0)
                                                        cout << "pijy " << pijy << endl;
                                                int ij_y = mutual_table[t][r][s];
                                                int ij_y_total = count_class[t];
                                                long double pij_y = (long double)(ij_y + 1)/
                                                        (ij_y_total+ attr_table[i].value.size()*attr_table[j].value.size());
                                                if(i == 0 && j ==0)
                                                        cout << "pij_y "<< ij_y << " " << ij_y_total << " " << pij_y << endl;
                                                int i_y = get3DFrom3D(mutual_table,t,r,attr_table[j].value.size());
                                                int i_y_total = count_class[t];
                                                long double pi_y = (long double)(i_y + 1)/
                                                        (i_y_total + attr_table[i].value.size());
                                                if(i == 0 && j ==0)
                                                        cout << "pi_y " << pi_y << endl;

                                                int j_y = get2DFrom3D(mutual_table,t,s,attr_table[i].value.size());
                                                int j_y_total = count_class[t];
                                                long double pj_y = (long double)(j_y + 1)/
                                                        (j_y_total + attr_table[j].value.size());
                                                if(i == 0 && j ==0)
                                                        cout << "pj_y " << pj_y << endl;

                                                mutual_ij += pijy*log(pij_y/(pi_y*pj_y));
                                        }
                                }
                        }
                        mutual_info[i][j] = mutual_ij;
                        mutual_info[j][i] = mutual_ij;
                }
        }
}

vector<vector<double> > Bayes::calculateCostForNaive(int indx){
	int classind = attr_table.size()-1;

	vector<double> cost_dim1(attr_table[indx].value.size(),0);
	vector<vector<double> > cost(2,cost_dim1);

	vector<int> count_dim1(attr_table[indx].value.size(),0);
	vector<vector<int> > count(2,count_dim1);

	int* sum = new int[2];
	sum[0] = attr_table[indx].value.size(); sum[1] = sum[0];
	for(int i = 0; i < dataset.size(); i ++){
		int instanceClass = dataset[i][classind];
		count[instanceClass][dataset[i][indx]]++;
		sum[instanceClass]++;
	}
	for(int i = 0; i < attr_table[indx].value.size(); i ++){
		cost[0][i] = (double)(count[0][i]+1)/sum[0];
		cost[1][i] = (double)(count[1][i]+1)/sum[1];
	}
	return cost;
}

void Bayes::naiveBayes(){
	string classname = attr_table[attr_table.size()-1].name;
	addVertex(classname);
	root = getVertex(classname);
	for(int i = 0; i < attr_table.size()-1; i ++){
		addVertex(attr_table[i].name);
		vector<vector<double> > cost = calculateCostForNaive(i);
		addEdge(classname,attr_table[i].name,cost);
	}
}

void Bayes::displayBayes(){
	for(int i = 0; i < attr_table.size()-1; i ++){
		cout << attr_table[i].name;
		vertex* p = getVertex(attr_table[i].name);
		for(int j = 0; j < p->parent.size(); j ++){
			cout << " " + (p->parent)[j]->name;
		}
		cout << endl;
	}
        cout << endl;
}

void Bayes::testNaiveBayes(vector<vector<double> > testdata){
	int correct_num = 0;
        cout << setprecision(16);
	for(int i =0; i < testdata.size(); i ++){
		vector<double> instance = testdata[i];
		long double* multi_y = new long double[2];
		multi_y[0] = 1; multi_y[1] = 1;
		//for(int j = 0; j < attr_table.size()-1; j ++){
                for(int j = 0; j < root->adj.size(); j ++){
			//vertex* p = getVertex(attr_table[j].name);
			//vector<vector<double> > cost = (p->adj)[0].first;
			vector<vector<double> > cost = (root->adj)[j].first;
			multi_y[0] *= cost[0][instance[j]];
			multi_y[1] *= cost[1][instance[j]];
		}
		long double sum_multi = prob_y[0]*multi_y[0]+prob_y[1]*multi_y[1];

		long double* final_prob = new long double[2];
		final_prob[0] = (prob_y[0]*multi_y[0])/sum_multi;
		final_prob[1] = (prob_y[1]*multi_y[1])/sum_multi;
		int predict_class = final_prob[0] > final_prob[1]? 0 : 1;
		int actual_class = instance[attr_table.size()-1];
		if(predict_class == actual_class)
			correct_num ++;
		cout << attr_table[attr_table.size()-1].value[predict_class] << " " <<
			attr_table[attr_table.size()-1].value[actual_class] << " " <<
			final_prob[predict_class] << endl;
	}
	cout << endl << correct_num << endl;
}
