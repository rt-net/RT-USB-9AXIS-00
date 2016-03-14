#ifndef TINYMATHFUNCTION_H
#define TINYMATHFUNCTION_H

#define PI (3.1415926f)

float tInvSqrt(float x);
float tSqrt(float x);

void quickSort(int numbers[], int left, int right);


#define ABS(IN) ((IN) < 0 ? - (IN) : (IN))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define SWAP(a,b) ((a != b) && (a += b,b = a - b,a -= b))
#define MAX3(a, b, c) ((a) > (MAX(b, c)) ? (a) : (MAX(b, c)))
#define MAX4(a, b, c, d) ((a) > (MAX3(b, c, d)) ? (a) : (MAX3(b, c, d)))
#define MIN3(a, b, c) ((a) < (MIN(b, c)) ? (a) : (MIN(b, c)))
#define MIN4(a, b, c, d) ((a) < (MIN3(b, c, d)) ? (a) : (MIN3(b, c, d)))
#define SIGN(x) ((x<0)?(-1):((x>0)?1:0))
#define DEG2RAD(deg) (((deg) * PI) / 180.0)
#define RAD2DEG(rad) (((rad) * 180.0) / 3.1415926535)



#endif
