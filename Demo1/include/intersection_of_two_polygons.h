#ifndef INTERSECTION_OF_TWO_POLYGONS_H_
#define INTERSECTION_OF_TWO_POLYGONS_H_

#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<Math/Viewer_Matrix4x4.h>


#define Matrix4x4 Viewer_Matrix4x4_
#ifndef SIGN
#define SIGN(x) ((x>0)?1:((x<0)?-1:0))
#endif
#ifndef SAFE_SQRT
#define SAFE_SQRT(x) x>=0?sqrt(x):0
#endif
#ifndef SAFE_FREE
#define SAFE_FREE(x) if(x!=NULL){free(x);x=NULL;}
#endif
#ifndef M_PI
#define M_PI 3.141592653589
#endif
#ifdef __cplusplus
extern "C" {
#endif
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
static inline int normalize(double *a)
{
    double norm=a[0]*a[0]+a[1]*a[1]+a[2]*a[2];
    
    norm=SAFE_SQRT(norm);
    //1e-12 doen't affect the result
    if(norm==0||fabs(norm)<1e-11)
    {
        return 0;
    } 
    a[0]/=norm;a[1]/=norm;a[2]/=norm;
    return 1;

}
//double*get_triangle_plane_equation(double* tr1,double* tr2,double*tr3);
//@ p1 is a plane equation
//@ p2 is a plane equation
//return t* n+ p:: line equation
//double* find_intersection_get_line_equation_from_2plane(double * p1,double *p2);
//@ l is line equation: x=t*n+p
//@ tri1 tri2 tri3 is three points
//@ return the two value t1 t2
//double* line_intersection_triangle(double* l,double* tri1,double* tri2,double* tri3);

//@return double [2][3];

// convex polygons 

double ** get_intersection_points_from_two_polygons(double**poly1,int len1,double**poly2 ,int len2);
#ifdef __cplusplus
}
#endif

#endif