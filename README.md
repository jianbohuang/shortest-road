# shortest-road ：求经过指定点集的最短路径问题
﻿流程
﻿1.上传SDK和资料在里面了，有进展注意及时保存。
﻿
﻿2.建立hjb-branch，测试好后再pull request
﻿
﻿3.初期先实现带环路径。再进行分支界限



知识点tips:
1.哈密尔顿图的充分条件：对于顶点个数大于2的图，如果图中任意两点度的和大于或等于顶点总数，那这个图一定是哈密顿图。闭合的哈密顿路径称作哈密顿圈，含有图中所有顶点的路径称作哈密顿路径。即Dirac定理

2.哈密尔顿构造方法：http://blog.csdn.net/pi9nc/article/details/9219971

3.Dijkstra +heap:http://blog.sina.com.cn/s/blog_476a25110100mag0.html

4.In integer programming, this refers to having a polynomial number of constraints. For example, look at the travelling salesman formulations. The linear form has an exponential number of "subtour elimination consgtraints," so it is not compact. The
quadratic assignment formulation is compact.

5.需要一个超时熔断机制

6。利用出入度限制对图预处理

7.结点优先权和限界函数时都用到了一个剩余所有结点最小出边和，就相当于把所有城市最便宜的一条路（边）费用加起来的值。

7.是否考虑求单源最短路径时，如果当前节点存在多条相等的最短路径 保留出入度多的那条？

题目相关：
1）图中所有权重均为[1，20]内的整数；
2）任一有向边的起点不等于终点；
3）连接顶点 A 至顶点 B 的有向边可能超过一条，其权重可能一样，也可能不一样；
4）该有向图的顶点不会超过 600 个，每个顶点出度(以该点为起点的有向边的数量)不超过
8；
5）V'中元素个数不超过 50；
6）从 s 到 t 的不成环有向路径 P 是指，P 为由一系列有向边组成的从 s 至 t 的有向连通路
径，且不允许重复经过任一节点；


参考文献
Ibaraki T. Algorithms for obtaining shortest paths visiting specified nodes[J]. Siam Review, 1973, 15(2): 309-317.
Gomes T, Marques S, Martins L, et al. Protected shortest path visiting specified nodes[C]//Reliable Networks Design and Modeling (RNDM), 2015 7th International Workshop on. IEEE, 2015: 120-127.
