#include "include/Arcroll.h" 
#define Matrix4x4 Viewer_Matrix4x4_

void viewer_arcroll_init(Viewer_Arcroll*va)
{
    va->old_mouse_coord=(float*)malloc(sizeof(float)*2);
    va->vs=NULL;
    va->mc=0;
    va->vw=NULL;
}
void viewer_Arcroll_cursor_position_callback(Viewer_Intera* mi)
{

    Interactor_GlobalInfo* g_info=mi->g_info;
    Viewer_Arcroll* ma=(Viewer_Arcroll*)(mi->representation);
    Viewer_Camera* mc=(Viewer_Camera*)(ma->mc);
    if(g_info->mouse_button==VIEWER_MOUSE_BUTTON_LEFT&&g_info->mouse_action==VIEWER_PRESS&&g_info->key_action==0)
    {
        //printf("libo\n");
        float tempx=g_info->mouse_coord[0]-ma->old_mouse_coord[0];
        float tempy=g_info->mouse_coord[1]-ma->old_mouse_coord[1];
        tempx=-tempx*0.005;tempy=tempy*0.005;
        Matrix4x4 *temp_m=(Matrix4x4*)malloc(sizeof(Matrix4x4));
        Matrix4x4_init_float(temp_m);
        Matrix4x4 *MV=(Matrix4x4*)malloc(sizeof(Matrix4x4));
        Matrix4x4_init_float(MV);
        float* data=(float*)(MV->data);
        data[2*4+3]=mc->focal_distance;
        Matrix4x4 *MV_inverse=MV->inverse(MV);
        data=(float*)(temp_m->data);
        data[0*4+0]=cos(tempx);data[0*4+1]=-sin(tempx)*sin(tempy);
        data[0*4+2]=sin(tempx)*cos(tempy);
        data[1*4+1]=cos(tempy);
        data[1*4+2]=sin(tempy);
        data[2*4+0]=-sin(tempx);
        data[2*4+1]=-cos(tempx)*sin(tempy);
        data[2*4+2]=cos(tempx)*cos(tempy);
        MV->mult(MV,temp_m);
        MV->mult(MV,MV_inverse);
        MV->mult(MV,mc->matrix_inverse);
        mc->matrix_inverse->copy_data(mc->matrix_inverse,MV);
        Matrix4x4_free(mc->matrix);
        mc->matrix=mc->matrix_inverse->inverse(mc->matrix_inverse);
        Matrix4x4_free(temp_m);
        Matrix4x4_free(MV);
        Matrix4x4_free(MV_inverse);
    }
    else if(g_info->mouse_button==VIEWER_MOUSE_BUTTON_RIGHT&&g_info->mouse_action==VIEWER_PRESS)
    {
        float tempx=g_info->mouse_coord[0]-ma->old_mouse_coord[0];
        float tempy=g_info->mouse_coord[1]-ma->old_mouse_coord[1];
        tempx=-tempx*0.005;tempy=tempy*0.005;
        float*data=(float*)mc->matrix_inverse->data;
        data[0*4+3]-=tempx;
        data[1*4+3]-=tempy;
        Matrix4x4_free(mc->matrix);
        mc->matrix=mc->matrix_inverse->inverse(mc->matrix_inverse);
    }
    //printf("%lf %lf\n",g_info->mouse_coord[0],g_info->mouse_coord[1]);

    ma->old_mouse_coord[0]=g_info->mouse_coord[0];
    ma->old_mouse_coord[1]=g_info->mouse_coord[1];
}
void viewer_Arcroll_scroll_callback(Viewer_Intera*mi,double x,double y)
{
    Interactor_GlobalInfo* g_info=mi->g_info;
    Viewer_Arcroll* ma=(Viewer_Arcroll*)(mi->representation);

    Viewer_World* vw=ma->vw;
    Viewer_Camera* mc=(Viewer_Camera*)(ma->mc);
    //Viewer_World* vw=(Viewer_World*)(mi->representation);
    //printf("scroll key_mods:%d ,key_action:%d\n",g_info->key_mods,g_info->key_action);
    if((g_info->key==VIEWER_KEY_CONTROL&&g_info->key_action==1))
    {

        //printf("here\n");
        mc->focal_distance+=(float)y*0.05;
    }
    
    else if(g_info->key==VIEWER_KEY_ALT&&g_info->key_action==1)
    {
        char points[]="Points";
        Node* names_id=vw->find_species(vw,points);
        RB_int rbt,*rbt1=NULL;
        rbt.key=*((int*)(names_id->value));
        SAFE_FREE(names_id->value);
        free_node(names_id);
        rbt1=(RB_int*)vw->species2somethings->find(vw->species2somethings,&rbt);
        RB_Tree* tree=NULL;
        RB_Tree_Trav iter1;
        if(rbt1!=NULL)
        {
            tree=((RB_Tree*)(rbt1->value));
            //iter1=tree->begin(tree);
            for(iter1=tree->begin(tree);iter1.it!=NULL;iter1.next(&iter1))
            {
                Viewer_Something* vs=(Viewer_Something*)(iter1.second(&iter1));
                Viewer_Points* vp=(Viewer_Points*)(vs->evolution);      
                float * data=(float*)(vp->mat->data);
                data[0*4+0]*=(1-(float)y*0.05);
                data[1*4+1]*=(1-(float)y*0.05);
                data[2*4+2]*=(1-(float)y*0.05);          
            }
            //SAFE_FREE(iter1);
        }
        char faces[]="Faces";
        names_id=vw->find_species(vw,faces);
        rbt.key=*((int*)(names_id->value));
        SAFE_FREE(names_id->value);
        free_node(names_id);
        rbt1=(RB_int*)vw->species2somethings->find(vw->species2somethings,&rbt);
        if(rbt1!=NULL)
        {

            tree=(RB_Tree*)(rbt1->value);
            //iter1=tree->begin(tree);
            for(iter1=tree->begin(tree);iter1.it!=NULL;iter1.next(&iter1))
            {
                Viewer_Something* vs=(Viewer_Something*)(iter1.second(&iter1));
                Viewer_Faces* vf=(Viewer_Faces*)(vs->evolution);      
                float * data=(float*)(vf->mat->data);
                data[0*4+0]*=(1-(float)y*0.05);
                data[1*4+1]*=(1-(float)y*0.05);
                data[2*4+2]*=(1-(float)y*0.05);          
            }
            //SAFE_FREE(iter1);

        }
        char edges[]="Edges";
        names_id=vw->find_species(vw,edges);
        rbt.key=*((int*)(names_id->value));
        SAFE_FREE(names_id->value);
        free_node(names_id);
        rbt1=(RB_int*)vw->species2somethings->find(vw->species2somethings,&rbt);
        if(rbt1!=NULL)
        {

            tree=(RB_Tree*)(rbt1->value);
            //iter1=tree->begin(tree);
            for(iter1=tree->begin(tree);iter1.it!=NULL;iter1.next(&iter1))
            {
                Viewer_Something* vs=(Viewer_Something*)(iter1.second(&iter1));
                if(vs==ma->vs)
                {continue;}
                Viewer_Edges* ve=(Viewer_Edges*)(vs->evolution);      
                float * data=(float*)(ve->mat->data);
                data[0*4+0]*=(1-(float)y*0.05);
                data[1*4+1]*=(1-(float)y*0.05);
                data[2*4+2]*=(1-(float)y*0.05);          
            }
            //SAFE_FREE(iter1);
        }
    }
    else if(g_info->key_action==0)
    { 
        if(g_info->ipi.vs!=NULL)
        {

            Viewer_Something* ms=(Viewer_Something*)(g_info->ipi.vs);
            if(ms!=ma->vs)
            {
                float * data=NULL;
                if(strcmp("Faces",ms->name)==0)
                {
                    Viewer_Faces* mf=(Viewer_Faces*)(ms->evolution);

                    data=(float*)(mf->mat->data);
                //printf("%lf\n",(1-(float)y*0.05));
                }
                else if(strcmp("Edges",ms->name)==0)
                {
                    Viewer_Edges* ve=(Viewer_Edges*)(ms->evolution);
                    data=(float*)(ve->mat->data);

                }
                else if(strcmp("Points",ms->name)==0)
                {
                    Viewer_Points* vp=(Viewer_Points*)(ms->evolution);
                    data=(float*)(vp->mat->data);
                }
                if(data!=NULL)
                {
                    data[0*4+0]*=(1-(float)y*0.05);
                    data[1*4+1]*=(1-(float)y*0.05);
                    data[2*4+2]*=(1-(float)y*0.05);
                }
                  
            }

        }
        else
        {
            float *data=(float*)(mc->matrix_inverse->data);
            data[2*4+3]+=(float)y*0.05;
            Matrix4x4_free(mc->matrix);
            mc->matrix=mc->matrix_inverse->inverse(mc->matrix_inverse); 
        }

    }
}
void viewer_Arcroll_mouse_button_callback(Viewer_Intera* mi)
{
    Interactor_GlobalInfo* g_info=mi->g_info;
    
    //printf("%d\n",g_info->mouse_button);
    if(g_info->ipi.vs!=NULL)
    {
        Viewer_Something*vs=(Viewer_Something*)(g_info->ipi.vs);

        
       // printf("pick something id:%d\n",vs->id);
    }
    if(g_info->key==VIEWER_KEY_CONTROL&&g_info->key_action==1)
    {
    //    int id=g_info->readpixelcolor[0]*255*255+g_info->readpixelcolor[1]*255+g_info->readpixelcolor[2];
        //char faces[]="faces";
        //Node* names_id=Mesh_viewer_world_find_species()

    
    }
    
}

