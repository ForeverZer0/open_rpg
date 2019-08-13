#include "tilemap.h"

VALUE rb_cTilemap;

void rpg_tilemap_init(VALUE parent) {
    rb_define_class_under(parent, "Tilemap", rb_cObject);
    
}