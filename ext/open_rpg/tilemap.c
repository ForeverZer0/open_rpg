#include "./tilemap.h"

VALUE rb_cTilemap;
VALUE rb_cTileset;

void rpg_tilemap_init(VALUE parent) {

    rb_cTilemap = rb_define_class_under(parent, "Tilemap", rb_cObject);
    rb_cTileset = rb_define_class_under(parent, "Tileset", rb_cObject);

    

}