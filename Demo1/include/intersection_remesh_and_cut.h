#ifndef INTERSECTION_REMESH_AND_CUT_H_
#define INTERSECTION_REMESH_AND_CUT_H_
#include<intersection_curve_of_two_surfaces.h>
#include<subdivision_of_polygon.h>
#include<tool/libcell_tools_algorithm.h>



//调整拓扑
//m是要调整的分割线的网格
//m调整以后，mcp1 mcp2也要调整

void adjust_mesh_topology(Mesh* m,Mesh2_Crossover_Point* mcp1,Mesh2_Crossover_Point* mcp2);
//
//给一个cell,返回切割后的多边形区域
// m 是c所在的mesh,
// nm是切割边界的mesh
//mcp 是m到nm的信息容器
//vertices 是辅助链表，用来储存某些值，然后释放内存
Node* my_get_split_areas_from_one_cell(template_c*c ,Mesh* m, Mesh2_Crossover_Point* mcp,Mesh* nm,Node** vertices);

//m1是网格
//m是切割曲线
//返回m1重建后的网格
//tree2储存m的点到返回的mesh的点映射
// tree3储存返回mesh的cell到n1的cell的映射
// mcp 是m1到m的映射
//需要修改的地方: 还要储存返回mesh的点到n1的点的映射


Mesh* my_intersection_remesh(Node* n1,Mesh2_Crossover_Point*mcp,Mesh*m,Int_RB_Tree*tree2,Int_RB_Tree* tree3);

//m要切割的网格
//nm切割线
//tree储存nm到m的映射
//tree3是m的某个属性映射(因为切割后需要修改,所以传入)

void my_intersection_cut(Mesh* m,Mesh* nm,Int_RB_Tree* tree,Int_RB_Tree* tree3);

//Node* my_create_cell_required_vertices(template_c* c,Mesh* m1,Mesh* m,Mesh* );






#endif 