#ifndef __ROUTE_H__
#define __ROUTE_H__
#include <iostream>
#include <string.h>
#include <vector>
#include <queue>
#include <cstdio>
#include <cstdlib>
using namespace std;
#define NULL 0
#define INFINITE 32768//无限大 600x20=12000
#define MAX_NODE_NUM 600
#define MAX_DEMAND_NUM 50


//最短路径的节点
struct PathNode
{
	int v;
	int totalcost;
	int parent;//父节点编号-2代表根节点，-1代表还未能到达的点
	int edge;//与父节点相连的边

	PathNode(int a=0,int b=INFINITE,int c=-1,int e=-1):
		v(a),totalcost(b),parent(c),edge(e){}
	friend bool operator< (PathNode n1, PathNode n2)
	{
		return n1.totalcost > n2.totalcost;//考虑是否要求相等情况下比较出入度
	}
};



int readDemannd(char* demand_line,int &s ,int &t, int *&demand_nodes);
int readData(char *topo[5000],int edge_num, int edges[][3],int cost[][MAX_NODE_NUM],int vertex_out[][25]);
//用heap排序算单源最短路径
PathNode* dijkstra( int start,int vertex_num,int vertex_out[][25]);

void search_route(char *graph[5000], int edge_num, char *condition);
#endif
