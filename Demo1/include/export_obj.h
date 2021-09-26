#ifndef LIBCELL_EXPORT_OBJ_H_
#define LIBCELL_EXPORT_OBJ_H_
#include<Mesh_IO/Mesh_IO.h>
#include<Math/Viewer_Matrix4x4.h>
#define Matrix4x4 Viewer_Matrix4x4_
#define quote lib_cell_quote


static inline double * get_coordinate_in_another_coordinate_system(double* p,double* a1,double *a2,double*a3)
{
    Matrix4x4* m=(Matrix4x4*)malloc(sizeof(Matrix4x4));
    Matrix4x4_init_double(m);
    double* data=(double*)(m->data);
    data[0*4+0]=a1[0]; data[1*4+0]=a1[1];  data[2*4+0]=a1[2];   
    data[0*4+1]=a2[0]; data[1*4+1]=a2[1];  data[2*4+1]=a2[2];
    data[0*4+2]=a3[0]; data[1*4+2]=a3[1];  data[2*4+2]=a3[2];
    Matrix4x4* mi=m->inverse(m);
    if(mi==NULL)
    {
        Matrix4x4_free(m);
        return NULL;
    } 
    data=(double*)(mi->data);
    double * re=(double*)malloc(sizeof(double)*3); 
    memset(re,0,sizeof(double)*3);
    for(int i=0;i<3;i++)
    {
        re[i]=0;
       for(int j=0;j<3;j++)
       {

            re[i]+=data[i*4+j]*p[j];
       } 

    }
    Matrix4x4_free(m);
    Matrix4x4_free(mi);
    return re;
}
//return the triangle coordinate of p in p1 p2 p3
static inline double* get_triangle_coordinate(double* p,double * p1,double*p2,double* p3 )
{

    double a1[3]={p1[0]-p3[0],p1[1]-p3[1],p1[2]-p3[2]};
    double a2[3]={p2[0]-p3[0],p2[1]-p3[1],p2[2]-p3[2]};
    double pp[3]={p[0]-p3[0],p[1]-p3[1],p[2]-p3[2]};
    double* n=out_product(a1,a2);
    double* coord=get_coordinate_in_another_coordinate_system(pp,a1,a2,n);
    SAFE_FREE(n);
    if(coord==NULL)
    {
        return NULL;
    }
    double *re=(double*)malloc(sizeof(double)*3);
    re[0]=coord[0];re[1]=coord[1];
    re[2]=1-re[0]-re[1]; 
    SAFE_FREE(coord);
    return re;
}




void my_chuli_texture(Mesh* m,Mesh* m1,Int_RB_Tree* tree)
{
    OBJ_MESH_PROP* omp=(OBJ_MESH_PROP*)malloc(sizeof(OBJ_MESH_PROP));
    obj_mesh_prop_init(omp);
    OBJ_MESH_PROP* omp1=(OBJ_MESH_PROP*)(m1->prop);
    strcpy(omp->mtlname,omp1->mtlname);
    //printf("m1 mtl name:%s\n",omp1->mtlname);
    m->prop=omp;
    for(auto cit=m->c_begin(m);cit!=m->c_end(m);cit++)
    {

        template_c* c1=quote(cit);
        template_c* c=(template_c*)(tree->find(tree,c1->id));
        OBJ_CELL_PROP* ocp=(OBJ_CELL_PROP*)(c->prop);
        OBJ_CELL_PROP* ocp1=(OBJ_CELL_PROP*)malloc(sizeof(OBJ_CELL_PROP)*c1->vertices_size);
        c1->prop=ocp1; 

        for(int i=0;i<c1->vertices_size;i++)
        {
            obj_cell_prop_init(&ocp1[i]);
            strcpy(ocp1[i].name,ocp->name);
            double *coord=get_triangle_coordinate(c1->vertices[i]->point,c->vertices[0]->point,c->vertices[1]->point,c->vertices[2]->point);
            printf("coord %lf %lf %lf\n",coord[0],coord[1],coord[2]);
            for(int j=0;j<3;j++)
            {
                ocp1[i].vt[j]=coord[0]*ocp[0].vt[j]+coord[1]*ocp[1].vt[j]+coord[2]*ocp[2].vt[j];
            } 
            SAFE_FREE(coord); 
        }
    } 
}



