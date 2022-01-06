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
// abacus datastructure.
struct cluster{
    int xc; // start x-position 
    // abacus dp
    int ec; // ec <- ec + e(i)
    int qc; // qc <- qc +e(i)[x'(i) -wc ]
    int wc; // wc <- wc + w(i)
    cluster(int x)
        :xc{x},ec{0},qc{0},wc{0}{}
    // if overlap with predecessor cluster, then do cluster.
    // nodes for caculate each position x
    std::list<node*>nodes;
    void AddCell(node*n){
        nodes.push_back(n);
        ec += n->weight;
        qc += n->weight * (n->origin_x - wc);
        wc += n->width;
    }
    void AddCluster(cluster*other)
    {
        for(auto n:other->nodes)nodes.push_back(n);
        ec += other->ec;
        qc += (other->qc - other->ec * wc);
        wc += other->wc;
    }
};

struct subrow{
	subrow(int startx,int width,int coordinate)
		: x1{startx},x2{startx + width},y{coordinate}
	{
		remainSpace = x2-x1;
		cost = 0;
		clusterNum = BackupNum=0;
	}
	cluster& last(){
		if(empty()){
			std::cerr<<"error in cluster&last(),Clusters is empty\n";
			exit(1);
		}
		return Clusters.at(clusterNum-1);
	}
	bool empty(){return clusterNum==0;}
	void AppendCluster(int);
	void Collapse();//Collapse start from last Cluster in Clusters, and update ClusterIdx.
	void place(node*);//Place node to last Cluster in Clusters .
	int  getPos();//return the total Cost in Clusters.

	std::vector<cluster>Clusters;
	int x1,x2;
	int remainSpace;
	int y;
	int cost;
	int clusterNum;// point to last Cluster in Clusters.

	void Backup()
	{
		if(clusterNum > ClustersBackup.size())//need allocate 
			ClustersBackup.push_back(Clusters.back());
		
		if(clusterNum > ClustersBackup.size()){
			std::cerr<<"Back up error,clusterNum > clusterBack.size()\n";
			exit(1);
		}
		BackupNum = clusterNum;
		ClustersBackup.at(BackupNum-1) = Clusters.at(clusterNum-1);
	}

	//each time after calling subrow::place(node*) , need recover .
	void RecvoerClusters(){
		if(clusterNum==BackupNum)//only need recover last cluster.
			Clusters.at(clusterNum-1) = ClustersBackup.at(clusterNum-1);
		else if(clusterNum < BackupNum) // clusters call collapse during place(node*)
		{
			if(Clusters.size() < BackupNum)
			{
				std::cerr<<"Clusters.size() not enough!\n";
				exit(1);
			}
			for(int idx = clusterNum-1;idx < BackupNum;idx++)
				Clusters.at(idx) = ClustersBackup.at(idx);
		}
		clusterNum = BackupNum;
	}
private:
	int BackupNum;// point to last Cluster in Backup
	std::vector<cluster>ClustersBackup;
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
int abacus_Thread(std::vector<node*>nodes,std::vector<row>&rows,int threadNum,bool fixed_order = false);
int abacus_omp_v1(std::vector<node*>nodes,std::vector<row>&rows,int threadNum,bool fixed_order = false);

#endif
