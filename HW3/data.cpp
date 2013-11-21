#include "header.h"

void Data::parse_header(string singleline){
	int pos = singleline.find_first_of(' ');
	string type = singleline.substr(1,pos-1);
	if(type == "attribute"){	//make sure this line contains attribute information
		attribute attr;
		//Get the name of attribute
		int pos_start = singleline.find_first_of("\'")+1;
		int pos_end = singleline.find_last_of("\'")-1;
		attr.name = singleline.substr(pos_start,pos_end-pos_start+1);
		//cout << attr.name<< endl;
		//Determine the type of attribute : 0-real 1-nominal
		if(singleline.find_first_of('{') == -1)
			attr.type = 0;
		else{
			attr.type = 1;
			int temp_start = singleline.find_first_of('{')+2;
			int temp_end = singleline.find_last_of('}')-1;
			string temp = singleline.substr(temp_start,temp_end-temp_start+1);
			//Add the value of the nominal attribute to attr
			while(temp.length()!=0 ){
				int temp_pos = temp.find_first_of(',');
				//Deleta the space
				if (temp_pos == -1)
					temp_pos = temp.find_first_of(' ') == -1? temp.length(): temp.find_last_of(' ');
				attr.value.push_back(temp.substr(0,temp_pos));
				temp.erase(0,temp_pos+2);
			}
		}
		if(attr.name == "class"){
			for(int i = 0; i < attr.value.size(); i ++){
				if(attr.value[i] == "metastases")
					NEG_CLASS = i;
				else if(attr.value[i] == "malign_lymph")
					POS_CLASS = i;
			}
		}
		//Determine the index of each value
		attr.index = attr_table.size();
		//Store each attribute to the attr_table
		attr_table.push_back(attr);
	}
}

void Data::parse_data(string singleline){
    vector<double> instance;
//Get the value of each attribute
	for(int i=0; i < attr_table.size(); i++){
		if(singleline.length() == 0){
			cout<< " Error instance: " << endl;
			exit(-1);
		}
		int pos = singleline.find_first_of(',');
		if(pos == -1)
			pos = singleline.length();
		string value = singleline.substr(0,pos);
		if(attr_table[i].type == 0){
			double temp_value;
			istringstream(value)>>temp_value;
			instance.push_back(temp_value);
    	}else{
			//If the attribute is nominal type, transform the value into integer according to the attribute table
			double temp_value;
			temp_value = find(attr_table[i].value.begin(),attr_table[i].value.end(),value)-attr_table[i].value.begin();
			instance.push_back(temp_value);
		}
		singleline.erase(0,pos+1);
	}
    data.push_back(instance);
}
vector<vector<double> > Data::getData(){
	return data;
}
vector<attribute> Data::getAttr(){
	return attr_table;
}

/*Print the data table
PARA: table: the data that needed to print
*/
void Data::printData(){
	for( int i = 0; i < data.size(); i ++){
		for(int j = 0; j < data[i].size(); j ++)
			cout << data[i][j] << " ";
		cout << endl;
	}
}

/*Print the attribute table
PARA: table: the attribute table that needed to print
*/
void Data::printAttr_table(){
	int i,j;
	cout << attr_table.size() << endl;
	for( i = 0; i < attr_table.size(); i++){
		cout << attr_table[i].name<< " " << attr_table[i].type<<endl;
		if(attr_table[i].type == 1){
			for(j = 0; j < attr_table[i].value.size();j++)
				cout << attr_table[i].value[j] << " ";
		}
		cout << endl;
	}
}


