#include<intersection_remesh_and_cut.h>
#define iterator_edge lib_cell_iterator_edge
#define quote lib_cell_quote

typedef struct My_Vertex{    
    Mesh* mesh;
    //v1 mesh对应的点
    //v2 edges对应的点
    template_v* v1,*v2;
    //store new vertex's id
    int id;
    double dis;
    void* prop;
}My_Vertex;
static inline void my_vertex_init_(My_Vertex* myv)
{
    myv->v1=NULL;myv->v2=NULL;
    myv->mesh=NULL;
    myv->id=-1;
    myv->dis=0;
    myv->prop=NULL;
}

//将v v1合并为一点
static void merge_two_vertices(Mesh* m,template_v* v,template_v* v1,Mesh2_Crossover_Point* mcp1,Mesh2_Crossover_Point*mcp2)
{ 
    Crossover_Point* cp=(Crossover_Point*)(v1->prop);
    Mesh2_Crossover_Point* mcps[2]={mcp1,mcp2};
    printf("erase v id:%d\n",v1->id);
    for(int i=0;i<2;i++)
    {
        if(cp->scp[i].v!=NULL)
        {
            mcps[i]->v2p->erase(mcps[i]->v2p,cp->scp[i].v->id);
        }
        else if(cp->scp[i].f!=NULL)
        {
            mcps[i]->f2p->erase(mcps[i]->f2p,cp->scp[i].f->id);
        }
        else
        {
            mcps[i]->c2p->erase(mcps[i]->c2p,cp->scp[i].c->id);
        }
    }

    template_v* vs[2]={v,NULL};
    template_f* f=(template_f*)(v1->faces->value);
    if(f->vertices[0]==v1)
    {
        vs[1]=f->vertices[1];
    } 
    else
    {
        vs[1]=f->vertices[0];
    }

    m->delete_face(m,*f,true);
    m->create_facev(m,vs,2); 
}

static inline Node* adjust_edges_to_loop_from_nodes(Node* node,Mesh*m,Mesh2_Crossover_Point* mcp1,Mesh2_Crossover_Point*mcp2)
{
    if(node==NULL)
    {
        return NULL;
    }
    Node* n1=NULL,*n2=NULL;
    Node* nit=(Node*)(node->Next);n1=node;
    template_v* v=(template_v*)(n1->value),*v1=NULL;
    for(;nit!=NULL;nit=(Node*)(nit->Next))
    {
        v1=(template_v*)(nit->value);
        if(fabs(v->point[0]-v1->point[0])<1e-5&&fabs(v->point[1]-v1->point[1])<1e-5&&fabs(v->point[2]-v1->point[2])<1e-5)
        {
            n2=nit;
           // printf("find one \n");
            break;
        }
    }
    if(n1!=NULL&&n2!=NULL)
    {
       merge_two_vertices(m,v,v1,mcp1,mcp2);
    }
    Node* re=node_remove(node,n1);
    re=node_remove(re,n2);
    return re;
}
//检测并调整拓扑
void adjust_mesh_topology(Mesh* m,Mesh2_Crossover_Point*mcp1,Mesh2_Crossover_Point*mcp2)
{
    Node* node1=NULL,*node4=NULL;
    printf("ajust mesh topology\n");
    for(auto it=m->v_begin(m);it!=m->v_end(m);it++)
    {
        int size=node_size(quote(it)->faces);
        if(size%2==1)
        {
            Crossover_Point* cp=(Crossover_Point*)(quote(it)->prop);
            int flag=0;
            for(int i=0;i<2;i++)
            {
                if(cp->scp[i].v!=NULL&&m->vertex_is_boundary(m,*(cp->scp[i].v)))
                {
                    flag=1;
                    printf("v boundaries\n");
                }
                else if(cp->scp[i].f!=NULL&&m->face_is_boundary(m,*(cp->scp[i].f)))
                {
                    printf("f boundaries f id:%d %d\n",cp->scp[i].f->id);
                    flag=1;
                }
            } 
            if(flag==0)
            {
                printf("jiance 1 %d %.8lf %.8lf %.8lf size:%d\n",quote(it)->id,quote(it)->point[0],quote(it)->point[1],quote(it)->point[2],size);
                node1=node_overlying(node1,quote(it));
            }
        }
        else if(size==4)
        {
            printf("jiance 4:%d\n",quote(it)->id );
            node4=node_overlying(node4,quote(it));
        }
    }   
    int size=node_size(node1);
    node1=adjust_edges_to_loop_from_nodes(node1,m,mcp1,mcp2);
    int size1=node_size(node1);
    printf("size :%d size1:%d\n",size,size1 );
    while(size1!=size)
    {
        node1=adjust_edges_to_loop_from_nodes(node1,m,mcp1,mcp2);
        size=size1;
        size1=node_size(node1);
    }
    printf("size :%d size1:%d\n",size,size1 );
    free_node(node1);
    free_node(node4);
   
}

