#include<stdio.h>
#include<Viewer_Interpreter/Viewer_OpenGL_Interpreter.h>
#include<Mesh_IO/Mesh_IO.h>
#include<tool/libcell_tools_formats.h>
#include<tool/libcell_tools_view.h>
#include "subdivision_of_polygon.h"
#include "intersection_of_two_polygons.h"
#include "intersection_curve_of_two_surfaces.h"
#include "intersection_remesh_and_cut.h"
#include<tool/viewer_tools.h>
#include<tool/libcell_tools_view.h>
#include<Arcroll.h>

#include<Mesh/lib_cell_iterator.h>
#include<Mesh/Mesh_Operator.h>

#include<macros.h>

#define Matrix4x4 Viewer_Matrix4x4_


void add_default_somethings(Viewer_World* vw)
{
    Node* n=vw->create_something(vw,"Camera");
    Viewer_Something*vs=(Viewer_Something*)(n->value);
    Viewer_Camera* vc=(Viewer_Camera*)(vs->evolution);
    vc->is_using=1;
    Matrix4x4 *p=Projection(M_PI/3.0f,
        (float)(vw->g_info->resolution[0])/(float)(vw->g_info->resolution[1]),0.5f,200.0f);
    p->copy_data(vc->Proj,p);
    Matrix4x4_free(p);
    free_node(n);
    
    n=vw->create_something(vw,"Intera");
    vs=(Viewer_Something*)(n->value);
    Viewer_Intera*vi=(Viewer_Intera*)(vs->evolution);
    Viewer_Arcroll*va=(Viewer_Arcroll*)malloc(sizeof(Viewer_Arcroll));
    viewer_arcroll_init(va);
    va->mc=vc;
    va->vw=vw;
    vi->representation=(void*)va;
    vi->cursor_position_callback=viewer_Arcroll_cursor_position_callback;
    vi->scroll_callback=viewer_Arcroll_scroll_callback;
    vi->mouse_button_callback=viewer_Arcroll_mouse_button_callback;
    vi->key_callback=viewer_Arcroll_key_callback; 
    free_node(n);
}
void test_show()
{
    Viewer_World_Manager vwm;
    viewer_world_manager_init(&vwm);

    Viewer_World *vw=vwm.create_world(&vwm,NULL);

    add_default_somethings(vw);
 
    Node* n=vw->create_something(vw,"Edges");
    Viewer_Something* vs=(Viewer_Something*)(n->value);
    Viewer_Edges* ve=(Viewer_Edges*)(vs->evolution);
    ve->Data_rows=5;
    ve->Data_index_rows=5;

    viewer_tools_fill_data("%f",ve->Data_rows*3,&(ve->Data),
    0.0,0.0,0.0,
    1.0,0.0,0.0,
    2.0,1.0,0.0,
    1.9,2.0,0.0,
    0.7,1.4,0.0);

    viewer_tools_fill_data("%u",ve->Data_index_rows*2,&(ve->Data_index),
    0,1,
    1,2,
    2,3,
    3,4,
    4,0);
    ve->color_rows=ve->Data_index_rows;
    ve->set_color(ve,1.0,0.0,0.0,1.0);
    free_node(n);

    n=vw->create_something(vw,"Edges");
    vs=(Viewer_Something*)(n->value);
    ve=(Viewer_Edges*)(vs->evolution);
    ve->Data_rows=5;
    ve->Data_index_rows=5;
    viewer_tools_fill_data("%f",ve->Data_rows*3,&(ve->Data),
        1.2,1.1,0.0,
        1.6,0.2,0.0,
        2.2,-1.8,0.0,
        3.1,0.9,0.0,
        2.7,1.4,0.0);

    viewer_tools_fill_data("%u",ve->Data_index_rows*2,&(ve->Data_index),
        0,1,
        1,2,
        2,3,
        3,4,
        4,0);
    ve->color_rows=ve->Data_index_rows;
    ve->set_color(ve,1.0,0.0,0.0,1.0);  
    n=vw->create_something(vw,"Points");
    vs=(Viewer_Something*)(n->value);
    Viewer_Points* vp=(Viewer_Points*)(vs->evolution);
    vp->Data_rows=2;
    viewer_tools_fill_data("%f",vp->Data_rows*3,&(vp->Data),
        1.476923,0.476923,0.0,
        1.974510,1.254902,0.0);
    vp->pointsize=10.0; 
    vp->set_color(vp,0.0,1.0,0.0,1.0);
    free_node(n); 
    //Viewer_World *vw1=vwm.create_world(&vwm,"second windows");

    Viewer_Opengl_Interpreter voi;
    viewer_opengl_interpreter_initn(&voi,&vwm);
    voi.interpreter(&voi);
    viewer_opengl_interpreter_free(&voi);
    viewer_world_manager_free(&vwm);  
}
void test_show_mesh_lines(Viewer_World* vw,Mesh* m)
{
    Node* n=vw->create_something(vw,"Edges");
    Viewer_Something*vs=(Viewer_Something*)(n->value);
    Viewer_Edges* ve=(Viewer_Edges*)(vs->evolution);
    ve->Data_rows=m->num_v(m);
    ve->Data_index_rows=m->num_f(m);
    get_lines_data_from_2dim_cell(m,&(ve->Data),&(ve->Data_index));
    ve->color_rows=ve->Data_index_rows;
    ve->set_color(ve,1.0,0.0,0.6,1.0); 
    free_node(n);

}
void test_show_mesh_cells(Viewer_World* vw,Mesh*m)
{
    Node* n=vw->create_something(vw,"Faces");
    Viewer_Something*vs=(Viewer_Something*)(n->value);
    Viewer_Faces* vf=(Viewer_Faces*)(vs->evolution);
    vf->Data_rows=m->num_v(m);
    vf->Data_index_rows=m->num_c(m);
    get_data_from_2dim_cell(m,&(vf->Data),&(vf->Data_index));
    vf->color_rows=vf->Data_index_rows;
    vf->set_color(vf,0.9,0.6,0.7,1.0); 
    
    free_node(n); 

}
void test_subdivision()
{
    double vs[6][3]={{0,0,0},{1,1,0},{2,1,0},{2,10,0},{1,1.2,0},{-1,1,0}};
    
    int len=6;
    double** v=(double**)malloc(sizeof(double*)*len);
    
    for(int i=0;i<len;i++)
    {
        v[i]=(double*)malloc(sizeof(double)*3);
        v[i][0]=vs[i][0];v[i][1]=vs[i][1];v[i][2]=vs[i][2];
        printf("%lf %lf %lf\n",v[i][0],v[i][1],v[i][2] );
    }
    int **s=(int**)malloc(sizeof(int*)*(len-2));
    for(int i=0;i<len-2;i++)
    {
        s[i]=(int*)malloc(sizeof(int)*3);
    }



    // //*****************************
    Matrix4x4* m=(Matrix4x4*)malloc(sizeof(Matrix4x4));
    Matrix4x4_init_double(m);
    // double * data=(double*)(m->data);
    // data[0*4+0]=1/sqrt(6);data[0*4+1]=1/sqrt(3);data[0*4+2]=1/sqrt(2);
    // data[1*4+0]=2/sqrt(6);data[1*4+1]=-1/sqrt(3);data[1*4+2]=0;
    // data[2*4+0]=1/sqrt(6);data[2*4+1]=1/sqrt(3);data[2*4+2]=-1/sqrt(2);
    // for(int i=0;i<len;i++)
    // {
    //     double temp[3]={0,0,0};  
    //     for(int j=0;j<3;j++)
    //     {
    //         for(int k=0;k<3;k++)
    //         {
    //             temp[j]+=data[j*4+k]*v[i][k];
    //         }
    //     }
    //     v[i][0]=temp[0];v[i][1]=temp[1];v[i][2]=temp[2]; 
    // }
//*********************
    subdivision_of_polygon(v,len,s); 

    for(int i=0;i<len-2;i++)
    {

        printf("%d %d %d\n",s[i][0],s[i][1],s[i][2] );
    }
    Matrix4x4_free(m);
    // if(is_subdivision_of_polygon_acute_angle(v,len,0))
    // {
    //     printf("shi\n");
    // }
    for(int i=0;i<len-2;i++)
    {
        free(s[i]);

    }

    free(s);
    free(v);
}
void test_intersection_of_two_polygons()
{
    int len1=5,len2=5;
    double **p1=(double**)malloc(sizeof(double*)*len1),**p2=(double**)malloc(sizeof(double*)*len2);
    double tep1[5][3]={{0,0,0},{1,0,0},{2,1,0},{1.9,2,0},{0.7,1.4,0}},tep2[5][3]={{1.2,1.1,0},{1.6,0.2,0},{2.2,-1.8,0},{3.1,0.9,0},{2.7,1.4,0}};
    for(int i=0;i<len1;i++)
    {
        p1[i]=(double*)malloc(sizeof(double)*3);
        p1[i][0]=tep1[i][0];p1[i][1]=tep1[i][1];p1[i][2]=tep1[i][2];
        printf("%lf %lf %lf\n",p1[i][0],p1[i][1],p1[i][2]);
    }
    for(int i=0;i<len2;i++)
    {
        p2[i]=(double*)malloc(sizeof(double)*3);
        p2[i][0]=tep2[i][0];p2[i][1]=tep2[i][1];p2[i][2]=tep2[i][2];
         printf("%lf %lf %lf\n",p2[i][0],p2[i][1],p2[i][2]);
    }
    
    double ** ts=get_intersection_points_from_two_polygons(p1,5,p2,5);
    if(ts==NULL)
    {
        printf("cowuuu\n");
    }
    // printf("%lf %lf %lf\n",ts[1][0],ts[1][1],ts[1][2]);
     
    for(int i=0;i<len1;i++)
    {
        free(p1[i]);

    } 
    for(int i=0;i<len2;i++)
    {
        free(p2[i]);
    }
    free(p1);free(p2);
}

