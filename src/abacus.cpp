#include "../header/abacus.hpp"
#include <algorithm>
#include <omp.h>
#include <climits>




// user need to enter terminal with the increasing x order.
// the block function check the relative position between termianl node and place-row , and make the  correction : split the subrows or boundary correction.
void row::block(fixed_node &terminal){
    //pre-checking y range
    if(terminal.origin_y + terminal.height <= y  || terminal.origin_y >= y + height)return ;

    subrow* last = &(*subrows.rbegin());// this is why user need to enter terminal with the increasing x.
    int condition;//overlap condition
    int t_x1 = terminal.origin_x;
    int t_x2 = t_x1 + terminal.width;
    if( (t_x2 <= last->x1 )  || ( t_x1 >= last->x2) )condition = 0;//not overlap in x.
    else if(t_x1 <= last->x1  && t_x2 >=last->x2)condition = 1; // completely ovelap
    else if(t_x1 <= last->x1 && t_x2 < last->x2)condition = 2; // x|xx---|
    else if(t_x1 > last->x1 && t_x2 < last->x2)condition = 3;//|--xxx--| need split
    else if(t_x1 > last->x1 && t_x2 >= last->x2)condition = 4; //|---xx|x 
    if(condition==1)
        subrows.pop_back();//delete subrow
    else if(condition==2)
        last->x1 = t_x2;
    else if(condition==3){//split new subrow
        subrows.push_back({t_x2,last->x2-t_x2,y});
        last = &subrows.at(subrows.size()-2);//push back may allocate new memory
        last->x2 = t_x1;
    }
    else if(condition==4)
        last->x2 = t_x1;
    if(condition > 1) last->remainSpace = last->x2 - last->x1;
}



void subrow::Collapse(){
    int c = clusterNum - 1;
    // check if overlap happend.
    for(;c >= 0 ;c--){
        cluster&cur = Clusters.at(c);
        cur.xc = cur.qc / cur.ec;
        if(cur.xc < x1) cur.xc = x1;
        if(cur.xc > x2 - cur.wc)cur.xc = x2 - cur.wc;
        
        if(c > 0 && Clusters.at(c-1).xc + Clusters.at(c-1).wc > cur.xc)
            Clusters.at(c-1).AddCluster(&cur);
        else 
            break;//do not decrease c
    }
    clusterNum = c + 1;
}

// modified part : boundary fixed.
inline int modify_x(int x1,int x2,node*n){
    if(n->origin_x < x1)
        return x1;
    if(n->origin_x + n->width > x2)
        return x2 - n->width;
    return n->origin_x;
}

void subrow::AppendCluster(int x){
    if(Clusters.size()==clusterNum){//need allocate new memmory.
        Clusters.push_back(cluster{x});
    }
    else if(Clusters.size() > clusterNum)//update clusterNum
        Clusters.at(clusterNum) = cluster{x};
    else{
        std::cerr<<"subrow::AppendCluster error, clusterNum > clusters.size()\n";
        exit(1);
    }
    clusterNum++;
}


void subrow::place(node*n){
    int m_x = modify_x(x1,x2,n);// abacus fixed.
    if(empty() || last().xc + last().wc <= m_x){//if do not need place compactly.
        AppendCluster(m_x);//new cluster
        last().AddCell(n);//new cluster add cell
    }
    else{
        last().AddCell(n);
        Collapse();//recursive Collapse
    }
}

int subrow::getPos(){
    int cost = 0;
    for(int c = 0;c < clusterNum;c++){
        cluster& clst = Clusters.at(c);
        int x = clst.xc;
        for(node*n : clst.nodes){
            n->x = x;
            n->y = y;
            cost+=std::abs(n->x - n->origin_x);
            cost+=std::abs(n->y - n->origin_y);
            x += n->width;
        }
    }
    return cost;//total cost
}


int SubRowPlace(subrow*r,node*n){
    r->place(n);
    return r->getPos();
}


