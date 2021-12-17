#ifndef ABACUS_HPP
#define ABACUS_HPP

#include<iostream>
#include<vector>
#include<list>
#include <algorithm>
#include <utility>

struct node{
	node(int x,int y,int w = -1,int h = -1 )
		:origin_y{y},origin_x{x},width{w},height{h}{}
	node(std::string n,int x,int y,int w  ,int h   )
		:name{n},origin_y{y},origin_x{x},width{w},height{h}{}
	void setShape(int w,int h){
		width = w;
		height = h;
	}
	int origin_x,origin_y;//global placement result
	int width;
	int height;
	int weight = 1;
	int x,y;
	std::string name;
};
struct subrow{
	subrow(int startx,int width,int coordinate)
		: x1{startx},x2{startx + width},y{coordinate}{remainSpace = x2-x1;}
	void insert(node*n){
		nodes.push_back(n);
		remainSpace-=n->width;
		if(remainSpace < 0){
			std::cerr<<"subrow insert error\n";
			exit(1);
		}
	}
	std::vector<node*>RipUp(int threshold);//RipUp  the width <= threshold
	std::pair<int,int> placeRow(node*n=nullptr);
	std::list<node*>nodes;
	int x1,x2;
	int remainSpace;
	int y;
};

using fixed_node = node;
struct row{
	row(int x,int coordinate,int w,int h)
		:y{coordinate},height{h}{
		subrows.push_back({x,w,coordinate});
	}
	int y;
	int height;
	std::vector<subrow> subrows;
	//need sorted by x.
	void block(fixed_node&terminal);
	std::pair<subrow*,int> placeRow(node* n);
	int getCost();
	int getRemain();
	std::vector<node*> RipUp(int threshold);//RipUp  the width <= threshold
};

void placeTerminal(std::vector<fixed_node>&terminals,std::vector<row>&rows);
int abacus(std::vector<node*>nodes,std::vector<row>&rows);

#endif
