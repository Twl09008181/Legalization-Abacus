#include<iostream>
#include<vector>
#include<list>
#include <algorithm>
#include "header/abacus.hpp" 



std::vector<node*>pack(std::vector<node*>&nodes,row&r);

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

	std::vector<node*>nodes;
	nodes.push_back(&n1);
	nodes.push_back(&n2);
	nodes.push_back(&n3);
	nodes.push_back(&n4);
	nodes.push_back(&n5);

	std::cout<<"row0 remain:"<<row0.getRemain()<<"\n";
	std::vector<node*>unpacked = pack(nodes,row0);

	if(unpacked.empty())
	for(auto n:nodes)
		std::cout<<n->x<<" "<<n->x+n->width<<"\n";
	else{
		std::cout<<"placed failed--------------- !!\n";
		for(auto n:unpacked)
			std::cout<<"width: "<<n->width<<"\n";
	}

	while(!unpacked.empty())
	{
		auto ripup = row0.RipUp((*unpacked.begin())->width);
		unpacked = pack(unpacked,row0);
		if(!unpacked.empty())
		{
			std::cout<<"final :can't place!\n";
			for(auto un:unpacked)
				std::cout<<un->width<<"\n";
			break;
		}
		unpacked = pack(ripup,row0);
	}

	if(unpacked.empty()){
		std::cout<<"cost :"<<row0.getCost()<<"\n";
		for(auto n:nodes)
			std::cout<<n->x<<" "<<n->x+n->width<<"\n";
	}




	return 0;
}

std::vector<node*>pack(std::vector<node*>&nodes,row&r){
	std::vector<node*>unpacked;
	for(auto n:nodes){
		auto result = r.placeRow(n);
		if(result.first==nullptr){
			unpacked.push_back(n);
		}
		else
			result.first->insert(n);//place
	}
	std::sort(unpacked.begin(),unpacked.end(),[](node* n1,node*n2){return n1->width > n2->width;});
	return unpacked;
}