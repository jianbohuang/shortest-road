#include "include.h"
#include <time.h>
int maxnum (int a, int b) { if (a < b) return b; else return a; };
void intdisplay(void *t) { cout << *(int *)t << " "; };

Array::~Array()
{
      for (int i=0 ; i < rows*cols ; i++)
         if (array[i] != NULL) delete array[i];
      delete array;
}

Array::Array (int r, int c)
{
   rows = r;
   cols = c;
   array = (void **)calloc(1, sizeof(void *)*r*c);
   for (int i=0 ; i < r*c ; i++) array[i] = NULL;
}

Array::Array(int c)
{
   rows = 1;
   cols = c;
   array = (void **)calloc(1,sizeof(void **)*c);
   for (int i=0 ; i < c ; i++) array[i] = NULL;
}

void *Array::get(int i, int j)
{
   if (i < 0 || i > rows) return NULL;
   if (j < 0 || j > cols) return NULL;
   return array[(i-1)*cols+j-1];
}

void *Array::get(int i)
{
   if (i < 0 || i > cols) return NULL;
   return array[i-1];
}

void Array::put(int i, int j, void *k)
{
   if (i > 0 && i <= rows && j > 0 && j <= cols) {
      if (array[(i-1)*cols+j-1] != NULL) delete array[(i-1)*cols+j-1];
      array[(i-1)*cols+j-1] = k;
   }
}

void Array::put(int i, void *k)
{
   if (i > 0 && i <= cols) {
      if (array[i-1] != NULL) delete array[i-1];
      array[i-1] = k;
   }
}

void Array::sort(int (* f)(const void *, const void *))
{
   size_t size;
   for (size=0 ; array[size] != NULL && size <= cols ; size++);
   qsort(array, size, sizeof(void *), f);
}

void fill(Array *a, int x, int y, int num)
{
   if (x > 1)
   {
      for (int i=1 ; i <= x ; i++)
         for (int j=1 ; j <= y ; j++) a->put(i,j, new int(num));
   }
   else
      for (int i=1 ; i <= y ; i++) a->put(i, new int(num));
}

class Cell
{
friend class Queue;
friend class Stack;

   void *item;
   Cell *next;

public:
   Cell(void *ptr, Cell *lst)
   {
      item = ptr;
      next = lst;
   }
};


class Queue
{
   Cell *tail;
   void (* dispfn)(void *);

public:
   Queue(void (* d)(void *)) { dispfn = d; tail = NULL;  }
   Queue() { dispfn = intdisplay; tail = NULL;  }
   void enqueue(void *t);
   void *dequeue();
   void display();
   int  empty()  {  return tail == NULL;  }
   ~Queue ()
   {
      void *t;
      while (tail != NULL) if ((t = dequeue()) != NULL) delete t;
   }
};

void Queue::enqueue(void *t)
{
   Cell *h;
   if (t == NULL) return;

   if (tail == NULL) {
      tail = new Cell(t, NULL);
      tail->next = tail;
   } else {
      h = new Cell(t, tail->next);
      tail->next = h;
      tail = h;
   }
}

void *Queue::dequeue()
{
   if (tail == NULL) return NULL;
   Cell *ptr = tail->next;
   void *t = ptr->item;
   if (ptr != tail) {
      tail->next = ptr->next;
      tail = tail->next;
   } else
      tail = NULL;
   delete ptr;
   return t;
}

void Queue::display()
{
   Cell *t;
   if (tail == NULL) { cout << "(empty)\n";  return; }
   for (t=tail->next ; t != tail ; t = t->next) (dispfn)(t->item);
   (dispfn)(tail->item);
   cout << "\n";
}


class node
{
   int nodeid;

public:
   int type;
   int children;
   int prnt;
   node (int c, int p, int t, int chi) {  nodeid = c;  
             prnt = p; type = t; children=chi; }
   node (int c, int p) {nodeid = c; prnt = p;}
   int parent()  {  return prnt; }
   int nodenum()  {  return nodeid;  }
};

// 在Union中檢查root的編號
int SimpleFind(int i, Array * WeightUnion)
{
	while ( *(int *)WeightUnion->get(i) > 0 ) 
		i  = *(int *)WeightUnion->get(i);
	return i;
}

// Weighted Union 利用 array 實作, 用來檢查是否有cycle產生
int redundancy (int node1, int node2, Array *WeightUnion)
{
   int temp, root1, root2, parent1, parent2;   
   root1 = SimpleFind( node1, WeightUnion );
   root2 = SimpleFind( node2, WeightUnion );
   // 檢查二個點是否屬於同一個集合, 如果是則會產生cycle
   if (root1 == root2 ) return 1;
   parent1 = *(int *)WeightUnion->get(root1);
   parent2 = *(int *)WeightUnion->get(root2);
   temp =  parent1 + parent2;
   if (parent1 > parent2) {
	   WeightUnion->put(root1, new int (root2));
	   WeightUnion->put(root2, new int (temp));   }    
	   else {
	   WeightUnion->put(root2, new int (root1));
	   WeightUnion->put(root1, new int (temp));       
	   } 
   return 0;
}

