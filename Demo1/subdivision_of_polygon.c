#include "include/subdivision_of_polygon.h"
static inline int normalize(double *a)
{
    double norm=a[0]*a[0]+a[1]*a[1]+a[2]*a[2];
    if(norm==0||fabs(norm)<1e-9)
    {
       return 0; 
    }
    norm=SAFE_SQRT(norm);
    if(norm==0||fabs(norm)<1e-9)
    {
        return 0;
    } 
    a[0]/=norm;a[1]/=norm;a[2]/=norm;
    return 1;
}

static inline double *out_product(double *a,double*b)
{
    double* re=(double*)malloc(sizeof(double)*3);
    
    re[0]=a[1]*b[2]-a[2]*b[1];
    re[1]=-a[0]*b[2]+a[2]*b[0];
    re[2]=a[0]*b[1]-a[1]*b[0];
    return re; 
}
static inline double inner_product(double* a,double* b)
{ 
    return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}
// is line intersecting with line on one point 
static inline int is_line_intersecting_with_line_segment(double* l,double* v1,double *v2)
{
    // is same dir
    double l1[3]={v2[0]-v1[0],v2[1]-v1[1],v2[2]-v1[2]};
    double * n3=out_product(l,l1);
    if(!normalize(n3))
    {
        SAFE_FREE(n3);
       return 0; 
    } 
    SAFE_FREE(n3);
//is same point
    if(fabs(v1[0]-l[3])<=1e-9&&fabs(v1[1]-l[4])<=1e-9&&fabs(v1[2]-l[5])<=1e-9)
    {
        return 1;
    }
    if(fabs(v2[0]-l[3])<=1e-9&&fabs(v2[1]-l[4])<=1e-9&&fabs(v2[2]-l[5])<=1e-9)
    {
        return 1;
    }
    double d1[3]={v1[0]-l[3],v1[1]-l[4],v1[2]-l[5]};
    double d2[3]={v2[0]-l[3],v2[1]-l[4],v2[2]-l[5]};

    double * n1=out_product(l,d1);
    double* n2=out_product(l,d2);
    //printf("n2 %lf %lf %lf\n",n2[0],n2[1],n2[2] ); 
    //printf("n1 %lf %lf %lf\n",n1[0],n1[1],n1[2] ); 
    if(inner_product(n1,n2)<=1e-9)
    {
        free(n1);free(n2);
        return 1;
    }
    //printf("zhengque\n");
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

int is_subdivision_of_polygon_acute_angle(double **v,int len,int an)
{
    double *l=NULL;
    int ia=(an-1+len)%len,ib=(an+1)%len;
    l=get_line_equation_from_two_p(v[ia],v[ib]);
    if(l==NULL)
    {
        return 1;
    }
    //printf("l:%lf %lf %lf %lf %lf %lf\n",l[0],l[1],l[2],l[3],l[4],l[5] ); 
    int temp_i=0,temp_j=0;
    int flag=-1,flag1=0;
  
    double temp1[3]={0},temp2[3]={0}; 
    for(int i=0;i<len-1;i++)
    {
        temp_i=(i+an)%len;
        temp_j=(temp_i+1)%len;

        //printf("once:tempi:%d tempj:%d  \n",temp_i,temp_j);
        int libo=is_line_intersecting_with_line_segment(l,v[temp_i],v[temp_j]);
        if(libo==1)
        {
            //printf("libo***************************************libo :%d\n",libo );
            double *l1=get_line_equation_from_two_p(v[temp_i],v[temp_j]);
            if(l1==NULL)
            {
                continue;
            } 
          
            double value=line_point_nearest_2_line(l,l1);
            if(flag1==0)
            {
              //  printf("here value :%lf\n",fabs(value*l[2]+l[5]-v[temp_j][2]));
                if(fabs(value*l[0]+l[3]-v[temp_j][0])<=1e-9&&fabs(value*l[1]+l[4]-v[temp_j][1])<=1e-9
                    &&fabs(value*l[2]+l[5]-v[temp_j][2])<=1e-9)
                {
                    temp1[0]=v[temp_i][0]-l[3];temp1[1]=v[temp_i][1]-l[4];temp1[2]=v[temp_i][2]-l[5];
                    flag1=1;
                //    printf("begin flag1\n");
                }
                else
                {
                    if(value>1e-9)
                    {
                     //   printf("%lf\n",value );
                        flag++;
                    } 
                }
            } 
            else
            {

                temp2[0]=v[temp_j][0]-l[3];temp2[1]=v[temp_j][1]-l[4];temp2[2]=v[temp_j][2]-l[5];
                double* temp_n1=out_product(l,temp1),*temp_n2=out_product(l,temp2); 
                if(inner_product(temp_n1,temp_n2)<=1e-9&&value>1e-9)      
                {
                    flag++; 
                   // printf("tongguo%d\n",flag);
                }
                else
                {

                }
                flag1=0;
                free(temp_n1);free(temp_n2); 
            }

            free(l1);
            //printf("flag :%d\n",flag );
        } 
    } 

    free(l);

   // printf("my end\n");
    if(flag%2==0)
    {
        return 1;
    }
    return 0;
}


typedef struct Sub_Poly_Ver{
    double* p;
    int i;
}Sub_Poly_Ver;
static inline sub_poly_ver_init(Sub_Poly_Ver* spv)
{
    spv->p=NULL;
    spv->i=0;
}
static void subdivision_of_polygon2( Sub_Poly_Ver* spvs,int len,int **s)
{
    if(len<3)
    {
        return ;
    }
    if(len==3)
    {
        s[0][0]=spvs[0].i;
        s[0][1]=spvs[1].i;
        s[0][2]=spvs[2].i;
        return ;
    }
    int an=0;
    double** v=(double**)malloc(sizeof(double*)*len);
    for(int i=0;i<len;i++)
    {
        v[i]=spvs[i].p;
    } 
    for(int i=0;i<len;i++)
    {
        if(is_subdivision_of_polygon_acute_angle(v,len,i))
        {
            an=i;
            break;
        } 
    }
   // printf("8****************************************************8an an:%d\n",an );
    free(v);
   // printf("here1\n");
    int ia=(an-1+len)%len,ib=(an+1+len)%len;
    
    s[0][0]=spvs[ia].i;
    s[0][1]=spvs[an].i;
    s[0][2]=spvs[ib].i;
    
    int temp_i=0;
    Sub_Poly_Ver* spvs1=(Sub_Poly_Ver*)malloc(sizeof(Sub_Poly_Ver)*(len-1));
    for(int i=0;i<len;i++)
    {
        if(i==an)
        {
            continue;
        }
        spvs1[temp_i]=spvs[i];
      
     
        temp_i++;
       
    }
    subdivision_of_polygon2(spvs1,len-1,&(s[1]));
    free(spvs1);

}
void subdivision_of_polygon(double ** v,int len,int ** s)
{
    Sub_Poly_Ver* spvs=(Sub_Poly_Ver*)malloc(sizeof(Sub_Poly_Ver)*len);
    for(int i=0;i<len;i++)
    {
        spvs[i].p=v[i];
        spvs[i].i=i;
    } 
    subdivision_of_polygon2(spvs,len,s); 
    free(spvs);

}