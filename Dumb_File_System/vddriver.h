#ifndef _VDDRIVER
#define _VDDRIVER

#ifndef _VDREAD
#define _VDREAD
int vdread(int, char*, int, int);
#endif

#ifndef _VDWRITE
#define _VDWRITE
int vdwrite(int, char*, int , int);
#endif

#endif