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

1、整体指引：
  1)使用一键式脚本编译、链接并打包压缩，如果编译失败请自行解决编译问题；
  2)如果编译成功会在bin/目录下生成可执行二进制文件"future_net"；
  3)使用如下格式调用并调试程序"./future_net /xxx/topo.csv /xxx/demand.csv /xxx/result.csv"，其中topo.csv和demand.csv是输入文件，result.csv是输出文件;
  4)调试成功后到竞赛官网提交SDK-gcc/目录下的压缩包"future_net.tar.gz"，稍后查询成绩。

2、shell脚本说明：
  执行此脚本可以一键编译、链接、打包。如果编译和链接正确，会在bin/下生成future_net二进制文件，并按照大赛要求生成二进制文件与代码的压缩打包文件存处于SDK/下。
  注意：
    1)shell脚本会删除bin/和build/目录，以及这两个目录下的所有文件和目录。请不要在此保存你的任何文档；
    2)如果想使用shell脚本一键功能，请保持SDK-gcc/目录下所有内容的完整，请不要修改任何目录名和文件名，并保持各目录和文件的位置关系不变。

3、SDK代码说明：
  我们已经提供了保姆式的服务，你只需要做：
  1)实现route.cpp文件中的search_route接口；
  2)依次调用record_result将路径结果写入缓冲区;
  3)如果计算结果为没有路径，则不需要调用record_result接口即可直接输出NA。
  SDK已经实现了读取文件、按要求格式写文件以及打印开始和结束时间的功能。
  注意：读取文件功能是指，将图的信息文件和路径信息文件按行读取到内存，其在内存中的存储格式仍是字符串格式。因为这些信息以什么格式存储涉及到算法设计，这样做是为了不禁锢你的思路。

4.参数的顺序从左到右为topo(图的数据G)、demand(需要计算的路径信息:s,t,V')、result(计算结果)，文件具体格式请参考赛题描述。


注意
1.为了在windows进行开发对sdk进行的修改，上传的时候需要检查恢复回去。


Debug log
1.ReadData              in case0   5ms
2.Read Data + DJS of s  in case1   110ms

参考文献