int BsSub(std::vector<subrow>&subs,node*n){
    int l = 0;
    int r= subs.size()-1;
    while(l<r)
    {
        int mid = (l+r)/2;
        if(subs.at(mid).x1==n->origin_x)return mid;
        else if(subs.at(mid).x1 > n->origin_x)r = mid-1;
        else l = mid+1;
    }
    return std::max(0,l);
}

bool tryPlace(subrow&sub,node*n,int&bestCost,subrow*&bestp){
    if(sub.remainSpace >= n->width){//still have space
        sub.place(n);
        int afterCost = sub.getPos();
        int dCost = afterCost - sub.cost;//new - original : cost of placing one node.
        sub.RecvoerClusters();
        if(dCost < bestCost){
            bestp = &sub;
            bestCost = dCost;
            return true;
        }
        else 
            return false;
    }
    return true;
}

//first : optimal subrow to place n
//second : delta_cost
std::pair<subrow*,int> row::placeRow(node* n){
    //std::cout<<"placerow\n";
    subrow* p = nullptr;//find a subrow to place p.
    int bestCost = INT_MAX;
    int start = BsSub(subrows,n);
    for(int i = start-1;i<=start+1;i++){
        if(i>=0&&i<subrows.size())
            tryPlace(subrows.at(i),n,bestCost,p);
    }
   
    for(int i = start-2;i>=0;i--){
        if(i>=0)
            if(!tryPlace(subrows.at(i),n,bestCost,p))break;
    }
    for(int i = start+2;i<subrows.size();i++){
        if(i<subrows.size())
            if(!tryPlace(subrows.at(i),n,bestCost,p))break;
    }
    return {p,bestCost};
}
int row::getCost()
{
    int cost = 0;
    for(auto &sub:subrows)
        cost += sub.getPos();// get final position
    return cost;
}
int row::getRemain()
{
    int space = 0;
    for(auto sub:subrows)space+=sub.remainSpace;
    return space;
}

int binarySearchRow(std::vector<row>&rows,node*n)
{
    int l = 0;
    int r = rows.size()-1;
    while(l<r)
    {
        int mid = (l+r)/2;
        if(rows.at(mid).y==n->origin_y)return mid;
        else if(rows.at(mid).y > n->origin_y)r = mid-1;
        else l = mid+1;
    }
    return std::max(0,l);
}


extern bool FIXEDORDER;//for mulitithreading
bool chooseRow(int cost,subrow* bplace,int r,int*bestcost,subrow**bestplace,int*bestrow)
{
    if(FIXEDORDER)
    {
        if(cost<*bestcost || r > *bestrow){
            *bestcost = cost;
            *bestplace = bplace;
            *bestrow = r;
            return true;
        }
    }
    else if(cost < *bestcost){
        *bestcost = cost;
        *bestplace = bplace;
        *bestrow = r;
        return true;
    }
    return false;

}

bool tryPlace2(std::vector<row>&rows,int i,node*n,int&bestCost,subrow*&bestPlace,int&bestRow)
{
    row& r = rows.at(i);
    auto place = r.placeRow(n);
    if(place.first && place.second <= bestCost){
        return chooseRow(place.second,place.first,i,&bestCost,&bestPlace,&bestRow);
    }
    else if(place.first)
    return false;
    return true;
}



#include<thread>
#include<mutex>
#include<queue>
#include <condition_variable>


struct jobArgs{
    node*n;
    int r;
};
struct threadArg{
    std::vector<row>*rows;
    int bestcost = INT_MAX;
    subrow*bestplace = nullptr;
    int bestrow = -1;
    bool busy = false;
};

void Realjob(jobArgs arg1,threadArg*arg2)
{
    tryPlace2(*arg2->rows,arg1.r,arg1.n,arg2->bestcost,arg2->bestplace,arg2->bestrow);
}

void DoJob(bool*shutdown,std::queue<jobArgs>*jobs,std::condition_variable*cv,std::mutex*m,threadArg*arg)
{
    while(!*shutdown){
        arg->busy = false;
        std::unique_lock<std::mutex>locker(*m);
        cv->wait(locker,[jobs,shutdown]{return *shutdown||!jobs->empty();});//當shudown或是有新任務時停止等待   
        if(*shutdown)break;
        
        arg->busy = true;
        if(!jobs->empty()){
            auto job = jobs->front();jobs->pop();
            locker.unlock();
            Realjob(job,arg);
        }
        else
            locker.unlock();
        arg->busy = false;
    }
}