//讀取測試資料檔中的Nodes, Edges, Terminal 這些參數
void ReadConfigData(char *filename, int *cities, int *edges, 
					int *parnum)
{
   char type[10];
   int value;
   char *buffer = new char[128];

   fstream fin(filename, ios::in);
   while (fin.getline(buffer, 128, '\n')) {
      sscanf(buffer, "%s%d", type, &value);
	  switch (type[0]) { 
      case 'N':
		 if (type[1] == 'o' ) *cities = value;
		 break;
      case 'E':
		 if (type[1] == 'd' ) *edges = value;
		 break;
      case 'T':
		 if (type[1] == 'e' ) *parnum = value;
		 break;
      }
   }
   fin.close();
}

// 讀取測試檔中的資料, connection用來存所有edge的資料
// allnode存所有點的資料, cost 則是2維陣列來存所有點之間的cost
void ReadData(Array *connections, Array *cost, Array *allnode, 
			  Array *pnode, char *filename)
{
   char *buffer = new char[128];
   int node1, node2, value, index=0, sindex=0;
   char type[10];
   cout << "Reading Data-->";
   fstream fin(filename, ios::in);
   while (fin.getline(buffer, 128, '\n')) {
      sscanf(buffer, "%s%d%d%d", type, &node1, &node2, &value);
	  if (strlen(type)>1) continue;
	  switch (type[0]) { 
      case 'E':
         cost->put(node1, node2, new int(value)); 
		 cost->put(node2, node1, new int(value)); 
		 if (node1 < node2) 
			 connections->put(++index,new link(node1,node2,value));
		 else
             connections->put(++index,new link(node2,node1,value));
		 break;
      case 'T':
         pnode->put(++sindex,new int(node1));
		 allnode->put(node1, new node (node1, -1, PARTINODE, 0));
		 break;
      }
   }
   fin.close();
   cout << "OK\n";
}  

// QSort中使用的判斷函數
int edgecomp (const void *x, const void *y)
{
   if ((*(link **)x)->getCost() < (*(link **)y)->getCost()) return -1;
   if ((*(link **)x)->getCost() > (*(link **)y)->getCost()) return 1;
   return 0;
}


int main( int argc, char **argv )
{
   int cities=0, parnum=0, edges=0, count=0, i, index = 0, totalcost;
   Array *cnnct, *partinode, *allnode, *solut, *cost, *G1, *G2;
   Array *G1solu, *G2solu;
   terminal **path;
   clock_t starttime, endtime;

   if ( argc <= 1 ) 
      usage ( ); 

   if(access(argv[1], 00)) 
   {
     cout<<"File does not exist\n";  
     return 0;
   }


   ReadConfigData(argv[1], &cities, &edges, &parnum);
   cout << "Total Nodes-> " << cities <<"\n";
   cout << "Total Edges-> " << edges << "\n";
   cout << "Terminal Nodes-> " << parnum << "\n";

   cnnct = new Array(edges);	
   partinode = new Array(parnum);   
   allnode = new Array(cities);   
   solut = new Array(cities, cities);
   cost = new Array(cities, cities);
   G1 = new Array(parnum*(parnum-1)/2+1);
   G2 = new Array(cities*(cities-1)/2+1);
   G1solu = new Array(parnum, parnum);
   G2solu = new Array(cities,cities);   
   path = new terminal*[cities+1];
   
   // 初始化資料
   initialnode(allnode, cities);
   fill( cost, cities, cities, INFINITE );

   //讀入資料
   ReadData(cnnct, cost, allnode, partinode, argv[1]);

   //量時間用的
   starttime = clock();

   //初始化資料
   fill( G1solu, parnum, parnum, 0 );
   fill( G2solu, cities, cities, 0 );
   fill(solut, cities, cities,0);

   cout << "Step 1. Construct G1 Graph->";
   cout.flush();
   //建立Terminal到其它點的shortest path tree
   for ( i=1; i<= parnum; i++ )
       path[*(int *)partinode->get(i)] = 
	        dijkstra(&index, cities, cost, i, parnum, partinode, G1);
   cout << "OK\n";
   cout.flush();

   cout << "Step 2. Find MST of G1->";
   cout.flush();
   //建立G1的Minimum Spanning Tree
   spanning(G1, G1solu, index, parnum);
   cout << "OK\n";
   
   cout << "Step 3. Construct G2 Graph->";
   cout.flush();
   //建立G2
   index=constructG2( cities, parnum, partinode, cost, G2, G1solu, path);
   cout << "OK\n";
   cout << "Step 4. Find MST of G2->";
   cout.flush();
   
   //建立G2的Minimum Spanning Tree
   spanning(G2, G2solu, index, cities);
   cout <<  "OK\n";
   cout << "Step 5. Delete Pendant Edges of G2->";
   cout.flush();

   //移除掉pendant的edge, 並且計算整個Steiner Tree的cost
   totalcost = delete_pendant( G2solu, allnode, cost,
	   *(int *)partinode->get(1), cities );
   cout << "OK\n";
   endtime = clock();  
   //印出整個Steiner Tree的edge
   printsolu( G2solu, cost, cities );
   cout << "Total tree cost: " << totalcost << "\n";
   cout << "Total Time: "  << (endtime-starttime)  << " msec \n";

   delete path;
   delete cnnct;
   delete G1;
   delete G1solu;
   return 1;
}