//

static inline double my_distance_two_points(double* p1,double* p2)
{
    double a[3]={p2[0]-p1[0],p2[1]-p1[1],p2[2]-p1[2]};
    double dis=a[0]*a[0]+a[1]*a[1]+a[2]*a[2];

    return SAFE_SQRT(dis);
}


//获取cell的边界点（按顺序）
//tree1是m到新建点的映射
//tree2是分割线mesh到新建点的映射
static  Node*  my_get_cell_boundary_vertices(template_c* c,Mesh2_Crossover_Point*mcp ,Int_RB_Tree* tree1,Int_RB_Tree*tree2)
{
    Node* re=NULL;
    Mesh m;
    Mesh_init(&m);

    Node* hfs=Mesh_adjust_halffaces(&m,c);
    Mesh_free(&m);
    for(Node* nit=hfs;nit!=NULL;nit=(Node*)(nit->Next))
    {
        template_hf*hf=(template_hf*)(nit->value);
        template_f* f=hf->face;
        template_v* v=hf->vertices[0];
        re=node_pushback(re,tree1->find(tree1,v->id));
        Node* node=(Node*)(mcp->f2p->find(mcp->f2p,f->id));
        for(Node* nit=node;nit!=NULL;nit=(Node*)(nit->Next))
        {
            template_v*v1=(template_v*)(nit->value);
            My_Vertex* mv=(My_Vertex*)(tree2->find(tree2,v1->id));
            mv->dis=my_distance_two_points(mv->v2->point,v->point) ;
            for(Node* nit1=re;nit1!=NULL;nit1=(Node*)(nit1->Prev))
            {
                My_Vertex* mv1=(My_Vertex*)(nit1->value);
                if(mv1->dis<=mv->dis)
                {
                    if(re==nit1)
                    {
                        re=node_pushback(re,mv);
                    }
                    else
                    {
                        Node*nn=(Node*)malloc(sizeof(Node));
                        Node_init(nn);
                        nn->value=mv;
                        if(nit1->Next!=NULL)
                        {
                            ((Node*)(nit1->Next))->Prev=nn;
                        }
                        nn->Next=nit1->Next;
                        nit1->Next=nn;
                        nn->Prev=nit1;
                    }
                    break; 
                }
            } 
        }
    }
    free_node(hfs);
    return node_reverse(re);
}
//返回值在链表的索引
static int my_get_node_value_index(Node* node,void* value)
{
    int re=-1;
    int i=0;
    for(Node*nit=node;nit!=NULL;nit=(Node*)(nit->Next))
    {
        if(nit->value==value)
        {
            re=i;
            break;
        }
        i++;
    }
    return re;
}