void viewer_Arcroll_key_callback(Viewer_Intera*vi)
{
    Interactor_GlobalInfo* g_info=vi->g_info;
    Viewer_Arcroll* va=(Viewer_Arcroll*)(vi->representation);
    if(g_info->key_action==VIEWER_PRESS&&g_info->key==VIEWER_KEY_D)
    {

        if(g_info->ipi.vs==va->vs||g_info->ipi.vs==NULL)
        {
            return ;
        }
        Viewer_Something*  vs=(Viewer_Something*)(g_info->ipi.vs);
        if(vs->disappear==1)
        {
            return;
        }
        va->disappear_somethings=node_overlying(va->disappear_somethings,vs);
        vs->disappear=1;  

    }
    else if(g_info->key_action==VIEWER_PRESS&&g_info->key==VIEWER_KEY_B)
    {
//    auto  vs=(Viewer_Something*)(g_info->pick_something);
  //      va->disappear_somethings=node_overlying(va->disappear_somethings,vs);
    //    vs->disappear=1;  
        if(va->disappear_somethings!=NULL)
        {
            Node* temp_n=va->disappear_somethings;va->disappear_somethings=(Node*)(va->disappear_somethings->Next);
            Viewer_Something* vs=(Viewer_Something*)(temp_n->value);

            vs->disappear=0; 

            free(temp_n);
        }
    }
    else if (g_info->key_action==VIEWER_PRESS&&g_info->key==VIEWER_KEY_I)
    {
        if(g_info->ipi.vs!=NULL)
        {
            printf("%s name_id:%d marked_id: %d\n",g_info->ipi.vs->name,g_info->ipi.vs->name_id,g_info->ipi.marked_element);

            // if(strcmp(g_info->ipi.vs->name,"Faces"))
            // {
            //     Viewer_Faces* vf=(Viewer_Faces*)(g_info->ipi.vs->evolution);


            // } 

        }
    }
    
}
#undef Matrix4x4
