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
#include<export_obj.h>
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

Node*  mytest_m(Mesh* m)
{
    printf("m num c:%d\n",m->num_c(m));
    Node* node=NULL,*node1=NULL;
    int flag=0;
    template_c* c=quote(m->c_begin(m));
    node=node_overlying(node,c);
    node1=node_overlying(node1,c);
    c->prop=&flag;
    printf("here\n");
    while(node1!=NULL)
    {
        Node* temp_n=NULL;
        for(Node* nit=node1;nit!=NULL;nit=(Node*)(nit->Next))
        {
            template_c* c=(template_c*)(nit->value);
            for(auto chfit=m->chf_begin(m,*c);chfit!=m->chf_end(m,*c);chfit++)
            {
                template_c* c1=m->s_opposite_halfface(quote(chfit))->cell;
                if(c1!=NULL&&c1->prop==NULL)
                {
                    temp_n=node_overlying(temp_n,c1);
                    node=node_overlying(node,c1);
                    c1->prop=&flag;
                }
            }
        } 
        free_node(node1);
        node1=temp_n;
    }
    printf("here\n");
    printf("node size:%d\n",node_size(node));
    for(Node* nit=node;nit!=NULL;nit=(Node*)(nit->Next))
    {
        template_c*c=(template_c*)(nit->value);
        c->prop=NULL;

    }
    //free_node(node);

    return node;
}
void my_test_show_cat_mesh_cells(Viewer_World* vw,Mesh*m)
{
    Node* n=vw->create_something(vw,"Faces");
    Viewer_Something*vs=(Viewer_Something*)(n->value);
    Viewer_Faces* vf=(Viewer_Faces*)(vs->evolution);
     
    vf->Data_rows=m->num_v(m);
    vf->Data_index_rows=m->num_c(m);
    get_data_from_2dim_cell(m,&(vf->Data),&(vf->Data_index)); 

    vf->color_rows=vf->Data_index_rows;
    vf->set_color(vf,0.4,0.6,0.7,1.0);

    vf->normal_rows=vf->Data_index_rows;
    vf->compute_normal(vf); 
    free_node(n);

    n=vw->create_something(vw,"Texture");
    vs=(Viewer_Something*)(n->value);
    Viewer_Texture* vt=(Viewer_Texture*)(vs->evolution);
    vf->texture=vs;
    SET_STR_VALUE(vt->image_file,"Cat_diffuse.jpg");
    float* texcoords=(float*)malloc(sizeof(float)*9*m->num_c(m)); 
    int i=0;
    for(auto it=m->c_begin(m);it!=m->c_end(m);it++)
    {
        texcoords[9*i+0]=4;
        OBJ_CELL_PROP*ofps=(OBJ_CELL_PROP*)(quote(it)->prop);
        for(int j=0;j<4;j++)
        {
            texcoords[9*i+1+j*2+0]=ofps[j].vt[0];texcoords[9*i+1+j*2+1]=ofps[j].vt[1];    
        }

        i++;
    }
    vt->each_face_texture_coord=texcoords;
    //vt->image_file=(char*)malloc(sizeof(char)*30); 
    // strcpy(vt->image_file,"Cat_diffuse.jpg");


    //free_node(n);  
}
void test_show_mesh_cells(Viewer_World* vw,Mesh*m)
{
    Node* n=vw->create_something(vw,"Faces");
    Viewer_Something*vs=(Viewer_Something*)(n->value);
    Viewer_Faces* vf=(Viewer_Faces*)(vs->evolution);
     
    vf->Data_rows=m->num_v(m);
    vf->Data_index_rows=m->num_c(m);
    get_data_from_2dim_cell(m,&(vf->Data),&(vf->Data_index));
    

    // Node* node=mytest_m(m);
    // vf->Data_index_rows=node_size(node);
    // vf->Data=(float*)malloc(sizeof(float)*vf->Data_rows*3); 
    // Int_RB_Tree* tree=(Int_RB_Tree*)malloc(sizeof(Int_RB_Tree));
    // int_rb_tree_init(tree);
    // int f_size=0;
    // for(Node* nit=node;nit!=NULL;nit=(Node*)(nit->Next))
    // {
    //     template_c* c=(template_c*)(nit->value);
    //     f_size+=c->vertices_size+1;
    // }
    // vf->Data_index=(unsigned int*)malloc(sizeof(unsigned int)*f_size);
    // int* temp_v_id=(int*)malloc(sizeof(int)*vf->Data_rows);
    // int i=0;
    // for(auto vit=m->v_begin(m);vit!=m->v_end(m);vit++)
    // {
    //     for(int j=0;j<3;j++)
    //     {
    //         vf->Data[i*3+j]=quote(vit)->point[j];
    //     }
    //     temp_v_id[i]=i;
    //     tree->insert(tree,quote(vit)->id,&temp_v_id[i]);
    //     i++;
    // }
    // i=0;
    // for(Node* nit=node;nit!=NULL;nit=(Node*)(nit->Next))
    // {
    //     template_c* c=(template_c*)(nit->value);
    //     vf->Data_index[i]=c->vertices_size;
    //     i++;
    //     for(auto it1=m->cv_begin(m,*c);it1!=m->cv_end(m,*c);it1++)
    //     {
    //         vf->Data_index[i]=*((int*)(tree->find(tree,quote(it1)->id)));
    //         i++;
    //     }         
    // }    
    // free(temp_v_id);
    // int_rb_tree_free(tree);
    // free_node(node);
    
    vf->color_rows=vf->Data_index_rows;
    vf->set_color(vf,0.4,0.6,0.7,1.0);

    vf->normal_rows=vf->Data_index_rows;
    vf->compute_normal(vf);
    
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
//
//  

void test_intersection_of_two_surfaces()
{

    Mesh m1,m2,m4;
    Mesh_init(&m1);Mesh_init(&m2);Mesh_init(&m4);
    //_ReadOff_(&m4,"bone_scan1.off",3);
    _ReadOff_(&m1,"cylinder.off",3);
    _ReadOff_(&m2,"texture_BA.off",3);

    Node* n1=NULL,*n2=NULL;
    printf("begin\n"); 
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
    Mesh* m3=get_intersection_lines_of_two_nodesn(n1,n2,mcp1,mcp2);

    printf("m3 numv:%d\n",m3->num_v(m3)); 
    adjust_mesh_topology(m3,mcp1,mcp2);

    //测试重新mesh后的区域
    printf("测试;%d %d\n",mcp1->c2p->size,m1.num_f(&m1));
    Int_RB_Tree* tree=(Int_RB_Tree*)malloc(sizeof(Int_RB_Tree)),*tree1=(Int_RB_Tree*)malloc(sizeof(Int_RB_Tree));
    int_rb_tree_init(tree);int_rb_tree_init(tree1);
    Mesh* m5=my_intersection_remesh(n2,mcp2,m3,tree,tree1);

    my_intersection_cut(m5,m3,tree,tree1); 
    //mytest_m(m5);
    //printf("m5 numc:%d\n",m5->num_c(m5));
    int_rb_tree_free(tree); 
    printf("end\n");

    //
    // 可视化
    Viewer_World_Manager vwm;
    viewer_world_manager_init(&vwm);

    Viewer_World *vw=vwm.create_world(&vwm,NULL);

    add_default_somethings(vw);
    // m3*0.00001;
    // (*m5)*0.00001; 
    test_show_mesh_lines(vw,m3);
    test_show_mesh_cells(vw,m5);
    //test_show_mesh_lines(vw,&m1);

    // Mesh m6;
    // Mesh_init(&m6);
    // _ReadOff_(&m6,"texture_BA.off",3);
    // test_show_mesh_cells(vw,&m6);
    // Mesh_free(&m6);
    //get_intersection_lines_of_two_nodesn(n1,n2,m);
    Viewer_Opengl_Interpreter voi;
    viewer_opengl_interpreter_initn(&voi,&vwm);
    voi.interpreter(&voi);
    viewer_opengl_interpreter_free(&voi);
    viewer_world_manager_free(&vwm);
    //可视化

    free_node(n1);free_node(n2);
    Mesh_free(&m1);Mesh_free(&m2);Mesh_free(m3);free(m3);
}
// 中原动力的测试函数
void my_test(Mesh* m,double **ps,int len )
{
    Node* n1=NULL,*n2=NULL;
    // printf("begin\n"); 
    for(auto it=m->c_begin(m);it!=m->c_end(m);it++)
    {
        n1=node_overlying(n1,quote(it));
    } 
    Mesh m1;
    Mesh_init(&m1);
   // printf("here\n");
    double **V=(double**)malloc(sizeof(double*)*len*2);
    for(int i=0;i<len;i++)
    {
        V[i]=(double*)malloc(sizeof(double)*3);
        V[i][0]=ps[i][0];V[i][1]=ps[i][1];V[i][2]=-110.0;
        V[i+len]=(double*)malloc(sizeof(double)*3);
        V[i+len][0]=ps[i][0];V[i+len][1]=ps[i][1];V[i+len][2]=-50.0; 
    }
    int ** C=(int**)malloc(sizeof(int*)*len*2);
    for(int i=0;i<len;i++)
    {
        C[2*i+0]=(int*)malloc(sizeof(int)*4);
        C[2*i+0][0]=3;
        C[2*i+0][1]=i;C[2*i+0][2]=(i+1)%len;C[2*i+0][3]=i+len;
/////***********************
        C[2*i+1]=(int*)malloc(sizeof(int)*4);
        C[2*i+1][0]=3;
        C[2*i+1][1]=(i+1)%len+len;C[2*i+1][2]=i+len;C[2*i+1][3]=(i+1)%len;
    }

    int nums[3]={len*2,0,len*2};
    _ReadArray_(&m1,V,NULL,C,nums,3,2);


   
    for(auto it=m1.c_begin(&m1);it!=m1.c_end(&m1);it++)
    {
        n2=node_overlying(n2,quote(it));
    } 


    printf("n1 size:%d\n",node_size(n1));
    int flag=0;
    Node* nn1=simplify_node_of_two_nodes(n2,n1,&flag);
    printf("nn1 size:%d\n",node_size(nn1));
    printf("begin\n");
   
    export_obj_from_node(nn1,"test.obj","texture_BA.mtl");

    Mesh2_Crossover_Point* mcp1=(Mesh2_Crossover_Point*)malloc(sizeof(Mesh2_Crossover_Point));
    Mesh2_Crossover_Point* mcp2=(Mesh2_Crossover_Point*)malloc(sizeof(Mesh2_Crossover_Point));
    mesh2_crossover_point_init(mcp1);mesh2_crossover_point_init(mcp2);

    Mesh* m3=get_intersection_lines_of_two_nodesn(nn1,n2,mcp1,mcp2);

    adjust_mesh_topology(m3,mcp1,mcp2);

    //测试重新mesh后的区域
    // printf("测试;%d %d\n",mcp1->c2p->size,m1.num_f(&m1));
    Int_RB_Tree* tree=(Int_RB_Tree*)malloc(sizeof(Int_RB_Tree)),*tree1=(Int_RB_Tree*)malloc(sizeof(Int_RB_Tree));
    int_rb_tree_init(tree);int_rb_tree_init(tree1);
    Mesh* m5=my_intersection_remesh(nn1,mcp1,m3,tree,tree1);

    my_intersection_cut(m5,m3,tree,tree1); 


    
    //mytest_m(m5);
    //printf("m5 numc:%d\n"m5->num_c(m5));
    int_rb_tree_free(tree1);
    int_rb_tree_free(tree); 
    printf("可视化\n");

    // Mesh obj_mesh;
    // Mesh_init(&obj_mesh);
    // obj_mesh.dimension=2;
    // obj_mesh.simplex=0;

    // _ReadObj_(&obj_mesh,"12221_Cat_v1_l3.obj"); 

// 可视化
    Viewer_World_Manager vwm;
    viewer_world_manager_init(&vwm);

    Viewer_World *vw=vwm.create_world(&vwm,NULL);

    add_default_somethings(vw);
    // m3*0.00001;
    // (*m5)*0.00001; 
    // test_show_mesh_lines(vw,m3);
    test_show_mesh_cells(vw,m5);
    // my_test_show_cat_mesh_cells(vw,&obj_mesh);

    // test_show_mesh_lines(vw,&m1);

    // Mesh m6;
    // Mesh_init(&m6);
    // _ReadOff_(&m6,"texture_BA.off",3);
    // test_show_mesh_cells(vw,&m6);
    // Mesh_free(&m6);
    //get_intersection_lines_of_two_nodesn(n1,n2,m);
    Viewer_Opengl_Interpreter voi;
    viewer_opengl_interpreter_initn(&voi,&vwm);
    voi.interpreter(&voi);
    viewer_opengl_interpreter_free(&voi);
    viewer_world_manager_free(&vwm);
    //可视化

    // Mesh_free(&obj_mesh);

    Mesh_free(&m1);

    // for(auto it=m2.c_begin(&m2);it!=m2.c_end(&m2);it++)
    // {
    //     n2=node_overlying(n2,quote(it));
    // }

    // Mesh2_Crossover_Point* mcp1=(Mesh2_Crossover_Point*)malloc(sizeof(Mesh2_Crossover_Point));
    // Mesh2_Crossover_Point* mcp2=(Mesh2_Crossover_Point*)malloc(sizeof(Mesh2_Crossover_Point));
    // mesh2_crossover_point_init(mcp1);mesh2_crossover_point_init(mcp2);
    // Mesh* m3=get_intersection_lines_of_two_nodesn(n1,n2,mcp1,mcp2); 

}

int main()
{
    
    // test_intersection_of_two_surfaces();
    Mesh mesh;
    Mesh_init(&mesh);
    // _ReadOff_(&mesh,"texture_BA.off",3);
    _ReadObj_(&mesh,"texture_BA.obj");

    // for(auto cit=mesh.c_begin(&mesh);cit!=mesh.c_end(&mesh);cit++)
    // {
    //     OBJ_CELL_PROP* ocps=(OBJ_CELL_PROP*)(quote(cit)->prop);


    // }

    double* quad[4]={NULL};
    quad[0]=(double*)malloc(sizeof(double)*3);
    quad[1]=(double*)malloc(sizeof(double)*3);
    quad[2]=(double*)malloc(sizeof(double)*3);
    quad[3]=(double*)malloc(sizeof(double)*3);
  



    quad[0][0]=0.0;quad[0][1]=30.0;quad[0][2]=-110.0;
    quad[1][0]=20.0;quad[1][1]=30.0;quad[1][2]=-110.0; 
    quad[2][0]=20.0;quad[2][1]=50.0;quad[2][2]=-110.0;
    quad[3][0]=0.0;quad[3][1]=50.0;quad[3][2]=-110.0;
    // quad[4][0]=0.0;quad[4][1]=30.0;quad[4][2]=-50.0;
    // quad[5][0]=20.0;quad[5][1]=30.0;quad[5][2]=-50.0; 
    // quad[6][0]=0.0;quad[6][1]=50.0;quad[6][2]=-50.0;
    // quad[7][0]=20.0;quad[7][1]=50.0;quad[7][2]=-50.0;



    my_test(&mesh,quad,4);
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
    Mesh_free(&mesh);
    printf("enddddd\n");

}