void test_intersection_of_two_surfaces()
{

    Mesh m1,m2,m3,m4;
    Mesh_init(&m1);Mesh_init(&m2);Mesh_init(&m3);Mesh_init(&m4);
    //_ReadOff_(&m4,"bone_scan1.off",3);
    _ReadOff_(&m1,"sphere.off",3);
    _ReadOff_(&m2,"sphere1.off",3);

    Node* n1=NULL,*n2=NULL;
   
    for(auto it=m1.c_begin(&m1);it!=m1.c_end(&m1);it++)
    {
        n1=node_overlying(n1,quote(it));
    } 
    for(auto it=m2.c_begin(&m2);it!=m2.c_end(&m2);it++)
    {
        n2=node_overlying(n2,quote(it));
    }

    Mesh2_Crossover_Point* mcp1=(Mesh2_Crossover_Point*)malloc(sizeof(Mesh2_Crossover_Point));
    Mesh2_Crossover_Point* mcp2=(Mesh2_Crossover_Point*)malloc(sizeof(Mesh2_Crossover_Point));
    mesh2_crossover_point_init(mcp1);mesh2_crossover_point_init(mcp2);


    //get_intersection_lines_of_two_nodes(n1,n2,&m3,mcp1,mcp2); 
    get_intersection_lines_of_two_nodesn(n1,n2,&m3,mcp1,mcp2);
    adjust_mesh_topology(&m3);
    printf("m3 numv:%d\n",m3.num_v(&m3)); 

    //测试重新mesh后的区域
    printf("测试;%d\n",mcp1->c2p->size);

    Mesh* m5=my_intersection_remesh(&m2,mcp2,&m3);
    printf("m5 numc:%d\n",m5->num_c(m5));
     
    // for(auto it=m2.c_begin(&m2);it!=m2.c_end(&m2);it++)
    // {
    //     my_get_split_areas_from_one_cell(quote(it),&m2,mcp2,&m3);
    // }
    // for(auto it=mcp2->c2p->begin(mcp2->c2p);it.it!=NULL;it++)
    // {
    //     template_c* c=m2.get_cellp(&m2,it.first);
    //     printf("c id:%d    ",c->id);
    //     my_get_split_areas_from_one_cell(c,&m2,mcp2,&m3);
    // }


    //
    // 可视化
    Viewer_World_Manager vwm;
    viewer_world_manager_init(&vwm);

    Viewer_World *vw=vwm.create_world(&vwm,NULL);

    add_default_somethings(vw);
    //m3*0.00001;
    test_show_mesh_lines(vw,&m3);
    test_show_mesh_cells(vw,m5);


    //get_intersection_lines_of_two_nodesn(n1,n2,m);
    Viewer_Opengl_Interpreter voi;
    viewer_opengl_interpreter_initn(&voi,&vwm);
    voi.interpreter(&voi);
    viewer_opengl_interpreter_free(&voi);
    viewer_world_manager_free(&vwm);
    //可视化

    free_node(n1);free_node(n2);
    Mesh_free(&m1);Mesh_free(&m2); Mesh_free(&m3);
}
int main()
{
 
    test_intersection_of_two_surfaces();
    int i=9,j=11,k=3;
    Node* node=NULL;
    node=node_overlying(node,&j);
    node=node_overlying(node,&i);
    //node=node_overlying(node,&k);
    printf("testlibo:%d\n",*((int*)(node_at(node,1)->value)));
    //test_show();

    // objtooff("texture_BA.obj");
    //test_subdivision();
    //test_show();
    //test_intersection_of_two_polygons();
   // printf("liboooooooooooo");
    printf("enddddd\n");

}
