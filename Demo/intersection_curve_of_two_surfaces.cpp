#include<intersection_curve_of_two_surfaces.h>
#include<Mesh/lib_cell_iterator.h>
#define DEFAULT_THRESHOLD  1e-7 


static inline int is_same_point(double *p1,double *p2)
{
    if(fabs(p1[0]-p2[0])<1e-6&&fabs(p1[1]-p2[1])<1e-6&&fabs(p1[2]-p2[2])<1e-6)
    {
        return 1;
    }
    return 0; 
}
// static inline double * get_coordinate_in_another_coordinate_system(double* p,double* a1,double *a2,double*a3)
// {
//     Matrix4x4* m=(Matrix4x4*)malloc(sizeof(Matrix4x4));
//     Matrix4x4_init_double(m);

//     double* data=(double*)(m->data);
//     data[0*4+0]=a1[0]; data[1*4+0]=a1[1];  data[2*4+0]=a1[2];   
//     data[0*4+1]=a2[0]; data[1*4+1]=a2[1];  data[2*4+1]=a2[2];
//     data[0*4+2]=a3[0]; data[1*4+2]=a3[1];  data[2*4+2]=a3[2];
//     Matrix4x4* mi=m->inverse(m);
//     if(mi==NULL)
//     {
//         Matrix4x4_free(m);
//         return NULL;
//     } 
//     data=(double*)(mi->data);
//     double * re=(double*)malloc(sizeof(double)*3); 
//     memset(re,0,sizeof(double)*3);
//     for(int i=0;i<3;i++)
//     {
//         re[i]=0;
//        for(int j=0;j<3;j++)
//        {

//             re[i]+=data[i*4+j]*p[j];
//        } 

//     }
//     Matrix4x4_free(m);
//     Matrix4x4_free(mi);
//     return re;
// }
// //return the triangle coordinate of p in p1 p2 p3
// static inline double* get_triangle_coordinate(double* p,double * p1,double*p2,double* p3 )
// {

//     double a1[3]={p1[0]-p3[0],p1[1]-p3[1],p1[2]-p3[2]};
//     double a2[3]={p2[0]-p3[0],p2[1]-p3[1],p2[2]-p3[2]};
//     double pp[3]={p[0]-p3[0],p[1]-p3[1],p[2]-p3[2]};
//     double* n=out_product(a1,a2);

//     double* coord=get_coordinate_in_another_coordinate_system(pp,a1,a2,n);

//     SAFE_FREE(n);
//     if(coord==NULL)
//     {
//         return NULL;
//     }
//     double *re=(double*)malloc(sizeof(double)*3);
//     re[0]=coord[0];re[1]=coord[1];
    

//     re[2]=1-re[0]-re[1];
 
//     SAFE_FREE(coord);
//     return re;
// }
//是否存在已经创建的点

static template_v* is_exist_same_point_in_cell(template_c* c,double* p,Mesh2_Crossover_Point* mcp)
{
    for(int i=0;i<c->vertices_size;i++)
    {
        template_v * v= (template_v*)(mcp->v2p->find(mcp->v2p,c->vertices[i]->id));
        if(v!=NULL&&is_same_point(p,v->point))
        {
            return v;
        }
    }
    for(Node* hfit=c->halffaces;hfit!=NULL;hfit=(Node*)(hfit->Next))
    {
        template_f* f=((template_hf*)(hfit->value))->face;
        Node* value=(Node*)(mcp->f2p->find(mcp->f2p,f->id));
        for(Node* nit=value;nit!=NULL;nit=(Node*)(nit->Next))
        {
           template_v*v=(template_v*)(nit->value);
           if(is_same_point(p,v->point))
           {
                return v;  
           }
        }
    }
    Node* value=(Node*)(mcp->c2p->find(mcp->c2p,c->id));

    for(Node* nit=value;nit!=NULL;nit=(Node*)(nit->Next))
    {
        template_v*v=(template_v*)(nit->value);
        if(is_same_point(p,v->point))
        {
            return v;  
        }
    }
    return NULL;
}


