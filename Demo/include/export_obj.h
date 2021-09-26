#ifndef LIBCELL_EXPORT_OBJ_H_
#define LIBCELL_EXPORT_OBJ_H_
#include<Mesh_IO/Mesh_IO.h>


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
        OBJ_CELL_PROP* ocp=(OBJ_CELL_PROP*)(c->prop); 
        Node* nit1=mtl_class;
        for(;nit1!=NULL;nit1=(Node*)(nit1->Next))
        {
            if(strcmp(ocp->name,(char*)(nit1->traits))==0)
            {
                break;
            } 
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
        fprintf(outfile,"usemtl %s\n",(char*)(nit->traits));
        for(Node* nit1=node_mtl;nit1!=NULL;nit1=(Node*)(nit1->Next))
        {
            template_c* c=(template_c*)(nit1->value);
            fprintf(outfile,"f ");
            for(int i=0;i<c->vertices_size;i++)
            {
                template_v* v=c->vertices[i];
                int* value=(int*)(tree->find(tree,v->id));
                fprintf(outfile,"%d/%d ",*value,id);
                id++;
            } 
            fprintf(outfile,"\n"); 
        }
        free_node(node_mtl);
    }  
    free_node(vertices);
    int_rb_tree_free_value(tree);

    int_rb_tree_free(tree);
    fclose(outfile);
}




#endif 