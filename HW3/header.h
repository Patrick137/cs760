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
	typedef pair<vector<vector<double> >, vertex *> ve;
	vector<ve> adj;
        vector<vertex*> parent;
	string name;
	vertex(string s){
	  name =s;
	}
};

class Bayes{
  public:
	Bayes(Data data);
	vector<vector<double> > dataset;
	vector<attribute> attr_table;
	typedef map<string, vertex* > vmap;
	vmap network;
	vertex* root;
	double* prob_y;
	void addVertex(const string &name);
	void addEdge(const string& from, const string& to, vector<vector<double> >);
	void displayBayes();
	void naiveBayes();
	vector<vector<double> > calculateCostForNaive(int indx);
	vertex* getVertex(const string &name);
	void testNaiveBayes(vector<vector<double> > testdata);
};