// 给一个边界围城的区域，返回这个区域内一条切割线
static Node* my_get_split_one_split_edge(Node* boundaries,Int_RB_Tree*tree2)
{
    Node* edge=NULL;
    int i=0;
    for(Node* nit=boundaries;nit!=NULL;nit=(Node*)(nit->Next))
    {
        edge=NULL;
        My_Vertex* mv=(My_Vertex*)(nit->value);
        if(mv->v2!=NULL)
        {
            for(Node* nit1=mv->v2->faces;nit1!=NULL;nit1=(Node*)(nit1->Next))
            {
                edge=NULL;
                template_f* f=(template_f*)(nit1->value);
                template_v* v=NULL;
                f->vertices[0]==mv->v2?v=f->vertices[1]:v=f->vertices[0];

                My_Vertex* mv1=(My_Vertex*)(tree2->find(tree2,v->id)),*mv2=mv;
                
                if(mv1!=NULL&&mv1!=node_at(boundaries,i+1)->value&&mv1!=node_at(boundaries,i-1)->value)
                {
                    Int_RB_Tree* tree=(Int_RB_Tree*)malloc(sizeof(Int_RB_Tree));
                    int_rb_tree_init(tree);
                    tree->insert(tree,f->id,f);
                    edge=node_pushback(edge,mv2);
                    edge=node_pushback(edge,mv1);
                   
                    while(mv1!=NULL&&node_find(boundaries,mv1)==NULL)
                    {
                        mv2=mv1; mv1=NULL;            
                        for(Node* nit2=mv2->v2->faces;nit2!=NULL;nit2=(Node*)(nit2->Next))
                        {
                            f=(template_f*)(nit2->value);
                            if(tree->find(tree,f->id)!=NULL)
                            {
                                continue;
                            }
                            else
                            {
                                tree->insert(tree,f->id,f);
                                f->vertices[0]==mv2->v2?v=f->vertices[1]:v=f->vertices[0]; 
                                mv1=(My_Vertex*)(tree2->find(tree2,v->id));
                                edge=node_pushback(edge,mv1);
                                break; 
                            }
                        }
                    }
                    if(mv1==NULL)
                    {
                        free_node(node_reverse(edge));
                        edge=NULL;
                    } 
                    int_rb_tree_free(tree);
                } 
                if(edge!=NULL)
                {
                    break;
                }
            } 
        } 
        if(edge!=NULL)
        {
           break; 
        }
        i++;
    }
    Node* re=node_reverse(edge);
    
    return re;
}


//给定边界围成的区域和切割线，返回切割后的两个区域
static Node** my_get_split_boundaries_area_from_boundaries_and_edge(Node* boundaries,Node* edge)
{
    if(edge==NULL)
    {return NULL;}
    Node* e_reverse=node_reverse(edge);
    int index_1=my_get_node_value_index(boundaries,edge->value);


    int index_2=my_get_node_value_index(boundaries,e_reverse->value);    
    int index1=index_1,index2=index_2;
    Node*edge1=node_copy(edge);
    if(index_1>index_2)
    {
        Node* temp_n=node_reversen(edge1);
        free_node(edge1);
        edge1=temp_n; 
        index1=index_2;
        index2=index_1;
    }
    int size=node_size(boundaries);
    Node** re=(Node**)malloc(sizeof(Node*)*2);
    memset(re,0,sizeof(Node*)*2);
    for(int i=0;i<index1;i++)
    {
        re[0]=node_pushback(re[0],node_at(boundaries,i)->value);
    }
    for(Node* nit=edge1;nit!=NULL;nit=(Node*)(nit->Next))
    {
        re[0]=node_pushback(re[0],nit->value);
    }
    
    for(int i=index2+1;i<size;i++)
    {
        re[0]=node_pushback(re[0],node_at(boundaries,i)->value);
    }
    for(int i=index1+1;i<index2;i++)
    {
        re[1]=node_pushback(re[1],node_at(boundaries,i)->value);

    }
    for(Node* nit=node_reverse(edge1);nit!=NULL;nit=(Node*)(nit->Prev))
    {
        re[1]=node_pushback(re[1],nit->value);
    }
    re[0]=node_reverse(re[0]);
    re[1]=node_reverse(re[1]);
    free_node(edge1);
    
    return re;
}  
//boundaries是边界
//nm是分割线
//tree2是 nm的点到boundaries点的映射
/// 给一个边界围成的区域，返回切割后的两个区域

