#include "../header/abacus.hpp"
#include <algorithm>



// user need to enter terminal with the increasing x order.
// the block function check the relative position between termianl node and place-row , and make the  correction : split the subrows or boundary correction.
void row::block(fixed_node &terminal){
    //pre-checking y range
    if(terminal.origin_y + terminal.height <= y  || terminal.origin_y >= y + height)return ;

    subrow& last = *subrows.rbegin();// this is why user need to enter terminal with the increasing x.
    int condition;//overlap condition
    int t_x1 = terminal.origin_x;
    int t_x2 = t_x1 + terminal.width;
    if( (t_x2 <= last.x1 )  || ( t_x1 >= last.x2) )condition = 0;//not overlap in x.
    else if(t_x1 <= last.x1  && t_x2 >=last.x2)condition = 1; // completely ovelap
    else if(t_x1 <= last.x1 && t_x2 < last.x2)condition = 2; // x|xx---|
    else if(t_x1 > last.x1 && t_x2 < last.x2)condition = 3;//|--xxx--| need split
    else if(t_x1 > last.x1 && t_x2 >= last.x2)condition = 4; //|---xx|x 
    if(condition==1)
        subrows.pop_back();//delete subrow
    else if(condition==2)
        last.x1 = t_x2;
    else if(condition==3){//split new subrow
        subrows.push_back({t_x2,last.x2-t_x2,y});
        last.x2 = t_x1;
    }
    else if(condition==4)
        last.x2 = t_x1;
    if(condition > 1) last.remainSpace = last.x2 - last.x1;
}

