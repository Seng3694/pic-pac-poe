#ifndef MY_TEXTURE_H
#define MY_TEXTURE_H

typedef enum {
  MY_TEXTURE_ID_00,
  MY_TEXTURE_ID_02,
  MY_TEXTURE_ID_03,
  MY_TEXTURE_ID_17,
  MY_TEXTURE_ID_19,
  MY_TEXTURE_ID_22,
  MY_TEXTURE_ID_29,
  MY_TEXTURE_ID_30
} my_texture_id;

struct {
  int left, top, width, height;
} const static my_texture_rects[] = {
    {32, 96, 8, 8},  {40, 96, 8, 8},  {32, 104, 8, 8},  {16, 96, 16, 16},
    {0, 96, 16, 16}, {0, 64, 32, 32}, {32, 64, 32, 32}, {0, 0, 64, 64},
};

#endif