static inline  Node** my_get_split_boundaries_areas(Node* boundaries,Mesh*nm ,Int_RB_Tree* tree2)
{
    Node** re=NULL;
    Node* edge=my_get_split_one_split_edge(boundaries,tree2);
    re=my_get_split_boundaries_area_from_boundaries_and_edge(boundaries,edge);
    free_node(edge);

    return re;
}

//m是c所在的mesh 
//mcp是m到nm的映射
//vertices储存新建的点，用来释放内存
//给一个cell,返回切割后的多边形区域

Node* my_get_split_areas_from_one_cell(template_c*c , Mesh2_Crossover_Point* mcp,Mesh* nm,Node** vertices)
{ 
    Int_RB_Tree* tree1=(Int_RB_Tree*)malloc(sizeof(Int_RB_Tree));
    int_rb_tree_init(tree1);
    Int_RB_Tree* tree2=(Int_RB_Tree*)malloc(sizeof(Int_RB_Tree));
    int_rb_tree_init(tree2);
    for(int i=0;i<c->vertices_size;i++)
    {
        My_Vertex*mv=(My_Vertex*)malloc(sizeof(My_Vertex));
        my_vertex_init_(mv);
        *vertices=node_overlying(*vertices,mv);
        mv->v1=c->vertices[i]; 
        tree1->insert(tree1,mv->v1->id,mv);
        mv->v2=(template_v*)(mcp->v2p->find(mcp->v2p,c->vertices[i]->id));
        if(mv->v2!=NULL)
        {
            tree2->insert(tree2,mv->v2->id,mv);
        }   
    }
    for(Node* hfit=c->halffaces;hfit!=NULL;hfit=(Node*)(hfit->Next))
    {
        template_f* f=((template_hf*)(hfit->value))->face;
        Node* value_n=(Node*)(mcp->f2p->find(mcp->f2p,f->id));
        for(Node* nit=value_n;nit!=NULL;nit=(Node*)(nit->Next))
        {
            My_Vertex* mv=(My_Vertex*)malloc(sizeof(My_Vertex));
            my_vertex_init_(mv);
            *vertices=node_overlying(*vertices,mv);
            mv->v2=(template_v*)(nit->value);
            tree2->insert(tree2,mv->v2->id,mv);
        }
    }
    Node* node=(Node*)(mcp->c2p->find(mcp->c2p,c->id));
    for(Node* nit=node; nit!=NULL;nit=(Node*)(nit->Next))
    {
        My_Vertex* mv=(My_Vertex*)malloc(sizeof(My_Vertex));
        my_vertex_init_(mv);
        *vertices=node_overlying(*vertices,mv);
        mv->v2=(template_v*)(nit->value);
        tree2->insert(tree2,mv->v2->id,mv); 
    } 
    
    Node* re=NULL,*re1=NULL;
    // Node* boundaries=my_get_cell_boundary_vertices(c,m,mcp,tree1,tree2);
    // if(tree2->size>0)
    // {        
    //     printf("c id: %d  ",c->id);
    //     Node* hfs=Mesh_adjust_halffaces(m,c);
    //     for(Node*nit=hfs;nit!=NULL;nit=(Node*)(nit->Next))
    //     {
    //         template_hf* hf=(template_hf*)(nit->value);
    //         printf("v id:%d  ",hf->vertices[0]->id);
    //     }
    //     printf("\n");
    //     printf("c id: %d  ",c->id);
    //     for(Node* nit=boundaries;nit!=NULL;nit=(Node*)(nit->Next))
    //     {
    //         My_Vertex* mv=(My_Vertex*)(nit->value);
    //         if(mv->v1!=NULL)
    //         {
    //             printf("v1 id:%d  ", mv->v1->id);
    //         }
    //         else
    //         {
    //             printf("v2 id:%d  ", mv->v2->id);
    //         }
    //     }
    //     printf("\n");
    //     Node* edge=my_get_split_one_split_edge(boundaries,tree2);
    //     printf("edge\n");
    //     for(Node* nit=edge;nit!=NULL;nit=(Node*)(nit->Next))
    //     {
    //         My_Vertex* mv=(My_Vertex*)(nit->value);
    //         if(mv->v1!=NULL)
    //         {
    //             printf("v1 id:%d  ", mv->v1->id);
    //         }
    //         else
    //         {
    //             printf("v2 id:%d  ", mv->v2->id);
    //         }
    //     }
    //     printf("\n");

    //     printf("split edge\n");
    //     Node**splits= my_get_split_boundaries_area_from_boundaries_and_edge(boundaries,edge);
    //     if(splits!=NULL)
    //     {
    //         for(Node*nit=splits[0];nit!=NULL;nit=(Node*)(nit->Next))
    //         {
    //             My_Vertex* mv=(My_Vertex*)(nit->value);
    //             if(mv->v1!=NULL)
    //             {
    //                 printf("v1 id:%d  ", mv->v1->id);
    //             }
    //             else
    //             {
    //                 printf("v2 id:%d  ", mv->v2->id);
    //             } 
    //         }
    //         printf("\n");
    //         for(Node*nit=splits[1];nit!=NULL;nit=(Node*)(nit->Next))
    //         {
    //             My_Vertex* mv=(My_Vertex*)(nit->value);
    //             if(mv->v1!=NULL)
    //             {
    //                 printf("v1 id:%d  ", mv->v1->id);
    //             }
    //             else
    //             {
    //                 printf("v2 id:%d  ", mv->v2->id);
    //             } 
    //         }
    //     }
    //     printf("\n");
    //     free_node(edge);
    //     free_node(hfs);
    // }
    // free_node(boundaries);

    Node* boundaries=my_get_cell_boundary_vertices(c,mcp,tree1,tree2);
    
    re1=node_overlying(re1,boundaries);
    while(re1!=NULL)
    {
        
        Node* temp_n=NULL;
        for(Node* nit=re1;nit!=NULL;nit=(Node*)(nit->Next))
        {
            Node**two_n= my_get_split_boundaries_areas((Node*)(nit->value),nm,tree2);

            if(two_n==NULL)
            {
             
                re=node_overlying(re,nit->value);
            }
            else
            {     
                temp_n=node_overlying(temp_n,two_n[0]);
                temp_n=node_overlying(temp_n,two_n[1]);
                free(two_n);
                free_node((Node*)(nit->value));
            }
        }
        free_node(re1);
        re1=temp_n;
    } 


    // if(node_size(re)>=0)
    // {
    //     printf("split edge\n");
    //     for(Node*nit=re;nit!=NULL;nit=(Node*)(nit->Next))
    //     {
    //         for(Node* nit1=(Node*)(nit->value);nit1!=NULL;nit1=(Node*)(nit1->Next))
    //         {
    //             My_Vertex* mv=(My_Vertex*)(nit1->value);
    //             if(mv->v1!=NULL)
    //             {
    //                 printf("v1 id:%d  ", mv->v1->id);
    //             }
    //             else
    //             {
    //                 printf("v2 id:%d  ", mv->v2->id);
    //             } 
    //         }
    //         printf("\n"); 
    //     }
    //     printf("\n"); 
    // }
    

    int_rb_tree_free(tree1);int_rb_tree_free(tree2);
    return re;
} 


