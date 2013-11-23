#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <math.h>
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <cfloat>
#include <assert.h>
#include <map>
#include <queue>

using namespace std;

struct attribute
{
	string name;
	int type; //0-numeric 1-nominal
	vector<string> value; //for numeric type
	int index; //the index in the data
};

class Data{
  private:
	vector<vector<double> > data;
	vector<attribute> attr_table;
	int NEG_CLASS;
	int POS_CLASS;

  public:
  void parse_header(string singleline);
  void parse_data(string singleline);
  vector<vector<double> > getData();
  vector<attribute> getAttr();
  void printData();
  void printAttr_table();
};

struct vertex{
	vector<vertex*> adj;
        vector<vertex*> parent;
	string name;
        int indx;
	vertex(string s){
	  name =s;
	}
};

struct primNode{
        int from;
        int to;
        long double cost;
};
struct CmpNode
{
        bool operator()(const primNode& lhs, const primNode& rhs){
               return lhs.cost<rhs.cost;
        }
};

class Bayes{
  public:
	Bayes(Data data);
        typedef map<int,vector<vector<long double> > > cost1Dmp;
        cost1Dmp cost1D;
        typedef map<int,vector<vector<vector<long double> > > > cost2Dmp;
        cost2Dmp cost2D;
	vector<vector<double> > dataset;
	vector<vector<long double> > mutual_info;
	vector<attribute> attr_table;
	typedef map<string, vertex* > vmap;
	vmap network;
	vertex* root;
	double* prob_y;
	void addVertex(const string &name);
	void addEdge(const string& from, const string& to);
	void displayBayes();
	void naiveBayes();
	void TANBayes();
	vector<vector<long double> > calculateCostForOneParent(int indx);
	void calculateMutualInfo();
	void printMutualInfo();
	vertex* getVertex(const string &name);
	void testNaiveBayes(vector<vector<double> > testdata);
	void testTANBayes(vector<vector<double> > testdata);

        void Prim();
        void generateTree(int from, vector<pair<int,int> >& edges);
        int getAttrIndx(string name);
};