class threadPool{
public:
    threadPool(int threadNum,std::vector<row>*rows)
    {
        shutDownFlag = false;
        threadArgs.resize(threadNum);
        for(auto &targ:threadArgs)
            targ.rows = rows;
        for(int i = 0;i<threadNum;i++){
            threads.push_back(std::thread(DoJob,&shutDownFlag,&jobs,&cv,&m,&threadArgs.at(i)));
        }
    }

    void reset()
    {
        for(auto &targ:threadArgs)
        {
            targ.bestcost = INT_MAX;
            targ.bestplace = nullptr;
            targ.bestrow = -1;
        }
    }

    void newJob(jobArgs job)
    {
        std::unique_lock<std::mutex>locker(m);
        jobs.push(job);
        locker.unlock();
        cv.notify_one();//通知做事
    }
    void shutDown()
    {
        shutDownFlag = true;
        cv.notify_all();
        for(auto &thread:threads){
            thread.join();
        }
    }
    bool doneJobs(){
        std::unique_lock<std::mutex>locker(m);
        /*
        return jobs.empty();//這邊有bug,光是empty是不夠!
        */
       if(!jobs.empty())return false;

        for(auto targ:threadArgs)
            if(targ.busy)
                return false;

        return true;
    }
//private:
    std::queue<jobArgs>jobs;
    std::condition_variable cv;
    std::mutex m;
    std::vector<std::thread>threads;
    std::vector<threadArg>threadArgs;
    bool shutDownFlag;
};



extern int ROWRANGE;


int abacus_omp_v2(std::vector<node*>nodes,std::vector<row>&rows,int threadNum,bool fixed_order){
    //sort by x
    std::sort(nodes.begin(),nodes.end(),[](node*n1,node*n2){
        if(n1->origin_x==n2->origin_x)
                return n1->width < n2->width; 
        return n1->origin_x < n2->origin_x;
        }
    );
    int thread_num = 0;
#pragma omp parallel num_threads(threadNum)
    thread_num = omp_get_num_threads();

    std::vector<node*>tracks[thread_num];
    int row_num_in_track = rows.size()/thread_num;
    for(size_t i=0;i<nodes.size();++i){
        int r = binarySearchRow(rows,nodes[i]);
        tracks[(r/row_num_in_track)%thread_num].push_back(nodes[i]);
    }
    /*
    for(int i=0;i<thread_num;++i){
        std::cout<<tracks[i].size()<<std::endl;
    }*/


    bool succ = true;
    
#pragma omp parallel num_threads(thread_num)
{
    int t_id = omp_get_thread_num();
    std::vector<node*>& track = tracks[t_id];
    for(auto n : track){
        int bestCost = INT_MAX;
        subrow* bestplace = nullptr;
        int bestRow = -1;
        int startRow = binarySearchRow(rows,n);
        int range = ROWRANGE;
        for(int i = std::max(startRow-range,t_id*row_num_in_track);i<=std::min(startRow+range,(t_id+1)*row_num_in_track-1);i++){
            if(i>=0 && i<rows.size())
                tryPlace2(rows,i,n,bestCost,bestplace,bestRow);
        }
        for(int i = startRow-range-1;i>=t_id*row_num_in_track;i--)
            if(!tryPlace2(rows,i,n,bestCost,bestplace,bestRow))break;
        for(int i = startRow+range+1;i<(t_id+1)*row_num_in_track;i++)
            if(!tryPlace2(rows,i,n,bestCost,bestplace,bestRow))break;

        if(bestplace){
            bestplace->place(n);
            bestplace->Backup();
            bestplace->cost = bestplace->getPos();
            bestplace->remainSpace-=n->width;
            if(bestplace->remainSpace<0){ 
                std::cerr<<"abacus remainspace<0!\n";
                exit(1);
            }
        }
        else{
            succ = false;
            break;
        }
    }

}

    if(succ){
        int cost = 0;
        for(auto &r:rows)
            cost+=r.getCost();
        return cost;
    }
    else{
        std::cout<<"abacus failed\n";
        return -1;
    }
}

