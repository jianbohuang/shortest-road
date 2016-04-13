
#include "route.h"
#include "lib_record.h"

int	_cost[MAX_NODE_NUM][MAX_NODE_NUM];//顶点间的cost
int _vertex_out[MAX_NODE_NUM][25];   //有出度的顶点列表[0]个数，[3i-2]to顶点，[3i-1]cost,[3i]所用边,i>=1
int _edge_leave_num;				//去掉重复边之后的边数


/*topo：边数据的指针数组, edge_num:图中边的数量*/
void search_route(char *topo[5000], int edge_num, char *demand)
{
	int vertex_num,demand_num,s,t;
	int *demand_nodes;
	int *e_arr=new int[edge_num*3];
	int (*edges)[3]=(int(*)[3])e_arr;//边索引
	PathNode *spath;				
	vertex_num=readData(topo, edge_num, edges, _cost, _vertex_out);
	demand_nodes=NULL;
	demand_num=readDemannd(demand,s,t,demand_nodes);

	if(demand_num==0)
	{
		cout<<"\n demandnode is NULL\n";
		spath=dijkstra(s, vertex_num, _vertex_out);
		if(spath[t].totalcost<INFINITE)
		{
			while(spath[t].parent>=0)
			{
				record_result(spath[t].edge);
				t=spath[t].parent;			
			}
			return;
		}else
			return;
	}
	/*1初始化 去掉t求D(s,vi)*/
	int temp_t=_vertex_out[t][0];
	_vertex_out[t][0]=0;
	spath=dijkstra(s, vertex_num, _vertex_out);
	cout<<"\shorest tree of "<< s<<"\n";
	/*去掉s,t,求D(Vi,Vl)保存在指针向量DIL中,去掉s求D(vi,t)*/
	int temp_s=_vertex_out[s][0];
	_vertex_out[s][0]=0;
	vector<PathNode*> DIL(demand_num);
	for(int i=0;i<demand_num;i++)
	{
		DIL[i]=dijkstra(demand_nodes[i], vertex_num, _vertex_out);
	}
	_vertex_out[t][0]=temp_t;
	_vertex_out[s][0]=temp_s;

	/**迭代n=demand_num次**/
	/**第0次求f<0,vl>**/
	int *f0=new int[demand_num];
	for(int i=0;i<demand_num;i++)
		f0[i]=DIL[i][t].totalcost;
	
	//分配共(n-1)*n*(n-1)计算空间
	int ***fn=new int **[demand_num-1];
	for (int i=0;i<demand_num-1;i++)
	{
		fn[i]=new int *[demand_num];
		for (int j=0;j<demand_num;j++)
		{
			fn[i][j]=new int[demand_num]();
			for(int k=0;k<demand_num;k++)
				fn[i][j][k]=INFINITE;
		}
	}

	int ***prej=new int **[demand_num-1];
	for (int i=0;i<demand_num-1;i++)
	{
		prej[i]=new int *[demand_num];
		for (int j=0;j<demand_num;j++)
		{
			prej[i][j]=new int[demand_num]();
		}
	}

	int ****routeLine=new int ***[demand_num-1];
	for (int i=0;i<demand_num-1;i++)
	{
		routeLine[i]=new int **[demand_num];
		for (int j=0;j<demand_num;j++)
		{
			routeLine[i][j]=new int *[demand_num];
			for (int k=0;k<demand_num;k++)
			{
				routeLine[i][j][k]=new int [vertex_num]();//可以优化为按需申请
			}
		}
	}

	/**第1次，求f<1,vivl>,i!=l, 要考虑demandnum=1的情况**/
	for (int i=0;i<demand_num;i++)
	{
		int vi=demand_nodes[i];
		for (int l=0; l<demand_num; l++)
		{
			if(l==i)continue;		
			int vl=demand_nodes[l];
			if(DIL[i][vl].totalcost>=INFINITE||f0[l]>=INFINITE)
				continue;
			
				//isCycle(D[i][l],f0[l])
				int *linearMap=routeLine[0][i][l];
				int parent=DIL[i][vl].parent;
				while(parent!=vi)
				{
					linearMap[parent]=1;
					parent=DIL[i][parent].parent;
				}

				parent=DIL[l][t].parent;//检查l到t 并且记录中间节点
				while(parent!=vl)
				{
					if(linearMap[parent]!=0)
						break;
					else
						linearMap[parent]=1;
					parent=DIL[l][parent].parent;
				}
			
			if(parent!=vl)//检查有环
				fn[0][i][l]=INFINITE;
			else
			{
				linearMap[t]=1;
				prej[0][i][l]=t;//记录前一个必经点
				fn[0][i][l]=DIL[i][vl].totalcost+f0[l];			
			}
		}
	}

	/**第n=2到|Vs|-1次**/
	for(int n=1; n<demand_num-2; n++)
	{
		int tag=0;
		for (int i=0;i<demand_num;i++)
		{
			
			int vi=demand_nodes[i];
			for(int l=0;l<demand_num ;l++)
			{
				if (l==i)continue;
				int vl=demand_nodes[l];
				if(DIL[i][vl].totalcost>=INFINITE)
					continue;

				int *linearMap=NULL;
				int min=INFINITE;
				int min_j=0;
				for (int j=0;j<demand_num;j++)
				{
					if(j==i||j==l)continue;
					
					if(fn[n-1][l][j]<min)
					{	
						//检查重复
						linearMap=routeLine[n-1][l][j];
						int parent=DIL[i][vl].parent;
						while (parent!=vi)
						{
							if(linearMap[parent]!=0)
								break;
							parent=DIL[i][parent].parent;
						}
						if(parent!=vi)//vi到vl与第n-1次中重复不可行
							continue;

						min=fn[n-1][l][j];
						min_j=j;
					}
				}
				if(min !=INFINITE)//如果有可行的
				{		
					tag=1;
					fn[n][i][l]=DIL[i][vl].totalcost+min;
					//记录节点
					int *srcLine=routeLine[n-1][l][min_j];
					linearMap=routeLine[n][i][l];
					memcpy(linearMap,srcLine,vertex_num*sizeof(int));//复制第n-1次的
					int parent=DIL[i][vl].parent;
					while(parent!=vi)
					{
						linearMap[parent]=1;
						parent=DIL[i][parent].parent;
					}
					//linearMap[vi]=1;//记录节点i自己
					prej[n][i][l]=min_j;//记录选择的j点
				}else
				{
					/*限制情况*/
					fn[n][i][l]=INFINITE;
				}
			}
		}
							if(tag==0)
						cout<<"break in ="<<n;
	}

	/**第n=|Vs|次,fn[n-1]**/
	int *fs=new int[demand_num]();
	for(int i=0;i<demand_num;i++)
		fs[i]=INFINITE;
	int *spre=new int[demand_num]();
	for(int l=0;l<demand_num;l++)
	{ 
		int vl=demand_nodes[l];
		if(spath[vl].totalcost>=INFINITE)
			continue;

		int min =INFINITE;
		int min_j=0;
		for (int j=0;j<demand_num;j++)
		{
			if(j==l)continue;
			int fij=fn[demand_num-2][l][j];
			if(fij<min)
			{
				//检查重复
				int *linearMap=routeLine[demand_num-2][l][j];
				int parent=spath[vl].parent;
				while (parent!=s)
				{
					if(linearMap[parent]!=0)
						break;
					parent=spath[parent].parent;
				}
				if(parent!=s)
					continue;
				
				min=fn[demand_num-2][l][j];
				min_j=j;
			}
		}
		if(min !=INFINITE)
		{
			fs[l]=DIL[s][vl].totalcost+min;
			spre[l]=min_j;
		}
	}

	int min_s2t=INFINITE;
	int last_i=0;
	for (int i=0;i<demand_num;i++)
	{
		if(fs[i]<min_s2t)
		{
			min_s2t=fs[i];
			last_i=i;
		}
	}
	if(min_s2t==INFINITE)//无结果
		return;
	cout<<"THE RESULT="<<min_s2t<<endl;

	vector<int> result;
	//求第一段的边
	int last_v=demand_nodes[last_i];
	int parent=spath[last_v].parent;
	int edg=spath[last_v].edge;
	result.push_back(edg);//放入第一条边
	while (parent!=s)
	{
		edg=spath[parent].edge;
		cout<<"RESULT Node="<<parent<<endl;
		result.insert(result.begin(),edg);//插入第一个位置
		parent=spath[parent].parent;
	}

	//递归n=0到|vs|-2结果的边
	int last_l=spre[last_i];
	for(int n=demand_num-2;n>=0;n--)
	{
		int vl=demand_nodes[last_l];
		int vi=demand_nodes[last_i];
		int parent=DIL[last_i][vl].parent;
		int edg=DIL[last_i][vl].edge;
		int length=result.size();
		result.insert(result.end(),edg);//末尾追加第一个边
		while(parent!=vi)
		{
			edg=DIL[last_i][parent].edge;
			result.insert(result.begin()+length,edg);
			parent=DIL[last_i][parent].parent;
		}	
		
		int j=prej[n][last_i][last_l];
		last_i=last_l;
		last_l=j;
	}

	//最后一段,last_l=t
	int vi_t=demand_nodes[last_i];
	int parent_t=DIL[last_i][t].parent;
	int edg_t=DIL[last_i][t].edge;
	int length_t=result.size();
	result.insert(result.end(),edg_t);//末尾追加第一个边
	while(parent_t!=vi_t)
	{
		edg_t=DIL[last_i][parent_t].edge;
		result.insert(result.begin()+length_t,edg_t);
		parent_t=DIL[last_i][parent_t].parent;
	}	
	

	for (int i=0;i<result.size();i++)
	{
		record_result((result[i]));
		cout<<"RESULT edge="<<result[i]<<endl;
	}

	delete[] e_arr;  
}