//Kruskal' algorithm
void spanning(Array *conn, Array *solut, int length, int max)
{
   int count=0;
   long i;
   Array *WeightUnion;
   link  *lo = NULL;
   WeightUnion = new Array(max);   
   fill(WeightUnion, 1, max, -1);

   // 將所有的edge由小排到大
   conn->sort(edgecomp);   

   for (i=1 ; i<=length ; i++) {
      lo = (link *)conn->get(i);
      //檢查是否有cycle產生
      if (!redundancy(lo->getnode1(), lo->getnode2(), WeightUnion)) {
         solut->put(lo->getnode1(), lo->getnode2(), new int(1));
         solut->put(lo->getnode2(), lo->getnode1(), new int(1));         
         if (++count == max-1) break;
	  }
	}
   delete WeightUnion;
}


//建立G2, 將G1的edge用G原來的path取代
int constructG2( int cities, int parnum, Array *parti, 
	Array *cost, Array *G2, Array *G1solu, terminal **tree)
{
	int i, j, v, v1, parent, index=0;
    for (i=1; i<=parnum; i++)
        for (j=i+1; j<=parnum; j++) 
			if (*(int *)G1solu->get(i,j) == 1 ) {
				v = *(int *)parti->get(i);
				v1 = *(int *)parti->get(j);
				while (tree[v][v1].parent != -1) {
					parent = tree[v][v1].parent;
					if (v1 < parent) 
					   G2->put(++index, 
						   new link (v1, parent, *(int *)cost->get(v1,parent) ));
					else
   					   G2->put(++index, 
						   new link (parent, v1, *(int *)cost->get(parent,v1) ));
					v1 = tree[v][v1].parent;
				}	
			}
     return index;
}

// 將結果印出
void printsolu(Array *solu, Array *cost, int max)
{
	int i, j ;
	for (i=1;i<=max;i++) 
		 for (j=i+1;j<=max; j++)
			 if (*(int *)solu->get(i, j))
		          cout << "<" << i << "," << j << "> -  " << *(int *)cost->get(i,j) << "\n";

}

// 將所有點設定其Parent為-1, children為0			
void initialnode( Array *allnode, int max )
{
	int i;
	for(i=1; i<=max; i++)
		allnode->put(i, new node (i, -1, NORMALNODE, 0));
}

// 刪除非Terminal的leave
int delete_pendant( Array *solu, Array *allnode, Array *cost, int start, int max )
{
   Queue *s = new Queue();
   node  *current, *childptr;

    int curr_no, child, parent, treecost=0;

   s->enqueue((node *)allnode->get(start));

   while (!s->empty()) {    
      current = (node *)s->dequeue();
	  curr_no = current->nodenum();
	  parent = current->parent();
	  child = 0;
   
      for (int i=1 ; i <= max ; i++) {
         if (*(int *)solu->get(curr_no,i) ==0 || curr_no == i || parent == i) continue;
		 childptr = (node *)allnode->get(i);
		 treecost = treecost + *(int *)cost->get( curr_no, i );
         s->enqueue(childptr);
		 child++;
		 childptr->prnt = curr_no;
      }
	  if ((child == 0) && (current->type == NORMALNODE) && (parent != -1)) {
		 s->enqueue(allnode->get(parent));
		 cout << "<" << curr_no << "," << parent << "> be removed";
		 solu->put( current->parent(), curr_no, new int (0) );
		 solu->put( curr_no, current->parent(), new int (0) );
		 treecost = treecost - *(int *)cost->get( curr_no, parent );
		 continue;
	  }
	  current->children = child;
   }
   delete s;
   return treecost;
}

void usage ( )
{
   cout << "Usage: kmb <filename>";   
   exit (1); 
}
