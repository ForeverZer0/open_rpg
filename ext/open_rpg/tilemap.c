

#include "./tmx/tmx.h"
#include "./internal.h"

// TODO: dispose

VALUE rb_cTilemap;
VALUE rb_cTileset;
VALUE rb_cTilemapLayer;
VALUE rb_cTilemapTile;
VALUE rb_cTilemapFrame;
VALUE rb_cTilemapImage;
VALUE rb_cTilemapObj;
VALUE rb_cTilemapObjGroup;

ALLOC_FUNC(rpg_tmx_map_alloc, tmx_map);
ALLOC_FUNC(rpg_tmx_tileset_alloc, tmx_tileset);
ALLOC_FUNC(rpg_tmx_layer_alloc, tmx_layer);
ALLOC_FUNC(rpg_tmx_tile_alloc, tmx_tile);
ALLOC_FUNC(rpg_tmx_anime_alloc, tmx_anim_frame);
ALLOC_FUNC(rpg_tmx_image_alloc, tmx_image);
ALLOC_FUNC(rpg_tmx_obj_alloc, tmx_object);
ALLOC_FUNC(rpg_tmx_objgroup_alloc, tmx_object_group);

static inline VALUE rpg_tilemap_prop_value(tmx_property *prop) {
    if (prop == NULL) {
        return Qnil;
    }
    switch (prop->type) {
        case PT_INT: return INT2NUM(prop->value.integer);
        case PT_FLOAT: return DBL2NUM(prop->value.decimal);
        case PT_BOOL: return RB_BOOL(prop->value.boolean);
        case PT_FILE:
        case PT_STRING: return rb_str_new2(prop->value.string);
        case PT_COLOR: {
            RPGcolor *color = ALLOC(RPGcolor);
            GLuint c = prop->value.color;
            color->a = ((c >> 24) & 0xFF) / 255.0f;
            color->r = ((c >> 16) & 0xFF) / 255.0f;
            color->g = ((c >> 8) & 0xFF) / 255.0f;
            color->b = ((c >> 0) & 0xFF) / 255.0f;
            return Data_Wrap_Struct(rb_cColor, NULL, RUBY_DEFAULT_FREE, color);
        }
        case PT_NONE:
        default: return Qnil;
    }
}

static void rpg_tilemap_prop_callback(tmx_property *prop, void *userdata) {
    VALUE hash = *((VALUE*) userdata); 
    VALUE key = STR2SYM(prop->name);
    rb_hash_aset(hash, key, rpg_tilemap_prop_value(prop));
}