//建立图
int readData(char *topo[5000],int edge_num, int edges[][3],int cost[][MAX_NODE_NUM],int vertex_out[][25])
{
	int vertex_count=0;
	int edge_filt_num=0;
	int index,node1,node2,value;	
	char *line_buff=new char[128];//eg: 0,0,1,1
	memset(line_buff,0,128);
	for (int i=0;i<edge_num;i++){	
		strcpy(line_buff,topo[i]);
		sscanf(line_buff,"%d,%d,%d,%d",&index,&node1,&node2,&value);
		edges[i][0]=node1;
		edges[i][1]=node2;
		edges[i][2]=value;
		//cout<<"node1="<<node1<<"node2:"<<node2<<endl;
		int existed_avlue=cost[node1][node2];
		if(existed_avlue==0)//新的边 可以修改为先在连接矩阵确定所有边后再放入出度表
		{
			cost[node1][node2]=value;
			vertex_count=max(vertex_count,max(node1,node2));
			int j=++vertex_out[node1][0];//发现一个新出度边，计数
			vertex_out[node1][j*3-2]=node2;//记录对应进入的顶点
			vertex_out[node1][j*3-1]=value;//边的值
			vertex_out[node1][j*3]=index;//记录边的索引
		}
		else 
		{
			edge_filt_num++;
			if(existed_avlue>value)//发现更小的边替换边索引	
			{
				cost[node1][node2]=value;
				int j=1;
				for(;vertex_out[node1][j*3-2]!=node2;j++);
				vertex_out[node1][j*3-1]=value;
				int old_edge=vertex_out[node1][j*3];
				vertex_out[node1][j*3]=index;			
			}				
		}	
	}

	int vertex_num=vertex_count+1;
	_edge_leave_num=edge_num-edge_filt_num;
	cout<<"\norginal_edge="<<edge_num<<" vertex_num="<<vertex_num<<endl;
	cout<<"_Edge_leave_num="<<_edge_leave_num<<" edge_filt_num="<<edge_filt_num<<endl; 
	cout<<"print _vertex_out[][]:\n";
	delete []line_buff;
	line_buff=NULL;
	return vertex_num;
}

