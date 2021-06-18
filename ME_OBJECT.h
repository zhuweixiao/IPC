#ifndef ME_OBJECT_H_
#define ME_OBJECT_H_

typedef signed char uint8;

typedef struct {
    uint8   OBJ_ID;
    float   OBJ_Width;
    float   OBJ_Height;
    uint8   Reserved[14];
} ME_OBJECT;

#endif