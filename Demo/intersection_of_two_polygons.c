#include <intersection_of_two_polygons.h>


//@ return array which have four length


static inline double*get_triangle_plane_equation(double* tr1,double* tr2,double*tr3)
{
    double a[3]={0},b[3]={0};
    a[0]=tr2[0]-tr1[0];
    a[1]=tr2[1]-tr1[1];
    a[2]=tr2[2]-tr1[2];

    b[0]=tr3[0]-tr2[0];
    b[1]=tr3[1]-tr2[1];
    b[2]=tr3[2]-tr2[2];
    double * n=out_product(a,b);
    if(!normalize(n))
    {
        free(n);
        return NULL;
    }
    double * re=(double*)malloc(sizeof(double)*4);
    re[3]=-(n[0]*tr1[0]+n[1]*tr1[1]+n[2]*tr1[2]);
    re[0]=n[0];re[1]=n[1];re[2]=n[2];
    // printf("test :%lf\n",re[0]*tr1[0]+re[1]*tr1[1]+re[2]*tr1[2] );
    // printf("test :%lf\n",re[0]*tr2[0]+re[1]*tr2[1]+re[2]*tr2[2] ); 
    // printf("test :%lf\n",re[0]*tr2[0]+re[1]*tr2[1]+re[2]*tr2[2] ); 
    
    free(n); 
    return re; 
}

//@ p1 is a plane equation
//@ p2 is a plane equation
//return t* n+ p:: line equation
double* find_intersection_get_line_equation_from_2plane(double * p1,double *p2)
{
    double * n=out_product(p1,p2);
    if(!normalize(n))
    {
        free(n);
        return NULL;
    } 
    Matrix4x4 * m=(Matrix4x4*)malloc(sizeof(Matrix4x4));
    Matrix4x4_init_double(m); 
    double* data=(double*)(m->data);
    data[0*4+0]=p1[0];data[0*4+1]=p1[1];data[0*4+2]=p1[2];

    data[1*4+0]=p2[0];data[1*4+1]=p2[1];data[1*4+2]=p2[2];

    data[2*4+0]=n[0];data[2*4+1]=n[1];data[2*4+2]=n[2];

    Matrix4x4*mi=m->inverse(m);
    if(mi==NULL)
    {
        free(n);
       // Matrix4x4_free(mi);
        Matrix4x4_free(m);
        return NULL;
    } 
    double *data1=(double*)(mi->data);
    double b[3]={-p1[3],-p2[3],0};
    double * re=(double*)malloc(sizeof(double)*6);
    //printf("mi*********************************\n");
    for(int i=0;i<3;i++)
    {
        re[i+3]=0;
        for(int j=0;j<3;j++)
        {
            double temp_d=0;
            for(int k=0;k<4;k++)
            {
                temp_d+=data[i*4+k]*data1[k*4+j];
            }
            //printf("%lf  ", temp_d);
        //   printf("%lf  ",data1[i*4+j] );
            re[i+3]+=b[j]*data1[i*4+j];
        }
        //printf("tset:%lf \n",data1[]);
      //  printf("\n");
    } 
    //printf("mi **************************************\n");
    re[0]=n[0];re[1]=n[1];re[2]=n[2];
    Matrix4x4_free(mi);

    Matrix4x4_free(m);
    free(n);
    return re;
  
}
static inline double * get_line_equation_from_two_p(double*p1,double* p2 )
{
    double * re=(double*)malloc(sizeof(double)*6);
    re[0]=p2[0]-p1[0];re[1]=p2[1]-p1[1];re[2]=p2[2]-p1[2];
    if(!normalize(re))
    {
        free(re);
        return NULL;
    } 
    re[3]=p1[0];re[4]=p1[1];re[5]=p1[2];

    return re;
}
static inline double distance_point_to_line(double* p,double* l)
{
    double a[3]={p[0]-l[3],p[1]-l[4],p[2]-l[5]};
    double dis=a[0]*l[0]+a[1]*l[1]+a[2]*l[2];
    double dis1=a[0]*a[0]+a[1]*a[1]+a[2]*a[2];
   
    return SAFE_SQRT(dis1-dis*dis); 

}
// static inline int is_line_same_dir_with_line_segment(double* l,double* v1 ,double* v2)
// {
//   //  is same dir
//     double l1[3]={v2[0]-v1[0],v2[1]-v1[1],v2[2]-v1[2]};
  