Mesh* my_intersection_remesh(Node* m1,Mesh2_Crossover_Point*mcp,Mesh*m,Int_RB_Tree* tree2,Int_RB_Tree* tree3)
{
    Mesh* re=(Mesh*)malloc(sizeof(Mesh));
    Mesh_init(re);
    re->dimension=2;re->simplex=1;re->manifold_require=1;
    Int_RB_Tree* tree1=(Int_RB_Tree*)malloc(sizeof(Int_RB_Tree));
    int_rb_tree_init(tree1);
    for(Node* cit=m1;cit!=NULL;cit=(Node*)(cit->Next))
    { 
        Node* vertices=NULL;
        Node* bns=my_get_split_areas_from_one_cell((template_c*)(cit->value),mcp,m,&vertices); 
        
        for(Node* nit=bns;nit!=NULL;nit=(Node*)(nit->Next))
        {
            
            Node* node=(Node*)(nit->value);
            int len=node_size(node);
            
            double**vv=(double**)malloc(sizeof(double*)*len);
            Node* nit1=node;
            template_v* v=NULL; 
            for(int i=0;i<len;i++)
            {
                vv[i]=(double*)malloc(sizeof(double)*3);
                My_Vertex* mv=(My_Vertex*)(nit1->value);
                v=mv->v2==NULL?mv->v1:mv->v2; 
                vv[i][0]=v->point[0];
                vv[i][1]=v->point[1];
                vv[i][2]=v->point[2];
                nit1=(Node*)(nit1->Next);
            }
            int** s=( int**)malloc(sizeof(int*)*(len-2));
            for(int i=0;i<len-2;i++)
            {
                s[i]=(int*)malloc(sizeof(int)*3);
            }
            subdivision_of_polygon(vv,len,s);

            for(int i=0;i<len-2;i++)
            {
                template_v* vs[3]={NULL,NULL,NULL};
                for(int j=0;j<3;j++)    
                {
                    My_Vertex* mv=(My_Vertex*)(node_at(node,s[i][j])->value);
                    vs[j]=(mv->v2==NULL?(template_v*)(tree1->find(tree1,mv->v1->id)):(template_v*)(tree2->find(tree2,mv->v2->id)));
                    if(vs[j]==NULL)
                    { 
                        vs[j]=re->create_vertexv(re,(mv->v2==NULL?mv->v1->point:mv->v2->point),3);
                        mv->v2==NULL?tree1->insert(tree1,mv->v1->id,vs[j]):tree2->insert(tree2,mv->v2->id,vs[j]); 
                    }
                }
                template_c*cc=re->create_cellv(re,vs,3); 
                tree3->insert(tree3,cc->id,cit->value);
            } 
            free_node(node);
            for(int i=0;i<len;i++)
            {
                free(vv[i]);
            }
            for(int i=0;i<len-2;i++)
            {
                free(s[i]);
            }
            free(vv);free(s);
        } 
        free_node_value(vertices);
        free_node(vertices);
        free_node(bns);
    }
    int_rb_tree_free(tree1);
    
    return re;
}

