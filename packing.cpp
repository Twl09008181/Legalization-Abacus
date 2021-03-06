#include<iostream>
#include<vector>
#include<list>
#include <algorithm>
#include "header/abacus.hpp" 
#include <fstream>

void output(std::vector<row>&rows,std::vector<fixed_node*>terminals,std::vector<node*>&nodes);


int main()
{


	
	// x range : (0,30)	
	// row rowH = 4 
	int rowH = 4;
	int rowW = 30;
	row row0{0,2,rowW,rowH};
	row row1{0,6,rowW,rowH};
	row row2{0,10,rowW,rowH};
	row row3{0,14,rowW,rowH};
	
	std::vector<row>rows{row0,row1,row2,row3};
	//std::vector<row>rows{row0};

	fixed_node t0{12,2,4,12}; //x = 12 ~ 16 , y = 2 ~ 14
	fixed_node t1{18,4,2,8};  //x = 18 ~ 20 , y = 4 ~ 12 
	fixed_node t2{22,0,2,20}; //x = 22 ~ 24 , y = 0 ~ 20
	fixed_node t3{25,3,2,2};  //x = 25 ~ 27   y = 3 ~ 5
	fixed_node t4{28,4,4,11}; //x = 28 ~ 32   y = 4 ~ 15 
	fixed_node t5{33,1,2,19}; //x = 33 ~ 35   y = 2 ~ 20

	std::vector<fixed_node*> terminals{&t0,&t1,&t2,&t3,&t4,&t5};
	std::sort(terminals.begin(),terminals.end(),
		[](fixed_node*t1,fixed_node*t2){return t1->origin_x < t2->origin_x;}
	);
	
	for(auto t : terminals){
		for(auto &r:rows)
			r.block(*t);
	}
	/*
	for(auto r:rows){
		for(auto sub:r->subrows){
			std::cout<<"["<<sub.x1<<" "<<sub.x2<<"] ";
		}
		std::cout<<"\n";
	}*/
	

	node n1{0,0,12,rowH};
	node n2{0,0,1,rowH};
	node n3{10,0,2,rowH};
	node n4{40,0,2,rowH};//can't place !! 
	node n5{41,0,1,rowH};
	node n6{100,0,22,rowH};
	node n7{15,0,4,rowH};
	node n8{31,0,4,rowH};
	node n9{11,0,4,rowH};
	node n10{0,0,4,rowH};
	node n11{5,0,4,rowH};
	node n12{5,0,4,rowH};
	node n13{5,0,4,rowH};
	node n14{5,0,2,rowH};
	node n15{5,0,2,rowH};
	node n16{5,0,2,rowH};
	node n17{5,0,2,rowH};
	node n18{5,0,2,rowH};
	node n19{5,0,2,rowH};
	node n20{5,0,2,rowH};
	node n21{5,0,1,rowH};
	node n22{5,0,1,rowH};

	std::vector<node*>nodes;
	nodes.push_back(&n1);
	nodes.push_back(&n2);
	nodes.push_back(&n3);
	nodes.push_back(&n4);
	nodes.push_back(&n5);
	nodes.push_back(&n6);
	nodes.push_back(&n7);
	nodes.push_back(&n8);
	nodes.push_back(&n9);
	nodes.push_back(&n10);
	nodes.push_back(&n11);
	nodes.push_back(&n12);
	nodes.push_back(&n13);
	nodes.push_back(&n14);
	nodes.push_back(&n15);
	nodes.push_back(&n16);
	nodes.push_back(&n17);
	nodes.push_back(&n18);
	nodes.push_back(&n19);
	nodes.push_back(&n20);
	nodes.push_back(&n21);
	nodes.push_back(&n22);


	int cost = abacus(nodes,rows);
	if(cost!=-1)
	{
		std::cout<<"total cost:"<<cost<<"\n";
		for(auto n:nodes){
			std::cout<<"pos : "<<n->x<<" "<<n->y<<" ";
			std::cout<<"shape:"<<n->width<<" "<<n->height<<"\n";
		}
	}
	else{
		std::cout<<"abacus failed\n";
	}

	output(rows,terminals,nodes);

	return 0;
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