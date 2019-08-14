#ifndef OPEN_RPG_TILEMAP
#define OPEN_RPG_TILEMAP 1

#include "./rpg.h"
#include "./tmx/tmx.h"

void rpg_tilemap_init(VALUE parent);
static VALUE rpg_tmx_map_alloc(VALUE klass);
static VALUE rpg_tmx_tileset_alloc(VALUE klass);
static VALUE rpg_tmx_layer_alloc(VALUE klass);
static VALUE rpg_tmx_tile_alloc(VALUE klass);
static VALUE rpg_tmx_anime_alloc(VALUE klass);
static VALUE rpg_tmx_image_alloc(VALUE klass);
static VALUE rpg_tmx_obj_alloc(VALUE klass);
static VALUE rpg_tmx_objgroup_alloc(VALUE klass);
static VALUE rpg_tmx_prop_alloc(VALUE klass);

static VALUE rpg_tilemap_from_file(VALUE klass, VALUE path);

#endif /* OPEN_RPG_TILEMAP */