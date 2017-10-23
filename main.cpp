//
//  main.cpp
//  AI2
//
//  Created by Jeet Mody on 10/13/17.
//  Copyright © 2017 Jeet Mody. All rights reserved.
//

#include <algorithm>
#include <vector>
#include <functional>
#include <queue>
#include <iostream>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstdio>
#include <math.h>
#include <random>
#include <set>
#include <string>
#include <map>
#include <climits>
using namespace std;
void printch(vector<vector<char>>);
int prune = 0;
int nodes = 0;
int branches = 0;
int start;
bool timeover = false;


struct compare
{
    bool operator()(pair<int,pair<int,int>> p , pair<int,pair<int,int>> q)
    {
        return p.first < q.first;
    }
};

class board
{
    
public:
    int n,m;
    vector <vector<char>> b; //Make it vector
    priority_queue <pair<int,pair<int,int>>,vector<pair<int,pair<int,int>>>,compare> next_states;
    int score;
    bool endState;
    board *parent;
    int movex,movey;
    
    board()
    {
        this->score = INT_MIN;
    }
    board(int n, vector <vector<char>> b)
    {
        this->n = n;
        this->m = n;
        this->b = b;
        endState = 0;
        //parent = NULL;
    }
    
    
    int mark(vector <vector<char>>& temp, int x, int y, char ch)
    {
        int c = 0;
        if(x >= 0 && y>=0 && x<n && y<n && temp[x][y] == ch)
        {
            temp[x][y] = '*';
            c = 1;
            c+= mark(temp,x+1,y,ch);
            c+= mark(temp,x-1,y,ch);
            c+= mark(temp,x,y-1,ch);
            c+= mark(temp,x,y+1,ch);
        }
        return c;
    }
    
    
    void generateStates(bool max)
    {
        auto temp = b;
        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j<n; j++)
            {
                if(temp[i][j]!='*')
                {
                    //cout<<"At i = "<<i<<" and j = "<<j<<"Temp is :\n";
                    //printch(temp);
                    int val = mark(temp,i,j,temp[i][j]);
                    //printch(temp);
                    //int vall;
                    /*if(max)
                     vall = val*val + score;
                     else
                     vall = -(score - val*val);
                     */
                    next_states.push(make_pair(val*val,make_pair(i,j)));
                    //next_states.push(make_pair(vall,make_pair(i,j)));
                }
            }
        }
        
        //cout<<"B is :\n";
        //printch(b);
        //cout<<"Temp is :\n";
        //printch(temp);
    }
    
    
    void gravity(vector <vector<char>> &temp, int x, int y)
    {
        //cout<<"before Gravity\n";
        //printch(temp);
        for (int i = 0; i<n ; i++)
        {
            int countS = 0;
            for (int j = n-1; j>=0; j--)
            {
                if(temp[j][i] == '*')
                    countS++;
                else
                {
                    if(countS>0)
                    {
                        //cout<<"-----\n";
                        //cout<<"j = "<<j<<" i = "<<i<<endl;
                        //cout<<countS;
                        temp[j+countS][i] = temp[j][i];
                        //cout<<countS<<endl;
                        temp[j][i] = '*';
                    }
                    
                }
            }
            
        }
        //cout<<"After Gravity\n";
        //printch(temp);
        
    }
    
    
    board* getNext(int x, int y, bool max, int curVal)
    {
        auto newB = b;
        mark(newB,x,y,newB[x][y]);
        gravity(newB,x,y);
        int flag = 0;
        for(int i = 0; i<n; i++)
        {
            for(int j = 0; j<n; j++)
            {
                if(newB[i][j] != '*')
                {
                    flag = 1;
                    break;
                }
            }
        }
        
        
        board *next = new board(n,newB);
        if(parent == NULL)
        {
            next->parent = next;
            next->movex = x;
            next->movey = y;
        }
        else
        {
            next->parent = this->parent;
        }
        if(max)
        {
            next->score = this->score + curVal;
            //next->score = curVal;
        }
        else
        {
            next->score = this->score - curVal;
            //next->score = - curVal;
        }
        
        if(flag == 0)
            next->endState = 1;
        return next;
    }
    
    void print()
    {
        for (vector<char> x : b)
        {
            for (char y : x)
            {
                cout<<y;
            }
            cout<<endl;
        }
        cout<<"----------------\n";
    }
    
    void printfile(ofstream& f)
    {
        for (vector<char> x : b)
        {
            for (char y : x)
            {
                f<<y;
            }
            f<<endl;
        }
    }
    
    
};
int depth = 0;