static void *rpg_tilemap_image_load(const char *path) {
    
    RPGimage *img = ALLOC(RPGimage);
    void *pixels = rpg_image_load(path, &img->width, &img->height);
    if (pixels == NULL) {
        rb_raise(rb_eRPGError, "failed to load image - %s", path);
    }
    img->font = NULL;
    img->fbo = 0;

    glGenTextures(1, &img->texture);
    glBindTexture(GL_TEXTURE_2D, img->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    RPG_FREE(pixels);

    return img;
}

static void rpg_tilemap_image_free(void *image) {
    RPGimage *img = image;
    glDeleteTextures(1, &img->texture);
    glDeleteFramebuffers(1, &img->fbo);
    RPG_FREE(image);
}

ATTR_READER(rpg_tilemap_width, tmx_map, width, UINT2NUM)
ATTR_READER(rpg_tilemap_height, tmx_map, height, UINT2NUM)
ATTR_READER(rpg_tilemap_tile_width, tmx_map, tile_width, UINT2NUM)
ATTR_READER(rpg_tilemap_tile_height, tmx_map, tile_height, UINT2NUM)
ATTR_READER(rpg_tilemap_tile_count, tmx_map, tilecount, UINT2NUM)
ATTR_READER(rpg_tilemap_side_length, tmx_map, hexsidelength, INT2NUM)

#pragma region Tilemap

static VALUE rpg_tilemap_from_file(VALUE klass, VALUE path) {
    char *fname = StringValueCStr(path);
    if (!RPG_FILE_EXISTS(fname)) {
        VALUE enoent = rb_const_get(rb_mErrno, rb_intern("ENOENT"));
        rb_raise(enoent, "%s", fname);
    }
    tmx_map *map = tmx_load(fname);
    if (map == NULL) {
        const char *msg = tmx_strerr();
        rb_raise(rb_eRPGError, msg);
    }
    return Data_Wrap_Struct(rb_cTilemap, NULL, NULL, map);
}

static VALUE rpg_tilemap_back_color(VALUE self) {
    tmx_map *map = DATA_PTR(self);

    RPGcolor *color = ALLOC(RPGcolor);
    GLuint c = (GLuint) map->backgroundcolor;
    color->a = ((c >> 24) & 0xFF) / 255.0f;
    color->r = ((c >> 16) & 0xFF) / 255.0f;
    color->g = ((c >> 8) & 0xFF) / 255.0f;
    color->b = ((c >> 0) & 0xFF) / 255.0f;

    return Data_Wrap_Struct(rb_cColor, NULL, RUBY_DEFAULT_FREE, color);
}

static VALUE rpg_tilemap_size(VALUE self) {
    tmx_map *map = DATA_PTR(self);
    RPGsize *size = ALLOC(RPGsize);
    size->width = map->width;
    size->height = map->height;
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_tilemap_tile_size(VALUE self) {
    tmx_map *map = DATA_PTR(self);
    RPGsize *size = ALLOC(RPGsize);
    size->width = map->tile_width;
    size->height = map->tile_height;
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_tilemap_orientation(VALUE self) {
    tmx_map *map = DATA_PTR(self);
    enum tmx_map_orient orient = map->orient;
    switch (orient) {
        case O_ORT: return STR2SYM("orthogonal");
        case O_ISO: return STR2SYM("isometric");
        case O_STA: return STR2SYM("staggered");
        case O_HEX: return STR2SYM("hexagonal");
        case O_NONE:
        default: return STR2SYM("none");
    }
}

static VALUE rpg_tilemap_stagger_index(VALUE self) {
    tmx_map *map = DATA_PTR(self);
    enum tmx_stagger_index index = map->stagger_index;
    switch (index) {
        case SI_EVEN: return STR2SYM("even");
        case SI_ODD: return STR2SYM("odd");
        case SI_NONE:
        default: return STR2SYM("none");
    }
}

static VALUE rpg_tilemap_stagger_axis(VALUE self) {
    tmx_map *map = DATA_PTR(self);
    enum tmx_stagger_axis axis = map->stagger_axis;
    switch (axis) {
        case SA_X: return STR2SYM("x");
        case SA_Y: return STR2SYM("y");
        case SA_NONE:
        default: return STR2SYM("none");
    }
}

static VALUE rpg_tilemap_render_order(VALUE self) {
    tmx_map *map = DATA_PTR(self);
    enum tmx_map_renderorder order = map->renderorder;
    switch (order) {
        case R_RIGHTDOWN: return STR2SYM("rightdown");
        case R_RIGHTUP: return STR2SYM("rightup");
        case R_LEFTDOWN: return STR2SYM("leftdown");
        case R_LEFTUP: return STR2SYM("leftup");
        case R_NONE:
        default: return STR2SYM("none");
    }
}

static VALUE rpg_tilemap_tile(VALUE self, VALUE gid) {
    tmx_map *map = DATA_PTR(self);
    int i = NUM2INT(gid);
    if (i < 0 || i >= map->tilecount) {
        return Qnil;
    }
    tmx_tile *tile = map->tiles[i];
    return tile ? Data_Wrap_Struct(rb_cTilemapTile, NULL, NULL, tile) : Qnil;
}

static VALUE rpg_tilemap_property(VALUE self, VALUE key) {
    tmx_map *map = DATA_PTR(self);
    VALUE str = rb_sym2str(key);
    char *k = StringValueCStr(str);
    tmx_property *prop = tmx_get_property(map->properties, k);
    return rpg_tilemap_prop_value(prop);
}

static VALUE rpg_tilemap_properties(VALUE self) {
    tmx_map *map = DATA_PTR(self);
    VALUE hash = rb_hash_new();
    tmx_property_foreach(map->properties, rpg_tilemap_prop_callback, &hash);
    return hash;
}

static VALUE rpg_tilemap_tilesets(VALUE self) {
    tmx_map *map = DATA_PTR(self);
    VALUE ary = rb_ary_new();
    for (tmx_tileset_list *list = map->ts_head; list != NULL; list = list->next) {
        rb_ary_push(ary, Data_Wrap_Struct(rb_cTileset, NULL, NULL, list->tileset));
    }
    return ary;
}

static VALUE rpg_tilemap_layers(VALUE self) {
    tmx_map *map = DATA_PTR(self);
    VALUE ary = rb_ary_new();
    for (tmx_layer *layer = map->ly_head; layer != NULL; layer = layer->next) {
        rb_ary_push(ary, Data_Wrap_Struct(rb_cTilemapLayer, NULL, NULL, layer));
    }
    return ary;
}

static VALUE rpg_tilemap_each_tileset(VALUE self) {
    RETURN_ENUMERATOR(self, 0, NULL);
    tmx_map *map = DATA_PTR(self);
    for (tmx_tileset_list *list = map->ts_head; list != NULL; list = list->next) {
        rb_yield(Data_Wrap_Struct(rb_cTileset, NULL, NULL, list->tileset));
    }
    return self;
}

static VALUE rpg_tilemap_each_layer(VALUE self) {
    RETURN_ENUMERATOR(self, 0, NULL);
    tmx_map *map = DATA_PTR(self);
    for (tmx_layer *layer = map->ly_head; layer != NULL; layer = layer->next) {
        rb_yield(Data_Wrap_Struct(rb_cTilemapLayer, NULL, NULL, layer));
    }
    return self;
}

#pragma endregion Tilemap

#pragma region Tile

static VALUE rpg_tmx_tile_inspect(VALUE self) {
    tmx_tile *tile = DATA_PTR(self);
    return rb_sprintf("<Tile: id:%u x:%d y:%u>", tile->id, tile->ul_x, tile->ul_y);
}

ATTR_READER(rpg_tmx_tile_id, tmx_tile, id, UINT2NUM)
ATTR_READER(rpg_tmx_tile_x, tmx_tile, ul_x, UINT2NUM)
ATTR_READER(rpg_tmx_tile_y, tmx_tile, ul_y, UINT2NUM)
ATTR_READER(rpg_tmx_tile_frame_count, tmx_tile, animation_len, UINT2NUM)
ATTR_READER(rpg_tmx_tile_type, tmx_tile, type, rb_str_new_cstr)

static VALUE rpg_tmx_tile_image(VALUE self) {
    tmx_tile *tile = DATA_PTR(self);
    return tile->image ? Data_Wrap_Struct(rb_cTilemapImage, NULL, NULL, tile->image) : Qnil;
}

static VALUE rpg_tmx_tile_collision(VALUE self) {
    tmx_tile *tile = DATA_PTR(self);
    return tile->collision ? Data_Wrap_Struct(rb_cTilemapObj, NULL, NULL, tile->collision) : Qnil;
}

static VALUE rpg_tmx_tile_property(VALUE self, VALUE key) {
    tmx_tile *tile = DATA_PTR(self);
    VALUE str = rb_sym2str(key);
    char *k = StringValueCStr(str);
    tmx_property *prop = tmx_get_property(tile->properties, k);
    return rpg_tilemap_prop_value(prop);
}

static VALUE rpg_tmx_tile_properties(VALUE self) {
    tmx_tile *tile = DATA_PTR(self);
    VALUE hash = rb_hash_new();
    tmx_property_foreach(tile->properties, rpg_tilemap_prop_callback, &hash);
    return hash;
}

static VALUE rpg_tmx_tile_tileset(VALUE self) {
    tmx_tile *tile = DATA_PTR(self);
    return tile->tileset ? Data_Wrap_Struct(rb_cTileset, NULL, NULL, tile->tileset) : Qnil;
}

static VALUE rpg_tmx_tile_animated_p(VALUE self) {
    tmx_tile *tile = DATA_PTR(self);
    return RB_BOOL(tile->animation_len);
}

static VALUE rpg_tmx_tile_frames(VALUE self) {
    tmx_tile *tile = DATA_PTR(self);
    long count = (long) tile->animation_len;
    tmx_anim_frame frame;
    VALUE frames = rb_ary_new_capa(count);
    for (long i = 0; i < count; i++) {
        VALUE frame = Data_Wrap_Struct(rb_cTilemapFrame, NULL, NULL, &tile->animation[i]);
        rb_ary_store(frames, i, frame);
    }
    return frames;
}

static VALUE rpg_tmx_tile_each_frame(VALUE self) {
    RETURN_ENUMERATOR(self, 0, NULL);
    tmx_tile *tile = DATA_PTR(self);
    long count = (long) tile->animation_len;
    for (long i = 0; i < count; i++) {
        rb_yield(Data_Wrap_Struct(rb_cTilemapFrame, NULL, NULL, &tile->animation[i]));
    }
    return self;
}

#pragma endregion Tile

#pragma region Frame

ATTR_READER(rpg_tmx_frame_id, tmx_anim_frame, tile_id, UINT2NUM)
ATTR_READER(rpg_tmx_frame_duration, tmx_anim_frame, duration, UINT2NUM);

static VALUE rpg_tmx_frame_inspect(VALUE self) {
    tmx_anim_frame *frame = DATA_PTR(self);
    return rb_sprintf("<Frame: tile_id:%u duration:%u>", frame->tile_id, frame->duration);
}

#pragma endregion Frame





void rpg_tilemap_init(VALUE parent) {
    tmx_alloc_func = RPG_REALLOC;
    tmx_free_func = RPG_FREE;
    tmx_img_load_func = rpg_tilemap_image_load;
    tmx_img_free_func = rpg_tilemap_image_free;

    rb_cTilemap = rb_define_class_under(parent, "Tilemap", rb_cObject);
    rb_cTileset = rb_define_class_under(rb_cTilemap, "Tileset", rb_cObject);
    rb_cTilemapLayer = rb_define_class_under(rb_cTilemap, "Layer", rb_cObject);
    rb_cTilemapTile = rb_define_class_under(rb_cTilemap, "Tile", rb_cObject);
    rb_cTilemapFrame = rb_define_class_under(rb_cTilemapTile, "Frame", rb_cObject);
    rb_cTilemapImage = rb_define_class_under(rb_cTilemap, "Image", rb_cObject);
    rb_cTilemapObj = rb_define_class_under(rb_cTilemap, "Object", rb_cObject);
    rb_cTilemapObjGroup = rb_define_class_under(rb_cTilemap, "Group", rb_cObject);

    rb_define_alloc_func(rb_cTilemap, rpg_tmx_map_alloc);
    rb_define_alloc_func(rb_cTileset, rpg_tmx_tileset_alloc);
    rb_define_alloc_func(rb_cTilemapLayer, rpg_tmx_layer_alloc);
    rb_define_alloc_func(rb_cTilemapTile, rpg_tmx_tile_alloc);
    rb_define_alloc_func(rb_cTilemapFrame, rpg_tmx_layer_alloc);
    rb_define_alloc_func(rb_cTilemapTile, rpg_tmx_tile_alloc);
    rb_define_alloc_func(rb_cTilemapObj, rpg_tmx_obj_alloc);
    rb_define_alloc_func(rb_cTilemapObjGroup, rpg_tmx_objgroup_alloc);

    // Tilemap
    rb_define_singleton_method(rb_cTilemap, "from_file", rpg_tilemap_from_file, 1);
    rb_define_method(rb_cTilemap, "width", rpg_tilemap_width, 0);
    rb_define_method(rb_cTilemap, "height", rpg_tilemap_height, 0);
    rb_define_method(rb_cTilemap, "size", rpg_tilemap_size, 0);
    rb_define_method(rb_cTilemap, "tile_width", rpg_tilemap_tile_width, 0);
    rb_define_method(rb_cTilemap, "tile_height", rpg_tilemap_tile_height, 0);
    rb_define_method(rb_cTilemap, "tile_size", rpg_tilemap_tile_size, 0);
    rb_define_method(rb_cTilemap, "back_color", rpg_tilemap_back_color, 0);
    rb_define_method(rb_cTilemap, "orientation", rpg_tilemap_orientation, 0);
    rb_define_method(rb_cTilemap, "stagger_axis", rpg_tilemap_stagger_axis, 0);
    rb_define_method(rb_cTilemap, "stagger_index", rpg_tilemap_stagger_index, 0);
    rb_define_method(rb_cTilemap, "render_order", rpg_tilemap_render_order, 0);
    rb_define_method(rb_cTilemap, "tile_count", rpg_tilemap_tile_count, 0);
    rb_define_method(rb_cTilemap, "side_length", rpg_tilemap_side_length, 0);
    rb_define_method(rb_cTilemap, "tilesets", rpg_tilemap_tilesets, 0);
    rb_define_method(rb_cTilemap, "layers", rpg_tilemap_layers, 0);
    rb_define_method(rb_cTilemap, "each_tileset", rpg_tilemap_each_tileset, 0);
    rb_define_method(rb_cTilemap, "each_layer", rpg_tilemap_each_layer, 0);
    rb_define_method(rb_cTilemap, "property", rpg_tilemap_property, 1);
    rb_define_method(rb_cTilemap, "properties", rpg_tilemap_properties, 0);
    rb_define_method(rb_cTilemap, "[]", rpg_tilemap_tile, 1);

    // Tile
    rb_define_method(rb_cTilemapTile, "tileset", rpg_tmx_tile_tileset, 0);
    rb_define_method(rb_cTilemapTile, "inspect", rpg_tmx_tile_inspect, 0);
    rb_define_method(rb_cTilemapTile, "id", rpg_tmx_tile_id, 0);
    rb_define_method(rb_cTilemapTile, "x", rpg_tmx_tile_x, 0);
    rb_define_method(rb_cTilemapTile, "y", rpg_tmx_tile_y, 0);
    rb_define_method(rb_cTilemapTile, "animated?", rpg_tmx_tile_animated_p, 0);
    rb_define_method(rb_cTilemapTile, "frame_count", rpg_tmx_tile_frame_count, 0);
    rb_define_method(rb_cTilemapTile, "frames", rpg_tmx_tile_frames, 0);
    rb_define_method(rb_cTilemapTile, "each_frame", rpg_tmx_tile_each_frame, 0);
    rb_define_method(rb_cTilemapTile, "property", rpg_tmx_tile_property, 1);
    rb_define_method(rb_cTilemapTile, "properties", rpg_tmx_tile_properties, 0);
    rb_define_method(rb_cTilemapTile, "image", rpg_tmx_tile_image, 0);
    rb_define_method(rb_cTilemapTile, "type", rpg_tmx_tile_type, 0);
    rb_define_method(rb_cTilemapTile, "collision", rpg_tmx_tile_collision, 0);

    // Frame
    rb_define_method(rb_cTilemapFrame, "tile_id", rpg_tmx_frame_id, 0);
    rb_define_method(rb_cTilemapFrame, "duration", rpg_tmx_frame_duration, 0);
    rb_define_method(rb_cTilemapFrame, "inspect", rpg_tmx_frame_inspect, 0);

    // Tileset
}