void my_intersection_cut(Mesh* m,Mesh* nm,Int_RB_Tree* tree,Int_RB_Tree *tree3)
{
    Node* node=NULL;
    for(auto fit=nm->f_begin(nm);fit!=nm->f_end(nm);fit++)
    {
        template_v* vs[2]={NULL};
        vs[0]=(template_v*)(tree->find(tree,quote(fit)->vertices[0]->id));
        vs[1]=(template_v*)(tree->find(tree,quote(fit)->vertices[1]->id));
        template_f* f=m->get_facev(m,vs,2);
        if(f==NULL)
        {
           // printf("cuowu vs:%d %d\n",vs[0]->id,vs[1]->id);
        }
        else
        {
            node=node_overlying(node,f);
        }

    }  
    Int_RB_Tree* tree1=(Int_RB_Tree*)malloc(sizeof(Int_RB_Tree));
    int_rb_tree_init(tree1);
    Int_RB_Tree* tree2=(Int_RB_Tree*)malloc(sizeof(Int_RB_Tree));
    int_rb_tree_init(tree2);

    Mesh_cut_along_the_curvef(m,node,tree1,tree2);
    for(auto it=tree1->begin(tree1);it.it!=NULL;it++)
    {
        int id=*((int*)(it.second));
        void* value=tree3->find(tree3,id);
        tree3->erase(tree3,id);
        tree3->insert(tree3,it.first,value); 
        //tree3->erase(tree3,id);
    }


    int_rb_tree_free_value(tree1);
    int_rb_tree_free(tree1); 
    int_rb_tree_free(tree2);
    free_node(node);
}