int abacus_Thread(std::vector<node*>nodes,std::vector<row>&rows,int threadNum,bool fixed_order){
    FIXEDORDER = fixed_order;
    //sort by x
    std::sort(nodes.begin(),nodes.end(),[](node*n1,node*n2){
        if(n1->origin_x==n2->origin_x)
                return n1->width < n2->width; 
        return n1->origin_x < n2->origin_x;
        }
    );

    threadPool P(threadNum,&rows);

    bool succ = true;

    int j = 0;
    for(auto n : nodes){
        P.reset();
        int bestCost = INT_MAX;
        subrow* bestplace = nullptr;
        int bestRow = -1;
        int startRow = binarySearchRow(rows,n);
        int range = ROWRANGE;

       for(int i = startRow-range;i<=startRow+range;i++){
           if(i>=0 && i<rows.size()){
               P.newJob(jobArgs{n,i});
           }
        }

        while(!P.doneJobs());//busy wait

        //Get best
        for(auto t:P.threadArgs){
            if(t.bestcost<=bestCost )
                chooseRow(t.bestcost,t.bestplace,t.bestrow,&bestCost,&bestplace,&bestRow);
        }

        for(int i = startRow-range-1;i>=0;i--)
            if(!tryPlace2(rows,i,n,bestCost,bestplace,bestRow))break;
        for(int i = startRow+range+1;i<rows.size();i++)
            if(!tryPlace2(rows,i,n,bestCost,bestplace,bestRow))break;

        if(bestplace){
            bestplace->place(n);
            bestplace->Backup();
            bestplace->cost = bestplace->getPos();
            bestplace->remainSpace-=n->width;
            if(bestplace->remainSpace<0){ 
                std::cout<<n->name<<"\n";
                std::cerr<<"abacus remainspace<0!\n";
                exit(1);
            }
        }
        else{
            succ = false;
            break;
        }
    }

    std::cout<<"done\n";
    if(succ){
        int cost = 0;
        for(auto &r:rows)
            cost+=r.getCost();
        while(!P.doneJobs());//busy wait
        P.shutDown();
        return cost;
    }
    else{
        std::cout<<"abacus failed\n";
        while(!P.doneJobs());//busy wait
        P.shutDown();
        return -1;
    }
}




//Serial Version
int abacus(std::vector<node*>nodes,std::vector<row>&rows){
    //sort by x
    std::sort(nodes.begin(),nodes.end(),[](node*n1,node*n2){
        if(n1->origin_x==n2->origin_x)
                return n1->width < n2->width; 
        return n1->origin_x < n2->origin_x;
        }
    );

    bool succ = true;
    for(auto n : nodes){
        int bestCost = INT_MAX;
        subrow* bestplace = nullptr;
        int bestRow = -1;
        int startRow = binarySearchRow(rows,n);
        int range = ROWRANGE;
        for(int i = startRow-range;i<=startRow+range;i++){
            if(i>=0 && i<rows.size())
                tryPlace2(rows,i,n,bestCost,bestplace,bestRow);
        }
        for(int i = startRow-range-1;i>=0;i--)
            if(!tryPlace2(rows,i,n,bestCost,bestplace,bestRow))break;
        for(int i = startRow+range+1;i<rows.size();i++)
            if(!tryPlace2(rows,i,n,bestCost,bestplace,bestRow))break;

        if(bestplace){
            bestplace->place(n);
            bestplace->Backup();
            bestplace->cost = bestplace->getPos();
            bestplace->remainSpace-=n->width;
            if(bestplace->remainSpace<0){ 
                std::cerr<<"abacus remainspace<0!\n";
                exit(1);
            }
        }
        else{
            succ = false;
            break;
        }
    }
    if(succ){
        int cost = 0;
        for(auto &r:rows)
            cost+=r.getCost();
        return cost;
    }
    else{
        std::cout<<"abacus failed\n";
        return -1;
    }
}
