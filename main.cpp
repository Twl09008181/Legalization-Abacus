#include<iostream>
#include<vector>
#include<list>
#include <algorithm>
#include "header/abacus.hpp" 
#include <fstream>
#include <string>

void output(std::vector<row>&rows,std::vector<fixed_node*>terminals,std::vector<node*>&nodes);

void output2(std::vector<node*>&nodes,std::vector<fixed_node*>&terminals1,std::vector<fixed_node*>&terminals2);
void parser(std::string auxName,std::vector<row>&rows,std::vector<fixed_node*>&terminals1,std::vector<fixed_node*>&terminals2,std::vector<node*>&nodes);

int ROWRANGE = 18;//default
bool FIXEDORDER = true;//set false to enhance flexibility. 
int THREADNUM = 0;
int main(int argc,char *argv[])
{

	std::vector<row>rows;
	std::vector<fixed_node*>terminals1;
	std::vector<fixed_node*>terminals2;
	std::vector<node*>nodes;
	parser(argv[1],rows,terminals1,terminals2,nodes);

	std::sort(terminals1.begin(),terminals1.end(),
		[](fixed_node*t1,fixed_node*t2){return t1->origin_x < t2->origin_x;}
	);
	
	for(auto t : terminals1){
		for(auto &r:rows)
			r.block(*t);
	}


	//Program setting
	for(int i = 0;i<argc;i++)
	{
		if(std::string(argv[i])=="-r" && i+1 < argc){
			ROWRANGE = std::stoi(argv[i+1]);
		}
		else if(std::string(argv[i])=="-nofixed")
			FIXEDORDER = false;
		else if(std::string(argv[i])=="-t"&& i+1 < argc)
			THREADNUM = std::stoi(argv[i+1]);
	}
	std::cout<<"Program Setting :\n";
	std::cout<<"ROWRANGE:"<<ROWRANGE<<"\nFIXEDORDER: "<< ( FIXEDORDER? "FIXED":"NOFIXED" )<<"\n";
	std::cout<<"THREADNUM:";
	if(THREADNUM)
		std::cout<<THREADNUM<<"\n";
	else 
		std::cout<<"Serial\n";
	std::cout<<"start do abacus\n";

	//Run abacus
	int cost;
	if(!THREADNUM)
		cost = abacus(nodes,rows);
	else 
		cost = abacus_Thread(nodes,rows,THREADNUM,FIXEDORDER);

	if(cost!=-1)
		std::cout<<"total cost:"<<cost<<"\n";
	else
		std::cout<<"abacus failed\n";
	
	//output(rows,terminals1,nodes);
	output2(nodes,terminals1,terminals2);

	return 0;
}

void pl_Line(std::ifstream& pl,std::string &name,int &x,int &y,bool *is_fixed=nullptr){
	pl >> name;
	std::string x_str,y_str;
	pl >> x_str >>y_str;
	x = std::stoi(x_str);
	y = std::stoi(y_str);
	std::string trash;
	pl >> trash >> trash;

	if(is_fixed)
	{
		pl >> trash;
		*is_fixed = (trash=="/FIXED");
	}
}

void nodes_Line(std::ifstream& nodes,std::string name,int &w,int &h,bool *is_fixed=nullptr){
	std::string n;
	nodes >> n;
	if(n!=name){std::cerr<<"nodes_Line  error\n";exit(1);}
	std::string w_str,h_str;
	nodes >>w_str >>h_str;
	w = std::stoi(w_str);
	h = std::stoi(h_str);
	if(is_fixed)
		nodes >> n;
}