// abacus datastructure.
struct cluster{
    int xc; // start x-position 
    // abacus dp
    int ec; // ec <- ec + e(i)
    int qc; // qc <- qc +e(i)[x'(i) -wc ]
    int wc; // wc <- wc + w(i)
    cluster(int x,cluster*pred)
        :xc{x},ec{0},qc{0},wc{0},predecessor{pred}{}
    // if overlap with predecessor cluster, then do cluster.
    cluster * predecessor;
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


cluster* Collapse(cluster *c,int min_x,int max_x){
    c->xc = c->qc / c->ec; // get new position
    // subject to row range
    if(c->xc < min_x)c -> xc = min_x;
    if(c->xc > max_x - c->wc)c -> xc = max_x - c->wc;
    // check if overlap happend.
    cluster * finalCluster = c;
    cluster * pred = c->predecessor;
    if(pred){
        if(pred->xc + pred->wc > c->xc){
            pred->AddCluster(c);
            delete c;
            finalCluster = Collapse(pred,min_x,max_x); 
        }
    }
    return finalCluster;
}

// modified part : boundary fixed.
int modify_x(int x1,int x2,node*n){
    if(n->origin_x < x1)
        return x1;
    if(n->origin_x + n->width > x2)
        return x2 - n->width;
    return n->origin_x;
}

// place the node n in subrow r , and return the last Cluster in this subrow.
cluster* place(cluster*lastC,node *n,subrow*r)
{
    int m_x = modify_x(r->x1,r->x2,n);// abacus fixed.
    //look ahead.
    if(!lastC || lastC->xc + lastC->wc <= m_x){ // if do not need to compact.
        lastC = new cluster(m_x,lastC);
        lastC->AddCell(n);
        //std::cout<<"create cluster, xc:"<<lastC->xc<<" width:"<<lastC->wc<<"\n";
    }
    else{// compactation is best.
        lastC->AddCell(n);
        lastC = Collapse(lastC,r->x1,r->x2);// need to check whether the new movement will touch the pred. 
        //std::cout<<"collapse cluster, xc:"<<lastC->xc<<" width:"<<lastC->wc<<"\n";
    }
    return lastC;
}
// placeRow helper function : update the node's position and return the cost  sum (x-x')^2
int getPos(cluster*lastC,int y){
    int cost = 0;
    while(lastC){
        int x = lastC->xc;
        for(node* n:lastC->nodes){
            n->x = x;
            cost+=std::abs(n->x - n->origin_x);
            cost+=std::abs(n->origin_y - y);
            x += n->width;
        }
        lastC = lastC->predecessor;
    }
    return cost;//total cost
}

// can be called by getting the final position and cost or called trially.(with non-nullptr n)
std::pair<int,int> subrow::placeRow(node*n){
    cluster* lastC = nullptr;
    for(auto ptr = nodes.begin();ptr!=nodes.end();++ptr)
        lastC = place(lastC,*ptr,this);
    
    //caculate position and cost.
    int cost1 = getPos(lastC,y);//get the original cost
    int cost2 = cost1;
    if(n){
        lastC = place(lastC,n,this);// get the new cost
        cost2 = getPos(lastC,y);
    }
    //memory free.
    while(lastC){
        cluster*tmp = lastC;
        lastC = lastC->predecessor;
        delete tmp;
    }
    return {cost1,cost2};
}

//first : optimal subrow to place n
//second : delta_cost
std::pair<subrow*,int> row::placeRow(node* n){
    //std::cout<<"placerow\n";
    subrow* p = nullptr;//find a subrow to place p.
    int bestCost = INT_MAX;
    int i = 0;
    for(auto &sub:subrows){
        //std::cout<<i++<<"remain:"<<sub.remainSpace<<"\n";
        if(sub.remainSpace >= n->width){//still have space
            auto cost = sub.placeRow(n);
            int deltaCost = cost.second - cost.first;//new - original : cost of placing one node.
            if(deltaCost < bestCost){
                p = &sub;
                bestCost = deltaCost;
            }
            else if (deltaCost >= bestCost){ // already find optimal  
                break;
            }
        }
    }
    return {p,bestCost};
}
int row::getCost()
{
    int cost = 0;
    for(auto &sub:subrows)
        cost += sub.placeRow().first;// call placeRow with nullptr.
    return cost;
}
int row::getRemain()
{
    int space = 0;
    for(auto sub:subrows)space+=sub.remainSpace;
    return space;
}
// rip up the node with width < threshod and return.
std::vector<node*> subrow::RipUp(int threshold){
    std::vector<node*>candidate;
    nodes.remove_if([this,&candidate,threshold](node*n){
        if(n->width < threshold){
            candidate.push_back(n);
            remainSpace+=n->width;
            return true;
        }
        return false;
    });
    return candidate;
}
std::vector<node*> row::RipUp(int threshold){
    std::vector<node*>candidate;
    for(auto &sub:subrows)
    {
        auto subCand = sub.RipUp(threshold);
        for(auto c:subCand)
            candidate.push_back(c);
    }
    std::sort(candidate.begin(),candidate.end(),[](node*n1,node*n2){return n1->x < n2->x;});
    return candidate;
}


std::vector<node*>Refine(std::vector<node*>&nodes,row&r){
	std::vector<node*>failed;
	for(auto n:nodes){
		auto result = r.placeRow(n);
		if(result.first==nullptr){
			failed.push_back(n);
		}
		else{
			result.first->insert(n);//place
            n->y = r.y;
        }
	}
	std::sort(failed.begin(),failed.end(),[](node* n1,node*n2){return n1->width > n2->width;});
	return failed;
}
bool RefinementPlace(std::vector<node*>&notDone,std::vector<row>&rows){
    std::sort(notDone.begin(),notDone.end(),[](node*n1,node*n2){return n1->width > n2->width;});
    for(int round = 0;round<10;round++)
    for(int i = 0;i < rows.size() && !notDone.empty();i++)
    {
        auto ripup = rows.at(i).RipUp((*notDone.begin())->width);
        notDone = Refine(notDone,rows.at(i));
        auto notDone2 = Refine(ripup,rows.at(i));
        for(auto n:notDone2)
            notDone.push_back(n);
    }
    return notDone.empty();
}

int abacus(std::vector<node*>nodes,std::vector<row>&rows){
    //sort by x
    std::sort(nodes.begin(),nodes.end(),[](node*n1,node*n2){
        if(n1->origin_x==n2->origin_x)
            return n1->width > n2->width; // wider first
        return n1->origin_x < n2->origin_x;
        }
    );

    std::vector<node*>notDone;notDone.reserve(nodes.size()/10);
    for(auto n : nodes){
        int bestCost = INT_MAX;
        subrow* bestplace = nullptr;
        row* bestRow = nullptr;
        
        int maxTry = 1;
        int count = 0;
        for(auto &r:rows){
            auto place = r.placeRow(n);
            if(place.first && place.second < bestCost){
                bestCost = place.second;
                bestplace = place.first;
                bestRow = &r;
                count++;
            }
            if(count==maxTry)break;
        }
        if(bestplace){
            bestplace->insert(n);
            n->y = bestRow->y;
        }
        else{
            notDone.push_back(n);
        }
    }
    bool done = true;
    if(!notDone.empty()){
        std::cout<<"before refinement\n";
		for(auto n:nodes){
			std::cout<<"pos : "<<n->x<<" "<<n->y<<" ";
			std::cout<<"shape:"<<n->width<<" "<<n->height<<"\n";
		}
        std::cout<<"do refine\n";
        done = RefinementPlace(notDone,rows);
    }

    if(done){ 
        //updating coordinate and return cost
        int cost = 0;
        for(auto &r:rows)
            cost+=r.getCost();
        return cost;
    }
    return -1;
}