// compute cosine value
static inline double cos_distance_point_to_line_segments1(double* p,double* p1,double* p2)
{

    double n[3]={p2[0]-p1[0],p2[1]-p1[1],p2[2]-p1[2]};
    double a[3]={p[0]-p1[0],p[1]-p1[1],p[2]-p1[2]}; 
    if(!normalize(n)||!normalize(a))
    {
        printf("normalize cuowu\n");
    }   
    //double dis=a[0]*a[0]+a[1]*a[1]+a[2]*a[2];
    double dis1=n[0]*a[0]+n[1]*a[1]+n[2]*a[2];
    return dis1;
    
}
static void vertex_attachment_to_celln(template_c*c,template_v* nv,Mesh2_Crossover_Point* mcp,SCrossover_Point * scp)
{
    for(int i=0;i<c->vertices_size;i++)
    {
        if(is_same_point(nv->point,c->vertices[i]->point))
        {
            mcp->v2p->insert(mcp->v2p,c->vertices[i]->id,nv);
            scp->v=c->vertices[i];
            return ;
        }
    } 
    for(Node* hfit=c->halffaces;hfit!=NULL;hfit=(Node*)(hfit->Next))
    {
        template_f* f=((template_hf*)(hfit->value))->face;
        if(fabs(cos_distance_point_to_line_segments1(nv->point,f->vertices[0]->point,f->vertices[1]->point)-1)<1e-7)
        {
            Node* value=(Node*)(mcp->f2p->find(mcp->f2p,f->id));
            value=node_overlying(value,nv);
            mcp->f2p->insert(mcp->f2p,f->id,value); 
            scp->f=f;
            return ; 
        }
    }

    Node* value=(Node*)(mcp->c2p->find(mcp->c2p,c->id));
    value=node_overlying(value, nv);
    mcp->c2p->insert(mcp->c2p,c->id, value);
     
    scp->c=c; 
}
//
//
static template_v** get_intersection_vertices_from_two_cells(template_c* c1,template_c* c2,Mesh* m,Mesh2_Crossover_Point * mcp1,Mesh2_Crossover_Point * mcp2)
{
    double ** points=get_intersection_points_from_two_cells(c1,c2);
    if(points==NULL)
    {return NULL;}
    int len=1;
    if(is_same_point(points[0],points[1]))
    {len=1;} 
    else
    {len=2;}
    template_v** re=(template_v**)malloc(sizeof(template_v*)*2);
    memset(re,0,sizeof(template_v*)*2);
    for(int i=0;i<len;i++)
    {
        re[i]=is_exist_same_point_in_cell(c1,points[i],mcp1);
        if(re[i]==NULL)
        {
            //re[i]=is_exist_same_point_in_cell(c2,points[i],mcp2);
            //if(re[i]==NULL)
            {
                re[i]=m->create_vertexv(m,points[i],3);
                Crossover_Point *cp=(Crossover_Point*)malloc(sizeof(Crossover_Point));
                crossover_point_init(cp);  
                re[i]->prop=cp;
                vertex_attachment_to_celln(c1,re[i],mcp1,&(cp->scp[0]));
                vertex_attachment_to_celln(c2,re[i],mcp2,&(cp->scp[1]));
            }
            // else
            // {
            //     printf("cuowu1\n");
            //     Crossover_Point* cp=(Crossover_Point*)(re[i]->prop);
            //     vertex_attachment_to_celln(c1,re[i],mcp1,&(cp->scp[0]));
            // }
        }
        else
        {
            // if(is_exist_same_point_in_cell(c2,points[i],mcp2)==NULL)
            // {
            //     printf("cuowu2\n");
            //     Crossover_Point*cp=(Crossover_Point*)(re[i]->prop);
            //     vertex_attachment_to_celln(c2,re[i],mcp2,&(cp->scp[1]));
            // }
        } 
    } 
    for(int i=0;i<2;i++)
    {
        SAFE_FREE(points[i]);
    } 
    free(points);
    return re;
}


void get_intersection_lines_of_two_nodes(Node* n1,Node* n2,Mesh* m,Mesh2_Crossover_Point* mcp1,Mesh2_Crossover_Point* mcp2)
{
    if(n1==NULL||n2==NULL)
    {return;}
    m->simplex=1;
    m->manifold_require=0;
    for(Node* it1=n1;it1!=NULL;it1=(Node*)(it1->Next))
    {
        template_c* c1=(template_c*)(it1->value);
        for(Node* it2=n2;it2!=NULL;it2=(Node*)(it2->Next))
        {
            template_c*c2=(template_c*)(it2->value);
            template_v** vs=get_intersection_vertices_from_two_cells(c1,c2,m,mcp1,mcp2);
            if(vs==NULL)
            {continue;}      
             
            if(vs[0]!=NULL&&vs[1]!=NULL)
            {
                m->create_facev(m,vs,2); 
            } 
            free(vs);
        }
    } 
}
static inline int is_intersected_two_cube(double *min1,double* max1,double* min2,double* max2)
{
    if(min2[0]>max1[0]+DEFAULT_THRESHOLD||min2[1]>max1[1]+DEFAULT_THRESHOLD||min2[2]>max1[2]+DEFAULT_THRESHOLD)
    {
        return 0;
    }
    if(max2[0]<min1[0]+DEFAULT_THRESHOLD||max2[1]<min1[1]+DEFAULT_THRESHOLD||max2[2]<min1[2]+DEFAULT_THRESHOLD) 
    {
        return 0;
    }
    return 1;
}

