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


	vector<long double> mutual_dim1(attr_table.size()-1,0);
	vector<vector<long double> > mutual_info_temp (attr_table.size()-1,mutual_dim1);
	mutual_info = mutual_info_temp;
}

int Bayes::getAttrIndx(string name){
        for(int i = 0; i < attr_table.size(); i ++){
                if(name==attr_table[i].name)
                        return i;
        }
        return -1;
}
void Bayes::addVertex(const string &name){
	vmap::iterator itr = network.begin();
	itr = network.find(name);
	if(itr == network.end()){
		vertex *v;
		v = new vertex(name);
                v->indx = getAttrIndx(name);
                assert(v->indx != -1);
		network[name] = v;
		return;
	}
}

vertex* Bayes::getVertex(const string &name){
	return (network.find(name)->second);
}

void Bayes::addEdge(const string& from, const string& to){
	vertex *f = (network.find(from)->second);
	vertex *t = (network.find(to)->second);
	f->adj.push_back(t);
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
        cout << setprecision(12);
        for(int i = 0; i < mutual_info.size(); i ++){
                cout << "Line " << i << endl;
                for(int j = 0; j < mutual_info[0].size() ; j ++)
                        cout << mutual_info[i][j] << " ";
                cout << endl;
        }
}
void Bayes::calculateMutualInfo(){
        int classindx = attr_table.size()-1;
        for(int i = 0; i < attr_table.size()-1; i ++){
                for(int j = i;j < attr_table.size()-1; j ++){
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
                        if(i == j){
                                mutual_info[i][j] = -1;
                                continue;
                        }
                        long double mutual_ij = 0;
                        for(int r = 0; r < attr_table[i].value.size(); r ++){
                                for(int s = 0; s < attr_table[j].value.size(); s++){
                                        for(int t = 0; t < 2; t++){
                                                int ijy = mutual_table[t][r][s];
                                                int ijy_total = dataset.size();
                                                long double pijy =(long double)(ijy+1)/
                                                       (ijy_total+2*attr_table[i].value.size()*attr_table[j].value.size());
                                                int ij_y = mutual_table[t][r][s];
                                                int ij_y_total = count_class[t];
                                                long double pij_y = (long double)(ij_y + 1)/
                                                       (ij_y_total+ attr_table[i].value.size()*attr_table[j].value.size());
                                                int i_y = get3DFrom3D(mutual_table,t,r,attr_table[j].value.size());
                                                int i_y_total = count_class[t];
                                                long double pi_y = (long double)(i_y + 1)/
                                                        (i_y_total + attr_table[i].value.size());
                                                int j_y = get2DFrom3D(mutual_table,t,s,attr_table[i].value.size());
                                                int j_y_total = count_class[t];
                                                long double pj_y = (long double)(j_y + 1)/
                                                        (j_y_total + attr_table[j].value.size());
                                                mutual_ij += pijy*log(pij_y/(pi_y*pj_y));
                                        }
                                }
                        }
                        mutual_info[i][j] = mutual_ij;
                        mutual_info[j][i] = mutual_ij;
                }
        }
}

bool checkVisitedOrNot(primNode node, bool* visited){
        return (visited[node.from] && !visited[node.to]) || (visited[node.to] && !visited[node.from]);
}

vector<int> getEdgeTo(int from, vector<pair<int,int> >& edges){
        vector<int> tos;
        vector<pair<int,int> >:: iterator itr;
        for(itr = edges.begin(); itr != edges.end(); itr ++){
                if((*itr).first == from){
                        tos.push_back((*itr).second);
                }
                if((*itr).second == from){
                        tos.push_back((*itr).first);
                }
        }
        return tos;
}

void removeEdge(int from,int to, vector<pair<int,int> >&edges){
        vector<pair<int,int> >:: iterator itr;
        for(itr = edges.begin(); itr != edges.end(); itr ++){
                if((*itr).first == from && (*itr).second == to
                                || (*itr).first == to && (*itr).second == from){
                        edges.erase(itr);
                        return;
                }
        }
}
void Bayes::Prim(){
        priority_queue<primNode,vector<primNode>,CmpNode> priQ;
        for(int i = 0; i < mutual_info.size(); i ++){
                for(int j = i; j < mutual_info.size(); j++){
                        primNode* node = new primNode();
                        node->from = j;
                        node->to = i;
                        node->cost = mutual_info[i][j];
                        priQ.push(*node);
                }
        }
        bool* visited_node = new bool[mutual_info.size()];
        for(int i = 0; i < mutual_info.size(); i ++)
                visited_node[i] = false;
        visited_node[priQ.top().from] = true;
        vector<pair<int,int> > added_edges;
        for(int i = 0; i < mutual_info.size()-1; i ++){
                primNode node;
                vector<primNode> temp_node;
                do{
                        node = (priQ.top());
                        temp_node.push_back(node);
                        priQ.pop();

                }while(!checkVisitedOrNot(node,visited_node));
                for(int j = 0;j < temp_node.size(); j ++){
                        priQ.push(temp_node[j]);
                }
                //cerr << "Add Edge:" << node.from << " " << node.to << endl;
                added_edges.push_back(make_pair(node.from,node.to));
                visited_node[node.from] = true;
                visited_node[node.to] = true;
        }
        int from = 0;
        addVertex(attr_table[0].name);
        root = getVertex(attr_table[0].name);
        generateTree(from,added_edges);
}

