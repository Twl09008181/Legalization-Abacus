#include "../header/abacus.hpp"
#include <algorithm>



// user need to check the row y range is covered by this terminal node.
void row::block(fixed_node &terminal){
    //pre-checking y range
    if(terminal.origin_y + terminal.height <= y  || terminal.origin_y >= y + height)
        return ;
    subrow& last = *subrows.rbegin();       
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
        subrows.push_back({t_x2,last.x2-t_x2});
        last.x2 = t_x1;
    }
    else if(condition==4)
        last.x2 = t_x1;
    if(condition > 1) last.remainSpace = last.x2 - last.x1;
}


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

//ensure collision
int modify_x(int x1,int x2,node*n){
    if(n->origin_x < x1)
        return x1;
    if(n->origin_x + n->width > x2)
        return x2 - n->width;
    return n->origin_x;
}


cluster* place(cluster*lastC,node *n,subrow*r)
{
    int m_x = modify_x(r->x1,r->x2,n);
    //look ahead.
    if(!lastC || lastC->xc + lastC->wc <= m_x){
        lastC = new cluster(m_x,lastC);
        lastC->AddCell(n);
        std::cout<<"create cluster, xc:"<<lastC->xc<<" width:"<<lastC->wc<<"\n";
    }
    else{
        lastC->AddCell(n);
        lastC = Collapse(lastC,r->x1,r->x2);
        std::cout<<"collapse cluster, xc:"<<lastC->xc<<" width:"<<lastC->wc<<"\n";
    }
    return lastC;
}

int getPos(cluster*lastC){
    int cost = 0;
    while(lastC){
        int x = lastC->xc;
        for(node* n:lastC->nodes){
            n->x = x;
            cost += n->weight * (n->x - n->origin_x) * (n->x - n->origin_x);
            x += n->width;
        }
        lastC = lastC->predecessor;
    }
    return cost;//total cost
}

std::pair<int,int> subrow::placeRow(node*n){
    cluster* lastC = nullptr;
    for(auto ptr = nodes.begin();ptr!=nodes.end();++ptr)
        lastC = place(lastC,*ptr,this);
    
    //caculate position and cost.
    int cost1 = getPos(lastC);
    int cost2 = cost1;
    if(n){
        lastC = place(lastC,n,this);
        cost2 = getPos(lastC);
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
    subrow* p = nullptr;//find a subrow to place p.
    int bestCost = INT_MAX;
    for(auto &sub:subrows){
        if(sub.remainSpace >= n->width){//still have space
            auto cost = sub.placeRow(n);
            int deltaCost = cost.second - cost.first;
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
        cost += sub.placeRow().first;
    return cost;
}
int abacus(std::vector<node>&nodes,std::vector<row>&rows){
    //sort by x
    std::sort(nodes.begin(),nodes.end(),[](node&n1,node&n2){return n1.origin_x < n2.origin_x;});
    for(auto &n : nodes){
        int bestCost = INT_MAX;
        subrow* bestplace = nullptr;
        for(auto &r:rows){
            auto place = r.placeRow(&n);
            if(place.first && place.second < bestCost){
                bestCost = place.second;
                bestplace = place.first;
            }
        }
        if(bestplace)
            bestplace->insert(&n);
        else{
            std::cout<<"abacus failed\n";
            exit(1);
        }
    }
    //updating coordinate
    int cost = 0;
    for(auto &r:rows)
        cost+=r.getCost();
    return cost;
}
