#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "stb_image.h"
#include "stb_image_write.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "cli.h"

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
} color;

typedef struct {
  int width;
  int height;
  char name[256];
  color *data;
} texture;

static bool load_texture(const char *file, texture *t) {
  t->data = (color *)stbi_load(file, &t->width, &t->height, NULL, 4);
  memcpy(t->name, file, strlen(file) & 0xff);
  return t->data != NULL;
}

static texture *read_textures_from_file(size_t *length, size_t *capacity,
                                        FILE *file) {
  char buffer[256];
  texture *textures = malloc(sizeof(texture) * (*capacity));
  while (fgets(buffer, sizeof(buffer), file)) {
    texture t = {0};
    buffer[strcspn(buffer, "\n")] = '\0';
    if (!load_texture(buffer, &t)) {
      printf("error loading %s\n", buffer);
    } else {
      if (*length == *capacity) {
        (*capacity) *= 2;
        textures = realloc(textures, sizeof(texture) * (*capacity));
      }
      textures[(*length)++] = t;
    }
  }
  return textures;
}

typedef struct {
  int textureId;
  int left;
  int top;
  int width;
  int height;
} rectangle;

static void push_rectangle(lua_State *L, const rectangle *const r) {
  lua_createtable(L, 0, 5);
  lua_pushinteger(L, r->textureId);
  lua_setfield(L, -2, "textureId");
  lua_pushinteger(L, r->left);
  lua_setfield(L, -2, "left");
  lua_pushinteger(L, r->top);
  lua_setfield(L, -2, "top");
  lua_pushinteger(L, r->width);
  lua_setfield(L, -2, "width");
  lua_pushinteger(L, r->height);
  lua_setfield(L, -2, "height");
}

static rectangle *pack(const texture *const textures, const size_t length,
                       const int maxWidth, const char *script, int *const width,
                       int *const height) {
  rectangle *rects = malloc(sizeof(rectangle) * length);
  for (size_t i = 0; i < length; ++i)
    rects[i] = (rectangle){(int)i, 0, 0, textures[i].width, textures[i].height};

  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  int result = luaL_dofile(L, script);
  if (result) {
    fprintf(stderr, "couldn't load file: %s\n", lua_tostring(L, -1));
    free(rects);
    lua_close(L);
    return NULL;
  }

  lua_getglobal(L, "Pack");

  lua_newtable(L);
  for (size_t i = 0; i < length; ++i) {
    push_rectangle(L, &rects[i]);
    lua_rawseti(L, -2, i + 1);
  }

  lua_pushinteger(L, maxWidth);

  if (lua_pcall(L, 2, 3, 0) != 0) {
    fprintf(stderr, "failed to run script: %s\n", lua_tostring(L, -1));
    free(rects);
    lua_close(L);
    return NULL;
  }

  *height = lua_tonumber(L, -1);
  lua_pop(L, 1);

  *width = lua_tonumber(L, -1);
  lua_pop(L, 1);

  for (int i = 1; i <= (int)length; ++i) {
    lua_rawgeti(L, -1, i);

    lua_getfield(L, -1, "textureId");
    int textureIndex = lua_tonumber(L, -1);
    lua_getfield(L, -2, "left");
    rects[textureIndex].left = lua_tonumber(L, -1);
    lua_getfield(L, -3, "top");
    rects[textureIndex].top = lua_tonumber(L, -1);

    lua_pop(L, 4);
  }

  lua_pop(L, 1);

  lua_close(L);

  return rects;
}

static void write_texture(color *const data, const texture *const t,
                          const int width, const int x, const int y) {
  for (size_t y1 = 0; y1 < t->height; ++y1) {
    for (size_t x1 = 0; x1 < t->width; ++x1) {
      const size_t to = ((y1 + y) * width) + (x1 + x);
      const size_t from = (y1 * t->width) + x1;
      data[to] = t->data[from];
    }
  }
}

static void create_texture(const char *file, const texture *const textures,
                           const rectangle *const rectangles,
                           const size_t length, const int width,
                           const int height) {
  color *data = calloc(width * height, sizeof(color));

  for (size_t i = 0; i < length; ++i) {
    const rectangle *const r = &rectangles[i];
    write_texture(data, &textures[r->textureId], width, r->left, r->top);
  }

  stbi_write_png(file, width, height, 4, data, sizeof(color) * width);

  free(data);
}

static void export(const texture *const textures,
                   const rectangle *const rectangles, const size_t length,
                   const char *script, const char *outputFile) {
  // push a list of tables with left, top, width, height and name
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  int result = luaL_dofile(L, script);
  if (result) {
    fprintf(stderr, "couldn't load file: %s\n", lua_tostring(L, -1));
    lua_close(L);
    return;
  }

  lua_getglobal(L, "Export");

  lua_newtable(L);
  for (size_t i = 0; i < length; ++i) {
    const rectangle *const r = &rectangles[i];
    const texture *const t = &textures[r->textureId];
    lua_createtable(L, 0, 5);

    lua_pushinteger(L, r->left);
    lua_setfield(L, -2, "left");
    lua_pushinteger(L, r->top);
    lua_setfield(L, -2, "top");
    lua_pushinteger(L, r->width);
    lua_setfield(L, -2, "width");
    lua_pushinteger(L, r->height);
    lua_setfield(L, -2, "height");
    lua_pushstring(L, t->name);
    lua_setfield(L, -2, "name");

    lua_rawseti(L, -2, i + 1);
  }

  lua_pushstring(L, outputFile);

  if (lua_pcall(L, 2, 0, 0) != 0) {
    fprintf(stderr, "failed to run script: %s\n", lua_tostring(L, -1));
    lua_close(L);
    return;
  }

  lua_close(L);
}

int main(const int argc, const char *const argv[const]) {
  size_t length = 0;
  size_t capacity = 256;

  args a = {
      .packerScript = "scripts/bin_pack.lua",
      .packerOutputFile = "out.png",
      .exporterScript = "scripts/header_export.lua",
      .exporterOutputFile = "out.h",
  };

  const parse_result result = parse_args(argc, argv, &a);

  if (result == PARSE_RESULT_FAIL)
    return 1;
  else if (result == PARSE_RESULT_EXIT)
    return 0;

  texture *textures = read_textures_from_file(&length, &capacity, stdin);

  int w, h;
  rectangle *rectangles =
      pack(textures, length, a.widthConstraint, a.packerScript, &w, &h);

  if (rectangles) {
    create_texture(a.packerOutputFile, textures, rectangles, length, w, h);
    export(textures, rectangles, length, a.exporterScript,
           a.exporterOutputFile);
    free(rectangles);
  }

  for (size_t i = 0; i < length; ++i)
    stbi_image_free(textures[i].data);
  free(textures);
}