void Bayes::generateTree(int from, vector<pair<int, int> >& edges){
        if(edges.size() == 0)
                return;
        vector<int> to = getEdgeTo(from,edges);
        if(to.size() == 0)
                return;
        addVertex(attr_table[from].name);
        for(int i = 0; i < to.size(); i ++){
                addVertex(attr_table[to[i]].name);
                removeEdge(from,to[i],edges);
                addEdge(attr_table[from].name, attr_table[to[i]].name);
                //cout << "Generate Edge " << from << "- " << to[i] << endl;
                generateTree(to[i],edges);
        }
}

vector<vector<long double> > Bayes::calculateCostForOneParent(int indx){
	int classind = attr_table.size()-1;

	vector<long double> cost_dim1(attr_table[indx].value.size(),0);
	vector<vector<long double> > cost(2,cost_dim1);

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
		cost[0][i] = (long double)(count[0][i]+1)/sum[0];
		cost[1][i] = (long double)(count[1][i]+1)/sum[1];
	}
	return cost;
}

void Bayes::naiveBayes(){
	string classname = attr_table[attr_table.size()-1].name;
	addVertex(classname);
	root = getVertex(classname);
	for(int i = 0; i < attr_table.size()-1; i ++){
		addVertex(attr_table[i].name);
		vector<vector<long double> > cost = calculateCostForOneParent(i);
		addEdge(classname,attr_table[i].name);
                cost1D[i] = cost;
	}
}

void Bayes::TANBayes(){
        calculateMutualInfo();
        Prim();
	string classname = attr_table[attr_table.size()-1].name;
	addVertex(classname);
	root = getVertex(classname);
        for(int i = 0; i < attr_table.size()-1; i ++){
                //cerr << "CPT of Attribute: " << i << endl;
                string name = attr_table[i].name;
                vertex* cur = getVertex(name);
                addEdge(classname,name);
                if(cur->parent.size() == 1){
        		vector<vector<long double> > cost = calculateCostForOneParent(i);
                        cost1D[i] = cost;
                        /*for(int j = 0; j < cost.size(); j ++)
                                for(int k = 0; k < cost[0].size(); k ++)
                                        cerr<<"Pr(" << i << "="<< k << "|" << attr_table.size()-1 << "=" <<  j <<
                                            ")=" << cost[j][k] << endl;
                                            */
                }
                if(cur->parent.size() == 2){
                        vertex* parent1 = cur->parent[0];
                        int indx1 = parent1->indx;
                        int indx2 = attr_table.size()-1;
                        vector<long double> dim3 (attr_table[indx1].value.size(),0);
                        vector<vector<long double> > dim2(attr_table[i].value.size(),dim3);
                        vector<vector<vector<long double> > > cost(attr_table[indx2].value.size(),dim2);

                        vector<int> cdim3 (attr_table[indx1].value.size(),0);
                        vector<vector<int> > cdim2(attr_table[i].value.size(),cdim3);
                        vector<vector<vector<int> > > count(attr_table[indx2].value.size(),cdim2);


                        vector<int> cpdim2 (attr_table[indx2].value.size(),0);
                        vector<vector<int> > partial_count(attr_table[indx1].value.size(),cpdim2);
                        for(int j = 0; j < dataset.size(); j ++){
                                vector<double> instance = dataset[j];
                                count[(int)instance[indx2]][(int)instance[i]][(int)instance[indx1]]++;
                                partial_count[(int)instance[indx1]][(int)instance[indx2]]++;
                        }
                        for(int r = 0; r < attr_table[i].value.size(); r ++)
                                for(int s = 0; s < attr_table[indx1].value.size(); s ++)
                                        for(int t = 0; t < attr_table[indx2].value.size(); t ++){
                                                cost[t][r][s] = (long double)(count[t][r][s]+1)/
                                                        (partial_count[s][t] + attr_table[i].value.size());
                                                /*cerr<<"Pr(" << i << "="<< r << "|" << indx1 <<
                                                        "=" << s << "," << indx2 << "=" << t << ")" <<
                                                        "=" << cost[t][r][s] << endl;*/
                                        }

                        cost2D[i] = cost;
                }

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

int Bayes::testNaiveBayes(vector<vector<double> > testdata){
	int correct_num = 0;
        cout << setprecision(16);
	for(int i =0; i < testdata.size(); i ++){
		vector<double> instance = testdata[i];
		long double* multi_y = new long double[2];
		multi_y[0] = 1; multi_y[1] = 1;
                for(int j = 0; j < root->adj.size(); j ++){
			vector<vector<long double> > cost = (*cost1D.find(j)).second;
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
        return correct_num;
}

int Bayes::testTANBayes(vector<vector<double> > testdata){
	int correct_num = 0;
        cout << setprecision(16);
	for(int i =0; i < testdata.size(); i ++){
		vector<double> instance = testdata[i];
		long double* multi_y = new long double[2];
		multi_y[0] = 1; multi_y[1] = 1;
                for(int j = 0; j < root->adj.size(); j ++){
                        if(root->adj[j]->parent.size() == 1){
        			vector<vector<long double> > cost = (*cost1D.find(j)).second;
	        		multi_y[0] *= cost[0][instance[j]];
		        	multi_y[1] *= cost[1][instance[j]];
                        }
                        if(root->adj[j]->parent.size() == 2){
                                int indx = root->adj[j]->parent[0]->indx;
                                vector<vector<vector<long double> > > cost = (*cost2D.find(j)).second;
                                multi_y[0] *= cost[0][(int)instance[j]][(int)instance[indx]];
                                multi_y[1] *= cost[1][(int)instance[j]][(int)instance[indx]];
                        }
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
        return correct_num;
}
