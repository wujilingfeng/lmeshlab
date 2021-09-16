#ifndef SUBDIVISION_OF_POLYGON_H_
#define SUBDIVISION_OF_POLYGON_H_
#include<stdlib.h>
#include<stdio.h>
#ifndef SAFE_SQRT
#define SAFE_SQRT(x) x>=0?sqrt(x):sqrt(-x)
#endif
#ifndef SAFE_FREE
#define SAFE_FREE(x) if(x!=NULL){free(x);}
#endif
#ifndef M_PI
#define M_PI 3.141592653589
#endif
#ifdef __cplusplus
extern "C"{

#endif

int is_subdivision_of_polygon_acute_angle(double**v,int len,int an);


void subdivision_of_polygon(double ** v,int len,int ** s);

#ifdef __cplusplus
}
#endif

#endif