//     printf("l:%lf %lf %lf %lf %lf %lf\n",l[0],l[1],l[2],l[3] );
//     printf("point dis line :%lf\n",distance_point_to_line(v1,l) );
//     normalize(l1);

//       printf("l1:%lf %lf %lf\n",l1[0],l1[1],l1[2] );
//     double * n3=out_product(l,l1);
//     if(!normalize(n3))
//     {
//        // printf("is sanem dir\n");
//         SAFE_FREE(n3);
//         return 1; 
//     } 
//     SAFE_FREE(n3);
//    return 0; 
// }

//is line intersecting with line segment


static inline int is_line_intersecting_with_line_segment(double* l,double* v1,double *v2)
{

    double d1[3]={v1[0]-l[3],v1[1]-l[4],v1[2]-l[5]};
    double d2[3]={v2[0]-l[3],v2[1]-l[4],v2[2]-l[5]};
 
    double * n1=out_product(l,d1);
    double* n2=out_product(l,d2);
    normalize(n1);
    normalize(n2);
   
    if(inner_product(n1,n2)<=1e-10)
    {
        free(n1);free(n2);
        return 1;
    }
    free(n1);free(n2);
    
    return 0; 
}

// @ l: the normalized line n1*t1+p1
// @ l1:the normalized line n2*t2+p2
// @ return the t1   

static inline double line_point_nearest_2_line(double* l,double* l1)
{
    double n1n2=inner_product(l,l1);
    double p1p2[3]={l[3]-l1[3],l[4]-l1[4],l[5]-l1[5]};

    return (n1n2* inner_product(p1p2,l1)-inner_product(p1p2,l))/(1-(n1n2)*(n1n2));
}

//@l :: t*n+p
//@poly::polygon
//@return t1
double * line_intersection_with_polygon(double* l,double** poly ,int len)
{
    double* re=(double*)malloc(sizeof(double)*2);

    int temp_i=0,mark=-1;
    for(int i=0;i<len;i++)
    {
        if(temp_i>=2)
        {
            break;
        }

       // printf("dis:%lf\n",distance_point_to_line(poly[i],l) );
        if(distance_point_to_line(poly[i],l)<1e-7)
        {
            mark=i;
            re[temp_i]=(poly[i][0]-l[3])/l[0];
            temp_i++;
           
        }
    }
 //   printf("temp_idd:%d\n",temp_i );
    for(int i=0;i<len;i++)
    {
        if(temp_i>=2)
        {
            break;
        }
        int j=(i+1)%len;
        if(i==mark||j==mark)
        {
            continue;
        } 
        //printf("point to line:%.8lf \n",distance_point_to_line(poly[i],l) );
     //distance_point_to_line 
        if(!is_line_intersecting_with_line_segment(l,poly[i],poly[j]))
        {
           // printf("not inter\n");
            continue;
        }
        double* l1=get_line_equation_from_two_p(poly[i],poly[j]);
        if(l1==NULL)
        {
            continue;
        } 
        double t= line_point_nearest_2_line(l,l1);
        SAFE_FREE(l1); 
        re[temp_i]=t;
        temp_i++;
        if(temp_i==2)
        {
            if(fabs(re[0]-t)<1e-8)
            {
                temp_i--;
            }

        } 
    }
   // printf("temp_i %d\n",temp_i);
    if(temp_i==0)
    {
        SAFE_FREE(re);
        return NULL;
    }
    else if(temp_i==1)
    {
      //  printf("there is one point\n");
        re[1]=re[0];
    }

    if(re[1]<re[0])
    {
        double tempd=re[0];
        re[0]=re[1];
        re[1]=tempd;
    }
    return re; 
}