void printch(vector<vector<char>> temp)
{
    for (vector<char> x : temp)
    {
        for (char y : x)
        {
            cout<<y;
        }
        cout<<endl;
    }
    cout<<"--------\n";
}
void printpq( priority_queue <pair<int,pair<int,int>>,vector<pair<int,pair<int,int>>>,compare> pq)
{
    while(!pq.empty())
    {
        auto x = pq.top();
        cout<<x.first<<":"<<x.second.first<<" "<<x.second.second<<endl;
        pq.pop();
    }
}

board* alphabeta(board* b, int alpha, int beta,int depth, bool maximize, int max_depth, double time)
{
    
    if(depth== max_depth || b->endState)
    {
        return b;
    }
    
    auto totTime =(double)(clock()-start)/ CLOCKS_PER_SEC;
    
    if(totTime>time)
    {
        cout<<totTime<<endl;
        cout<<time;
        timeover = true;
        return b;
    }
    
    nodes++;
    if(maximize)
    {
        
        b->generateStates(maximize);
        auto pq= b->next_states;
        board* newBoard= NULL;
        board* maxBoard= NULL;
        pair<int,int> point;
        
        while(!pq.empty())
        {
            auto best = pq.top();
            int curVal = best.first;
            point = best.second;
            pq.pop();
            newBoard = b->getNext(point.first,point.second,maximize, curVal);
            auto newState = alphabeta(newBoard, alpha,beta, depth+1, false,max_depth, time);
            
            if(timeover)
                return maxBoard;
            
            if(maxBoard == NULL)
            {
                maxBoard = newState;
            }
            
            else if(newState->score > maxBoard->score)
            {
                maxBoard = newState;
            }
            
            newState = NULL;
            delete newState;
            alpha= max(alpha,maxBoard->score);
            
            if( beta <= alpha)
            {
                prune++;
                break;
            }
            
        }
        
        return maxBoard;
        
    }
    
    else
    {
        b->generateStates(maximize);
        auto pq= b->next_states;
        board* newBoard= NULL;
        board* minBoard= NULL;
        pair<int,int> point;
        
        while(!pq.empty())
        {
            auto best = pq.top();
            int curVal = best.first;
            point = best.second;
            pq.pop();
            newBoard = b->getNext(point.first,point.second,maximize, curVal);
            auto newState = alphabeta(newBoard, alpha,beta, depth+1, true ,max_depth,time);
            
            if(timeover)
                return minBoard;
            
            if(minBoard == NULL)
            {
                minBoard = newState;
            }
            
            if(newState->score < minBoard->score)
            {
                minBoard = newState;
            }
            
            newState = NULL;
            delete newState;
            beta= min(beta,minBoard->score);
            
            if( beta <= alpha)
            {
                prune++;
                break;
            }
        }
        return minBoard;
    }
    
}


int main(int argc, const char * argv[]) {
    
    start = clock();
    fstream inputfile;
    ios_base::sync_with_stdio(false);
    int n,p;
    double time;
    inputfile.open("/Users/jeetmody/IBM/AI2/AI2/input5.txt");
    inputfile>>n>>p>>time;
    //cout<<n;
    vector<vector<char>> v(n,vector<char>(n,0));
    char ch;
    for(int i=0;i<n;++i)
    {
        for(int j=0;j<n;++j)
        {
            inputfile>>ch;
            v[i][j]= ch;
            
        }
        
    }
    
    ofstream op;
    string o = "/Users/jeetmody/IBM/AI2/AI2/output.txt";
    time = time/2;
    int j = 8;
    
    if(time<1.5)
        j = 1;
    
    board *res2 = NULL, *res = NULL;
    double tim[9];
    for(int i = 1; i <= 10; i++)
    {
        cout<<"Hi";
        board *root = new board(n,v);
        root->parent = NULL;
        root->score = 0;
        cout<<"\nGoing for i :"<<i<<endl;
        res2 = alphabeta(root, INT_MIN , INT_MAX , 0, true, i,time);
        if(timeover)
        {
            cout<<endl<<i<<endl;
            cout<<"timeover";
            break;
        }
        
        auto totTime =(double)(clock()-start)/ CLOCKS_PER_SEC;
        tim[i] = totTime;
        cout<<"Completed"<<totTime<<endl;
        if(i>=4)
        {
            
            auto even = tim[2]/tim[1];
            auto odd = tim[3]/tim[2];
            if(totTime*even*odd<time)
                i = i%2==0?i+1:i;
            
        }
        
        op.open (o, ios::out);
        res = res2;
        char x = res->parent->movey + 'A';
        op<<x;
        op<<res->parent->movex+1<<endl;
        res->parent->printfile(op);
        op.close();
        cout<<endl<<x;
        cout<<res->parent->movex+1<<endl;
        if(res->endState)
            break;
        
        int multi = i%2==0 ? tim[3]/tim[2] : tim[2]/tim[1];
        if(totTime*multi*0.95>time)
            break;
        
    }
    
    return 0;
}
