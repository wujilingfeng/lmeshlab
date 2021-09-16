#ifndef INTERSECTION_REMESH_AND_CUT_H_
#define INTERSECTION_REMESH_AND_CUT_H_
#include<intersection_curve_of_two_surfaces.h>
#include<subdivision_of_polygon.h>
#include<tool/libcell_tools_algorithm.h>





void adjust_mesh_topology(Mesh* m);

Node* my_create_cell_required_vertices(template_c* c,Mesh* m1,Mesh* m,Mesh* );
Mesh* my_intersection_remesh(Mesh* m1,Mesh*m);
void my_intersection_cut(Mesh* m1,Mesh* m);






#endif 