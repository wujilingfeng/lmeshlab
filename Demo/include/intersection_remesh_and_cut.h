#ifndef INTERSECTION_REMESH_AND_CUT_H_
#define INTERSECTION_REMESH_AND_CUT_H_
#include<intersection_curve_of_two_surfaces.h>
#include<subdivision_of_polygon.h>
#include<tool/libcell_tools_algorithm.h>





void adjust_mesh_topology(Mesh* m);
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
Mesh* my_intersection_remesh(Mesh* m1,Mesh2_Crossover_Point*mcp,Mesh*m);



//Node* my_create_cell_required_vertices(template_c* c,Mesh* m1,Mesh* m,Mesh* );
// void my_intersection_cut(Mesh* m1,Mesh* m);






#endif 