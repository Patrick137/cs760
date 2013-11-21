#include "header.h"

Bayes::Bayes(Data data){
	dataset = data.getData();
	attr_table = data.getAttr();
	prob_y = new double[2];
	int* sum = new int[2];
	sum[0] = 0; sum[1] = 0;
	for(int i = 0; i < dataset.size(); i ++){
		sum[(int)dataset[i][attr_table.size()-1]]++;
	}
	prob_y[0] = (double)sum[0]/(sum[0] + sum[1]);
	prob_y[1] = (double)sum[1]/(sum[0] + sum[1]);
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
	cout << "Add vertex: " << name << endl;
}

vertex* Bayes::getVertex(const string &name){
	return (network.find(name)->second);
}

void Bayes::addEdge(const string& from, const string& to, vector<vector<double> > cost){
	vertex *f = (network.find(from)->second);
	vertex *t = (network.find(to)->second);
	pair<vector<vector<double> >,vertex *> edge = make_pair(cost,t);
	f->adj.push_back(edge);
	return;
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
		sum[instanceClass] ++;
	}
	for(int i = 0; i < attr_table[indx].value.size(); i ++){
		cost[0][i] = (double)(count[0][i]+1)/sum[0];
		cost[1][i] = (double)(count[1][i]+1)/sum[1];
	}

	cout << "Attribute " << indx << " " << cost[0].size() <<  endl;
	for(int i = 0; i < attr_table[indx].value.size(); i ++){
		cout << cost[0][i] << " ";
	}	
	cout << endl;
	for(int i = 0; i < attr_table[indx].value.size(); i ++){
		cout << cost[1][i] << " ";
	}	
	cout << endl;

	return cost;
}

void Bayes::naiveBayes(){
	string classname = attr_table[attr_table.size()-1].name;
	addVertex(classname);
	root = getVertex(classname);
	for(int i = 0; i < attr_table.size(); i ++){
		addVertex(attr_table[i].name);
		vector<vector<double> > cost = calculateCostForNaive(i);
		addEdge(attr_table[i].name,classname,cost);
	}
}

void Bayes::displayBayes(){
	for(int i = 0; i < attr_table.size()-1; i ++){
		cout << attr_table[i].name;
		vertex* p = getVertex(attr_table[i].name);
		for(int j = 0; j < p->adj.size(); j ++){
			cout << " " + (p->adj)[j].second->name;
		}
		cout << endl;
	}
}

void Bayes::testNaiveBayes(vector<vector<double> > testdata){
	int correct_num = 0;
	for(int i =0; i < testdata.size(); i ++){
		vector<double> instance = testdata[i];
		double* multi_y = new double[2];
		multi_y[0] = 1; multi_y[1] = 1;
		for(int j = 0; j < attr_table.size(); j ++){
			vertex* p = getVertex(attr_table[j].name);
			vector<vector<double> > cost = (p->adj)[0].first;
			multi_y[0] *= cost[0][instance[j]];
			multi_y[1] *= cost[1][instance[j]];
		}

		double sum_multi = prob_y[0]*multi_y[0]+prob_y[1]*multi_y[1];

		double* final_prob = new double[2];
		final_prob[0] = prob_y[0]*multi_y[0]/sum_multi;
		final_prob[1] = prob_y[1]*multi_y[1]/sum_multi;
		int predict_class = final_prob[0] > final_prob[1]? 0 : 1;
		int actual_class = instance[attr_table.size()-1];
		cout << final_prob[0] << " " << final_prob[1] << endl;
		if(predict_class == actual_class)
			correct_num ++;
		cout << attr_table[attr_table.size()-1].value[predict_class] << " " <<
			attr_table[attr_table.size()-1].value[actual_class] << " " <<
			final_prob[predict_class] << endl;
	}
	cout << correct_num << endl;
}