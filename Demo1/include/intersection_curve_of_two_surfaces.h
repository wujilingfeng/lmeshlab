#ifndef INTERSECTION_CURVE_OF_SURFACES_H_
#define INTERSECTION_CURVE_OF_SURFACES_H_

#include<intersection_of_two_polygons.h>
#include<Mesh_IO/Mesh_IO.h>
#include<Mesh/lib_cell_iterator.h>
#define quote lib_cell_quote
#ifdef __cplusplus
extern "C" {
#endif
//#include<tools_node.h>

typedef struct SCrossover_Point
{//联合id
    template_c* c;
    template_v* v;
    template_f* f;
    double coordinates[3];

}SCrossover_Point;
typedef struct Crossover_Point {
    SCrossover_Point scp[2]; 
    void* prop;

}Crossover_Point;
static inline void crossover_point_init(Crossover_Point* cp)
{
    for(int i=0;i<2;i++)
    {
        cp->scp[i].c=NULL;
        cp->scp[i].v=NULL;
        cp->scp[i].f=NULL;
        memset(cp->scp[i].coordinates,0,sizeof(double)*3);
    } 
    
    cp->prop=NULL;

}
static inline void crossover_point_free(Crossover_Point* cp)
{
    free(cp);
}
typedef struct Mesh2_Crossover_Point{
    Int_RB_Tree* v2p,*f2p,*c2p;  
}Mesh2_Crossover_Point;
static inline void mesh2_crossover_point_init(Mesh2_Crossover_Point* mcp)
{
    mcp->v2p=(Int_RB_Tree*)malloc(sizeof(Int_RB_Tree));
    int_rb_tree_init(mcp->v2p);
    mcp->f2p=(Int_RB_Tree*)malloc(sizeof(Int_RB_Tree));
    int_rb_tree_init(mcp->f2p);
    mcp->c2p=(Int_RB_Tree*)malloc(sizeof(Int_RB_Tree));
    int_rb_tree_init(mcp->c2p);

}
static inline  void mesh2_crossover_point_free(Mesh2_Crossover_Point*mcp)
{
    int_rb_tree_free(mcp->v2p);
    int_rb_tree_free(mcp->f2p);
    int_rb_tree_free(mcp->c2p);
    mcp->v2p=NULL;mcp->f2p=NULL;mcp->c2p=NULL;
} 
static inline double** get_intersection_points_from_two_cells(template_c* c1,template_c* c2)
{
    int len1=c1->vertices_size,len2=c2->vertices_size;
    double**poly1=(double**)malloc(sizeof(double*)*len1);
    double**poly2=(double**)malloc(sizeof(double*)*len2);

    for(int i=0;i<len1;i++)
    {
        poly1[i]=c1->vertices[i]->point;

    }
    for(int i=0;i<len2;i++)
    {
        poly2[i]=c2->vertices[i]->point;
    }
    double **re=get_intersection_points_from_two_polygons(poly1,len1,poly2,len2);
    free(poly1);free(poly2); 
    
    return re;
}


//template_v** get_intersection_vertices_from_two_cells(template_c* c1,template_c* c2);
//make sure prop not be occupied
Node* simplify_node_of_two_nodes(Node* n1,Node *n2,int* flag);
//Node* get_intersection_lines_segments_of_two_nodes(Node* n1,Node* n2,Mesh* m);

Mesh* get_intersection_lines_of_two_nodes(Node* n1,Node* n2,Mesh2_Crossover_Point* mcp1,Mesh2_Crossover_Point*mcp2);
Mesh*  get_intersection_lines_of_two_nodesn(Node* n11,Node* n22,Mesh2_Crossover_Point*mcp1,Mesh2_Crossover_Point*mcp2);

// mcp1储存m1到m的信息
//mcp2储存m2到m的信息

static Mesh*  get_intersection_lines_of_two_meshs(Mesh* m1,Mesh* m2,Mesh2_Crossover_Point* mcp1,Mesh2_Crossover_Point*mcp2)
{
    Mesh* re=NULL;
    Node* n1=NULL,*n2=NULL;
   
    for(auto it=m1->c_begin(m1);it!=m1->c_end(m1);it++)
    {
        n1=node_overlying(n1,quote(it));
    } 
    for(auto it=m2->c_begin(m2);it!=m2->c_end(m2);it++)
    {
        n2=node_overlying(n2,quote(it));
    }


    re=get_intersection_lines_of_two_nodesn(n1,n2,mcp1,mcp2);
    free_node(n1);free_node(n2);
    return re;
  //  interset_jiance(m);
   // printf("end here\n"); 
}
#ifdef __cplusplus
}
#endif

#endif