static inline double * get_intersection_points_from_two_line_segments(double* l11,double* l12,double* l21,double* l22 )
{
    double *l1=get_line_equation_from_two_p(l11,l12);
    double* l2=get_line_equation_from_two_p(l21,l22);
    double* re=NULL;
    if(l1==NULL||l2==NULL)
    {
        SAFE_FREE(l1);SAFE_FREE(l2);
        return NULL;
    }

    if(is_line_intersecting_with_line_segment(l1,l21,l22)&&is_line_intersecting_with_line_segment(l2,l11,l12))
    {
        double t=line_point_nearest_2_line(l1,l2);
        re=(double*)malloc(sizeof(double)*3);
        re[0]=t*l1[0]+l1[3];re[1]=t*l1[1]+l1[4];re[2]=t*l1[2]+l1[5];
        return re;
    } 

    SAFE_FREE(l1);SAFE_FREE(l2);
    return NULL;
}
//在同一个平面上的两个多边形相交点
static inline double** get_intersection_points_from_two_polygons_on_one_plane(double **poly1,int len1,double** poly2,int len2)
{
    double ** re=(double**)malloc(sizeof(double*)*2);
    re[0]=NULL;re[1]=NULL;
    int temp_i=0;
    for(int i=0;i<len1;i++)
    {
        int i1=i,i2=(i+1)%len1;
        for(int j=0;j<len2;j++)
        {
            int j1=j,j2=(j+1)%len2;
            double *vv=get_intersection_points_from_two_line_segments(poly1[i1],poly1[i2],poly2[j1],poly2[j2]);

            if(vv!=NULL&&temp_i<2)
            {
                re[temp_i]=vv;
                temp_i++;
            } 
        }
    }
    return re;
}

//@return double [2][3];
// poly1 poly2 not in one plane(不在同一个平面上)
//if no intersections return NULL
double ** get_intersection_points_from_two_polygons(double**poly1,int len1,double**poly2 ,int len2)
{
    double * p1=get_triangle_plane_equation(poly1[0],poly1[1],poly1[2]);
    if(p1==NULL)
    {
        return NULL;
    }
    double * p2=get_triangle_plane_equation(poly2[0],poly2[1],poly2[2]);
   //  printf("p1 %.10lf %.10lf %.10lf %.10lf \n",p1[0],p1[1],p1[2],p1[3] );
   // printf("p2 %.10lf %.10lf %.10lf %.10lf\n",p2[0],p2[1],p2[2],p2[3] );
    if(p2==NULL)
    {
       // printf("p2==NULL\n");
        free(p1);
        
        return NULL;
    } 
    double *l =find_intersection_get_line_equation_from_2plane(p1,p2);
    if(l==NULL)
    {
       // printf("l==NULL\n");
        free(p1);free(p2);

        //return get_intersection_points_from_two_polygons_on_one_plane(poly1,len1,poly2,len2);

        return NULL;
    }
    //printf("l :%.10lf %.10lf %.10lf %.10lf %.10lf %.10lf\n",l[0],l[1],l[2],l[3],l[4],l[5] );
    double *t1=line_intersection_with_polygon(l,poly1,len1);
    if(t1==NULL)
    {
       // printf("t1=NULL\n");
        free(p1);free(p2);free(l);
        return NULL;
    }
    
  //  printf("t1 :%.10lf %.10lf\n",t1[0],t1[1]);
  //  printf("%lf %lf %lf %lf %lf %lf \n",t1[0]*l[0]+l[3], t1[0]*l[1]+l[4],t1[0]*l[2]+l[5], 
  //  t1[1]*l[0]+l[3], t1[1]*l[1]+l[4],t1[1]*l[2]+l[5]);
    double * t2=line_intersection_with_polygon(l,poly2,len2); 
    //printf("t2 :%lf %lf\n",t2[0],t2[1]);
    // printf("%lf %lf %lf %lf %lf %lf \n",t2[0]*l[0]+l[3], t2[0]*l[1]+l[4],t2[0]*l[2]+l[5], t2[1]*l[0]+l[3], t2[1]*l[1]+l[4],t2[1]*l[2]+l[5]);
//does not intersected
    if(t2==NULL||t2[0]>t1[1]||t2[1]<t1[0])
    {
        SAFE_FREE(t2);free(t1);free(p1);free(p2);free(l);
        return NULL;
    }

// // intersected
    
    double ** re=(double**)malloc(sizeof(double*)*2);
    double t3[2];
    for(int i=0;i<2;i++)
    {
        if(t2[0]>t1[0])
        {
            t3[0]=t2[0];
        }
        else{
            t3[0]=t1[0];
        }
        if(t2[1]<t1[1])
        {
            t3[1]=t2[1];
        }
        else
        {
            t3[1]=t1[1];
        }
    }
    for(int i=0;i<2;i++)
    {
        re[i]=(double*)malloc(sizeof(double)*3);
        for(int j=0;j<3;j++)
        {
            re[i][j]=t3[i]*l[j]+l[3+j];
        }
    } 
    SAFE_FREE(t1);SAFE_FREE(t2);SAFE_FREE(p1);SAFE_FREE(p2);SAFE_FREE(l);
    return re;

}