int readDemannd(char* demand_line,int &s ,int &t, int *&demand_nodes)
{
	char* buff=new char(sizeof(demand_line));
	strcpy(buff,demand_line);
	const char *split=",|";
	char *p;
	p=strtok(buff,split);
	s=atoi(p);
	p=strtok(NULL,split);
	t=atoi(p);
	cout<<"s="<<s<<"t="<<t<<endl;
	p=strtok(NULL,split);
	if (p==NULL)
	{
		cout<<"demand num=0\n";
		buff=NULL;
		return 0;
	}
	int i;
	demand_nodes=new int[MAX_DEMAND_NUM];
	for(i=0;p!=NULL;i++)
	{
		demand_nodes[i]=atoi(p);
		p=strtok(NULL,split);
		cout<<demand_nodes[i]<<" ";
	}

	cout<<" deman_num="<<i<<endl;
	//delete buff;//不能直接释放buff
	buff=NULL;
	return i;
}

//用heap排序算单源最短路径
PathNode* dijkstra( int start,int vertex_num,int vertex_out[][25])
{
	int *visited= new int[vertex_num];//visited[i] = 1说明顶点vi已在集合S中
	memset(visited,0,sizeof(int)*vertex_num);
	PathNode *treenode = new PathNode[vertex_num];
	//init
	for (int i=0;i<vertex_num;i++)
	{
		treenode[i].v = i;
	}
	treenode[start].totalcost=0;
	treenode[start].parent=-2;
	priority_queue< PathNode,vector<PathNode>,less<PathNode> > q; //未访问堆
	q.push(PathNode(start,treenode[start].totalcost));
	cout<<"\nDJS for vertex:"<<start<<endl;
	while(!q.empty())
	{
		PathNode top=q.top();
		q.pop();
		int min_vertex=top.v;
		if(visited[min_vertex])
			continue;
		visited[min_vertex]=1;
		int min_cost=treenode[min_vertex].totalcost;
		cout<<"min_vertex="<<min_vertex<<" min_cost="<<min_cost<<endl;
		int out_num=vertex_out[min_vertex][0];
		for (int i=1;i<=out_num;i++)
		{
			int next_v=vertex_out[min_vertex][i*3-2];
			if (visited[next_v]==0)
			{
				int new_value=min_cost+vertex_out[min_vertex][i*3-1];
				if(new_value<treenode[next_v].totalcost)
				{
					treenode[next_v].totalcost=new_value;
					treenode[next_v].parent=min_vertex;
					treenode[next_v].edge=vertex_out[min_vertex][i*3];
					q.push(PathNode(next_v,new_value));					
				}
			}		
		}
	}
	delete []visited;
	return treenode;
}

/**start util_function**/
int maxnum (int a, int b) { if (a < b) return b; else return a; };
int minnum(int a, int b) { if (a <b) return a; else return b; };
/**end util_function**/