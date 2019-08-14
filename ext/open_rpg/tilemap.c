#include "tilemap.h"
#include "./internal.h"

VALUE rb_cTilemap;
VALUE rb_cTileset;
VALUE rb_cTilemapLayer;
VALUE rb_cTilemapTile;
VALUE rb_cTilemapFrame;
VALUE rb_cTilemapImage;
VALUE rb_cTilemapObj;
VALUE rb_cTilemapObjGroup;
VALUE rb_cTilemapProperty;

void rpg_tilemap_init(VALUE parent) {
    rb_cTilemap = rb_define_class_under(parent, "Tilemap", rb_cObject);
    rb_cTileset = rb_define_class_under(rb_cTilemap, "Tileset", rb_cObject);
    rb_cTilemapLayer = rb_define_class_under(rb_cTilemap, "Layer", rb_cObject);
    rb_cTilemapTile = rb_define_class_under(rb_cTilemap, "Tile", rb_cObject);
    rb_cTilemapFrame = rb_define_class_under(rb_cTilemapTile, "Frame", rb_cObject);
    rb_cTilemapImage = rb_define_class_under(rb_cTilemap, "Image", rb_cObject);
    rb_cTilemapObj = rb_define_class_under(rb_cTilemap, "Object", rb_cObject);
    rb_cTilemapObjGroup = rb_define_class_under(rb_cTilemap, "Group", rb_cObject);
    rb_cTilemapProperty = rb_define_class_under(rb_cTilemap, "Property", rb_cObject);

    rb_define_alloc_func(rb_cTilemap, rpg_tmx_map_alloc);
    rb_define_alloc_func(rb_cTileset, rpg_tmx_tileset_alloc);
    rb_define_alloc_func(rb_cTilemapLayer, rpg_tmx_layer_alloc);
    rb_define_alloc_func(rb_cTilemapTile, rpg_tmx_tile_alloc);
    rb_define_alloc_func(rb_cTilemapFrame, rpg_tmx_layer_alloc);
    rb_define_alloc_func(rb_cTilemapTile, rpg_tmx_tile_alloc);
    rb_define_alloc_func(rb_cTilemapObj, rpg_tmx_obj_alloc);
    rb_define_alloc_func(rb_cTilemapObjGroup, rpg_tmx_objgroup_alloc);
    rb_define_alloc_func(rb_cTilemapProperty, rpg_tmx_prop_alloc);




    tmx_alloc_func = RPG_REALLOC;
    tmx_free_func = RPG_FREE;

    rb_define_singleton_method(rb_cTilemap, "from_file", rpg_tilemap_from_file, 1);
}

ALLOC_FUNC(rpg_tmx_map_alloc, tmx_map);
ALLOC_FUNC(rpg_tmx_tileset_alloc, tmx_tileset);
ALLOC_FUNC(rpg_tmx_layer_alloc, tmx_layer);
ALLOC_FUNC(rpg_tmx_tile_alloc, tmx_tile);
ALLOC_FUNC(rpg_tmx_anime_alloc, tmx_anim_frame);
ALLOC_FUNC(rpg_tmx_image_alloc, tmx_image);
ALLOC_FUNC(rpg_tmx_obj_alloc, tmx_object);
ALLOC_FUNC(rpg_tmx_objgroup_alloc, tmx_object_group);
ALLOC_FUNC(rpg_tmx_prop_alloc, tmx_property);

static VALUE rpg_tilemap_from_file(VALUE klass, VALUE path) {
    

    return Qnil;
}