static inline double* get_cube_from_cell(template_c*c)
{
    double* re=(double*)malloc(sizeof(double)*6);
    re[0]=c->vertices[0]->point[0];re[1]=c->vertices[0]->point[1];re[2]=c->vertices[0]->point[2];
    re[3]=c->vertices[0]->point[0];re[4]=c->vertices[0]->point[1];re[5]=c->vertices[0]->point[2];

    for(int i=0;i<c->vertices_size;i++)
    {
        for(int j=1;j<3;j++)
        {
            if(c->vertices[j]->point[i]<re[i])
            {
                re[i]=c->vertices[j]->point[i];
            }
            else if(c->vertices[j]->point[i]>re[i+3])
            {
                re[i+3]=c->vertices[j]->point[i];
            }
        }
    }
    return re; 
}

static inline double* get_cube_from_cell_nodes(Node*n)
{
    if(n==NULL)
    {return NULL;}
    template_v* v1=(((template_c*)(n->value))->vertices[0]);
    double* re=(double*)malloc(sizeof(double)*6); 
    re[0]=v1->point[0];re[1]=v1->point[1];re[2]=v1->point[2];
    re[3]=v1->point[0];re[4]=v1->point[1];re[5]=v1->point[2];
    for(Node* nit=n;nit!=NULL;nit=(Node*)(nit->Next))
    {
        template_c*  c=(template_c*)(nit->value);
        for(int i=0;i<c->vertices_size;i++)
        {
            for(int j=0;j<3;j++)
            {
                if(c->vertices[i]->point[j]<re[j])
                {
                    re[j]=c->vertices[i]->point[j];
                }
                else if(c->vertices[i]->point[j]>re[j+3])
                {
                    re[j+3]=c->vertices[i]->point[j];
                }
            } 
        } 
    }
    return re;  
}
//simplyfy n2
//@flag :: is changed
Node* simplify_node_of_two_nodes(Node* n1,Node *n2,int* flag)
{
    *flag=0;
    if(n1==NULL||n2==NULL)
    {
        return NULL;
    }
    Node* re=NULL;
    double* cube1=get_cube_from_cell_nodes(n1);
    for(Node* nit=n2;nit!=NULL;nit=(Node*)(nit->Next))
    {
        template_c* c=(template_c*)(nit->value);
        double* cube2=get_cube_from_cell(c);
        if(is_intersected_two_cube(cube1,&(cube1[3]),cube2,&(cube2[3])))
        {
            re=node_overlying(re,c);
        }
        else
        {
            *flag=1;
        }
        SAFE_FREE(cube2);  
    }
    SAFE_FREE(cube1);
    return re;
}




void get_intersection_lines_of_two_nodesn(Node* n11,Node* n22,Mesh*m,Mesh2_Crossover_Point *mcp1,Mesh2_Crossover_Point*mcp2)
{
    Node* n1=node_copy(n11),*n2=node_copy(n22);
    int flag=0,flag1=0;
    while(1)
    {
        flag=0;
        Node* temp_n1=simplify_node_of_two_nodes(n1,n2,&flag1);
        if(flag1==0)
        {
            flag++;
        }
        free_node(n2);
        n2=n1;n1=temp_n1;temp_n1=NULL;
        temp_n1=simplify_node_of_two_nodes(n1,n2,&flag1);
        if(flag1==0)
        {
            flag++;
        } 
        free_node(n2);
        n2=n1, n1= temp_n1;temp_n1=NULL;

        if(flag>=2)
        {
            break;
        }
    }  
    get_intersection_lines_of_two_nodes(n1,n2,m,mcp1,mcp2);
    m->manifold_require=0;
    free_node(n1);free_node(n2);
}