void export_obj_from_node(Node* node,char* filename,char * mtlname)
{
#ifdef _WIN32
    FILE *outfile;
    fopen_s(&outfile,filename,"w");
#else
    FILE *outfile=fopen(filename,"w");
#endif
    if(!outfile)
    {
        printf("can't open this file\n"); 
        return ;
    }
    if(mtlname!=NULL)
    {
        fprintf(outfile,"mtllib %s\n",mtlname);
    }

    Int_RB_Tree* tree=(Int_RB_Tree*)malloc(sizeof(Int_RB_Tree));
    int_rb_tree_init(tree);
    Node* mtl_class=NULL;  

    int id=1;
    Node* vertices=NULL,*node_vt=NULL;
    for(Node* nit=node;nit!=NULL;nit=(Node*)(nit->Next))
    {
        template_c* c=(template_c*)(nit->value);
        if(mtlname!=NULL)
        {
            OBJ_CELL_PROP* ocp=(OBJ_CELL_PROP*)(c->prop); 
            Node* nit1=mtl_class;
            for(;nit1!=NULL;nit1=(Node*)(nit1->Next))
            {
                if(strcmp(ocp->name,(char*)(nit1->traits))==0)
                {break;} 
            }
            if(nit1!=NULL)
            {
                Node* node_mtl=(Node*)(nit1->value);
                node_mtl=node_overlying(node_mtl,c);
                nit1->value=node_mtl;
            } 
            else
            {
                Node* node_mtl=node_overlying(NULL,c);
                mtl_class=node_overlying(mtl_class,node_mtl);
                mtl_class->traits=ocp->name;
            }
        }
       
        for(int i=0;i<c->vertices_size;i++)
        {
            template_v*v=c->vertices[i];
            if(tree->find(tree,v->id)==NULL)
            {
                int* value=(int*)malloc(sizeof(int));
                *value=id;
                tree->insert(tree,v->id,value);
                vertices=node_overlying(vertices,v);
               // node_vt=node_overlying(node_vt,&(ocp[i]));
                id++;
            } 
        } 
    }
    for(Node* nit=node_reverse(vertices);nit!=NULL;nit=(Node*)(nit->Prev))
    {
        template_v* v=(template_v*)(nit->value);
        fprintf(outfile,"v %lf %lf %lf\n",v->point[0],v->point[1],v->point[2]); 
    } 
    if(mtlname==NULL)
    {
        mtl_class=node_overlying(NULL,node_copy(node));
    } 
    for(Node*nit=mtl_class;nit!=NULL;nit=(Node*)(nit->Next))
    {
        Node* node_mtl=(Node*)(nit->value);
        for(Node* nit1=node_mtl;nit1!=NULL;nit1=(Node*)(nit1->Next))
        {
            template_c* c=(template_c*)(nit1->value);
            OBJ_CELL_PROP* ocp=(OBJ_CELL_PROP*)(c->prop);
            if(ocp!=NULL)
            {
                for(int i=0;i<c->vertices_size;i++)
                {
                    fprintf(outfile,"vt %lf %lf %lf\n",ocp[i].vt[0],ocp[i].vt[1],ocp[i].vt[2]);
                }  
            }         
        }
    }

    id=1;
    for(Node* nit=mtl_class;nit!=NULL;nit=(Node*)(nit->Next))
    {
        Node* node_mtl=(Node*)(nit->value);
        mtlname==NULL?:fprintf(outfile,"usemtl %s\n",(char*)(nit->traits));
        for(Node* nit1=node_mtl;nit1!=NULL;nit1=(Node*)(nit1->Next))
        {
            template_c* c=(template_c*)(nit1->value);
            fprintf(outfile,"f ");
            for(int i=0;i<c->vertices_size;i++)
            {
                template_v* v=c->vertices[i];
                int* value=(int*)(tree->find(tree,v->id));
                mtlname==NULL?fprintf(outfile,"%d ",*value):fprintf(outfile,"%d/%d ",*value,id);
                id++;
            } 
            fprintf(outfile,"\n"); 
        }
        free_node(node_mtl);
    }  
    free_node(vertices);
    free_node(mtl_class);
    int_rb_tree_free_value(tree);

    int_rb_tree_free(tree);
    fclose(outfile);
}
void export_obj_from_mesh(Mesh* m,char* filename)
{
    OBJ_MESH_PROP* omp=(OBJ_MESH_PROP*)(m->prop);
    Node* node=NULL;
    for(auto cit=m->c_begin(m);cit!=m->c_end(m);cit++)
    {
        node=node_overlying(node,quote(cit));
    }

    omp==NULL?export_obj_from_node(node,filename,NULL):export_obj_from_node(node,filename,omp->mtlname);

    free_node(node);



}




#endif 