void parser(std::string auxName,std::vector<row>&rows,std::vector<fixed_node*>&terminals1,std::vector<fixed_node*>&terminals2,std::vector<node*>&nodes){

	std::ifstream aux{auxName};
	if(!aux){std::cerr<<"can't open "<<auxName<<"\n";exit(1);}
	//get .nodes .pl .scl
	std::string trash,nodesName,placeName,rowName;
	aux>>trash>>trash>>nodesName>>trash>>trash>>placeName>>rowName>>trash;
	aux.close();
	// read scl file
	std::ifstream scl{rowName};
	if(!scl){std::cerr<<"can't open "<<rowName<<"\n";exit(1);}
	std::string rowInfo = " ";
	while(rowInfo!="NumRows")scl>>rowInfo;scl >> rowInfo; // : 
	scl >> rowInfo;
	int rowNum = std::stoi(rowInfo);
	rows.reserve(rowNum);
	for(int i = 0;i < rowNum;i++)
	{
		while(rowInfo!="Coordinate")scl>>rowInfo;scl>>rowInfo;// :
		scl >> rowInfo;
		int y = std::stoi(rowInfo);

		while(rowInfo!="Height")scl>>rowInfo;scl>>rowInfo;// :
		scl >> rowInfo;
		int height = std::stoi(rowInfo);
		
		while(rowInfo!="Sitewidth")scl>>rowInfo;scl>>rowInfo;// :
		scl >> rowInfo;
		int siteWidth = std::stoi(rowInfo);

		while(rowInfo!="SubrowOrigin")scl>>rowInfo;scl>>rowInfo;// :
		scl >> rowInfo;
		int x = std::stoi(rowInfo);
		while(rowInfo!="NumSites")scl>>rowInfo;scl>>rowInfo;// :
		scl >> rowInfo;
		int NumSites = std::stoi(rowInfo);
		rows.push_back({x,y,NumSites*siteWidth,height});
	}
	scl.close();

	std::ifstream nodesinfo{nodesName};
	std::ifstream placeinfo{placeName};
	if(!nodesinfo){std::cerr<<"can't open "<<nodesName<<"\n";exit(1);}
	if(!placeinfo){std::cerr<<"can't open "<<placeName<<"\n";exit(1);}
	std::string ninfo;
	
	while(ninfo!="NumNodes")nodesinfo>>ninfo;nodesinfo>>ninfo;
	nodesinfo >> ninfo;
	int numNodes = std::stoi(ninfo);
	while(ninfo!="NumTerminals")nodesinfo>>ninfo;nodesinfo>>ninfo;
	nodesinfo >> ninfo;
	int Numterm = std::stoi(ninfo);
	nodesinfo >> ninfo;//first nodes name

	int x,y,w,h;
	std::string coordinate;
	while(coordinate!=ninfo)placeinfo >> coordinate;

	//first line
	std::string name = ninfo;
	nodesinfo >> ninfo;
	w = std::stoi(ninfo);
	nodesinfo >> ninfo;
	h = std::stoi(ninfo);	

	placeinfo >> coordinate;
	x = std::stoi(coordinate);
	placeinfo >> coordinate;
	y = std::stoi(coordinate);
	placeinfo >> coordinate >> coordinate;

	nodes.push_back(new node{name,x,y,w,h});
	for(int i = 1;i < numNodes - Numterm;i++)
	{
		pl_Line(placeinfo,name,x,y);
		nodes_Line(nodesinfo,name,w,h);
		nodes.push_back(new node{name,x,y,w,h});
		//std::cout<<name<<" "<<x<<" "<<y<<" "<<w<<" "<<h<<"\n";
	}
	for(int i = 0;i<Numterm;i++)
	{
		bool isFixed;
		pl_Line(placeinfo,name,x,y,&isFixed);
		nodes_Line(nodesinfo,name,w,h,&isFixed);
		if(isFixed)
			terminals1.push_back(new node{name,x,y,w,h});
		else 
			terminals2.push_back(new node{name,x,y,w,h});
		
		//std::cout<<name<<" "<<x<<" "<<y<<" "<<w<<" "<<h<<"\n";
	}


	nodesinfo.close();
	placeinfo.close();


}

void output(std::vector<row>&rows,std::vector<fixed_node*>terminals,std::vector<node*>&nodes){
	std::ofstream out{"Layout"};


	out<<rows.size()<<"\n"<<terminals.size()<<"\n"<<nodes.size()<<"\n\n";

	for(auto r:rows){
		out<<r.subrows.begin()->x1<<" "<<r.subrows.rbegin()->x2<<" "<<r.y<<" "<<r.y+r.height<<"\n";
	}
	out<<"\n";

	for(auto t:terminals)
		out<<t->origin_x<<" "<<t->origin_x + t->width<<" "<<t->origin_y<<" "<<t->origin_y+t->height<<"\n";

	out<<"\n";

	for(auto n:nodes)
		out<<n->x<<" "<<n->x+n->width<<" "<<n->y<<" "<<n->y+n->height<<"\n";


	out.close();
}
void output2(std::vector<node*>&nodes,std::vector<fixed_node*>&terminals1,std::vector<fixed_node*>&terminals2){

	std::ofstream out{"output.pl"};

	for(auto n:nodes)
		out << n->name << " "<<n->x<<" "<<n->y<<" : N\n";
	for(auto n:terminals1)
		out << n->name << " "<<n->origin_x<<" "<<n->origin_y<<" : N /FIXED\n";
	for(auto n:terminals2)
		out << n->name << " "<<n->origin_x<<" "<<n->origin_y<<" : N /FIXED_NI\n";

	out.close();





}
