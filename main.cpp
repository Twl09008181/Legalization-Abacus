#include<iostream>
#include<vector>
#include<list>
#include <algorithm>
#include "header/abacus.hpp" 


int main()
{


	/*
	// x range : (0,30)	
	// row height = 4 
	int rowH = 4;
	int rowW = 30;
	row row0{0,2,rowW,rowH};
	row row1{0,6,rowW,rowH};
	row row2{0,10,rowW,rowH};
	row row3{0,14,rowW,rowH};
	
	std::vector<row*>rows{&row0,&row1,&row2,&row3};

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
		for(auto r:rows)
			r->block(*t);
	}
	for(auto r:rows){
		for(auto sub:r->subrows){
			std::cout<<"["<<sub.x1<<" "<<sub.x2<<"] ";
		}
		std::cout<<"\n";
	}
	*/


	int height = 10;
	subrow row1{30,40};//30 ~ 70

	//first prepare total width = 30 to pack.
	node n1{0,0,5,height};//adjust to 30~35
	node n2{15,0,5,height};//adjust to 35~40
	node n3{35,0,10,height};//adjust to 40~50
	node n4{55,0,5,height};// same : 55~60
	node n5{80,0,5,height};//adjust to 65~70

	std::vector<node>nodes;
	nodes.push_back(n1);
	nodes.push_back(n2);
	nodes.push_back(n3);
	nodes.push_back(n4);
	nodes.push_back(n5);

	for(auto &n:nodes)
		row1.insert(&n);
	row1.placeRow();

	
	for(auto &n:nodes)
		std::cout<<n.x<<" "<<n.x+n.width<<"\n";

	node n6{90,0,10,height};//packed from behind. n4,n5 need be pushed front 10 width.
	row1.insert(&n6);
	row1.placeRow();	

	for(auto &n:nodes)
		std::cout<<n.x<<" "<<n.x+n.width<<"\n";
	std::cout<<n6.x<<" "<<n6.x+n6.width<<"\n";


	return 0;
}
