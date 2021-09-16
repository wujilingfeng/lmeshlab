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
Node* my_get_split_areas_from_one_cell(template_c*c ,Mesh* m, Mesh2_Crossover_Point* mcp,Mesh* nm);

// Node* my_create_cell_required_vertices(template_c* c,Mesh* m1,Mesh* m,Mesh* );
// Mesh* my_intersection_remesh(Mesh* m1,Mesh*m);
// void my_intersection_cut(Mesh* m1,Mesh* m);






#endif 