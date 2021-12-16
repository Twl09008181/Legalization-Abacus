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
    cluster(int x)
        :xc{x},ec{0},qc{0},wc{0}{}
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

int subrow::placeRow(node*n){
    cluster* lastC = nullptr;
    if(!nodes.empty()){ //processing the already placed nodes.
        auto ptr = nodes.begin();
        lastC = new cluster(modify_x(x1,x2,*ptr));
        lastC->AddCell(n);
        for(++ptr;ptr!=nodes.end();++ptr){
            int m_x = modify_x(x1,x2,*ptr);
            if(lastC->xc + lastC->wc <= m_x){
                lastC = new cluster(m_x);
                lastC->AddCell(n);
            }
            else{
                lastC->AddCell(n);
                Collapse(lastC,x1,x2);
            }
        }
    }
    if(n)
    {

    }
    //caculate position
    while(lastC)
    {
        cluster*tmp = lastC;
        lastC = lastC->predecessor;
        delete tmp;
    }
}

//first : optimal subrow to place n
//second : delta_cost
std::pair<subrow*,int> row::placeRow(node* n){
    subrow* place = nullptr;
    int bestCost = INT_MAX;
    for(auto &sub:subrows){
        if(sub.remainSpace >= n->width){
            int cost = sub.placeRow(n);
            if(cost < bestCost){
                place = &sub;
                bestCost = cost;
            }
            else if (cost >= bestCost){ // already find optimal  
                break;
            }
        }
    }
    return {place,bestCost};
}
void row::updatePos()
{
    for(auto &sub:subrows)
        sub.placeRow();
}
void abacus(std::vector<node>&nodes,std::vector<row>&rows){
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
   for(auto &r:rows)
        r.updatePos();
}
