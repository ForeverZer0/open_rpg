

#include "./tmx/tmx.h"
#include "./internal.h"

tmx_resource_manager *cache;

VALUE rb_cTilemap;
VALUE rb_cTileset;
VALUE rb_cLayer;
VALUE rb_cTile;
VALUE rb_cFrame;
VALUE rb_cImageInfo;
VALUE rb_cGroup;
VALUE rb_cMapObject;
VALUE rb_cShape;
VALUE rb_cText;
VALUE rb_cTemplate;

ALLOC_FUNC(rpg_tmx_map_alloc, tmx_map);
ALLOC_FUNC(rpg_tmx_tileset_alloc, tmx_tileset);
ALLOC_FUNC(rpg_tmx_layer_alloc, tmx_layer);
ALLOC_FUNC(rpg_tmx_tile_alloc, tmx_tile);
ALLOC_FUNC(rpg_tmx_anime_alloc, tmx_anim_frame);
ALLOC_FUNC(rpg_tmx_image_alloc, tmx_image);
ALLOC_FUNC(rpg_tmx_obj_alloc, tmx_object);
ALLOC_FUNC(rpg_tmx_objgroup_alloc, tmx_object_group);
ALLOC_FUNC(rpg_tmx_template_alloc, tmx_template)

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
    RPG_THROW_UNLESS_FILE(path);
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
    rpg_image_free(image);
}

static inline VALUE rpg_tmx_obj_class(enum tmx_obj_type type) {
    switch (type) {
        case OT_SQUARE:
        case OT_POLYGON:
        case OT_POLYLINE:
        case OT_ELLIPSE:
        case OT_POINT: 
            return rb_cShape;
        case OT_TEXT: return rb_cText;
        case OT_TILE: 
        case OT_NONE:
        default: return rb_cMapObject;
    }
}

static VALUE rpg_tmx_clear_cache(VALUE module) {
    tmx_free_resource_manager(cache);
    cache = tmx_make_resource_manager();
    return Qnil;
}

static VALUE rpg_tmx_cache_tileset(VALUE klass, VALUE path) {
    char *fname = StringValueCStr(path);
    RPG_THROW_UNLESS_FILE(fname);
    int result = tmx_load_tileset(cache, fname);
    return RB_BOOL(result);
}

static VALUE rpg_tmx_cache_template(VALUE klass, VALUE path) {
    char *fname = StringValueCStr(path);
    RPG_THROW_UNLESS_FILE(fname);
    int result = tmx_load_template(cache, fname);
    return RB_BOOL(result);
}

#pragma region Map

ATTR_READER(rpg_tmx_map_width, tmx_map, width, UINT2NUM)
ATTR_READER(rpg_tmx_map_height, tmx_map, height, UINT2NUM)
ATTR_READER(rpg_tmx_map_tile_width, tmx_map, tile_width, UINT2NUM)
ATTR_READER(rpg_tmx_map_tile_height, tmx_map, tile_height, UINT2NUM)
ATTR_READER(rpg_tmx_map_tile_count, tmx_map, tilecount, UINT2NUM)
ATTR_READER(rpg_tmx_map_side_length, tmx_map, hexsidelength, INT2NUM)

static VALUE rpg_tmx_map_from_file(VALUE klass, VALUE path) {
    char *fname = StringValueCStr(path);
    RPG_THROW_UNLESS_FILE(fname);
    tmx_map *map = tmx_rcmgr_load(cache, fname);
    if (map == NULL) {
        const char *msg = tmx_strerr();
        rb_raise(rb_eRPGError, msg);
    }
    return Data_Wrap_Struct(klass, NULL, NULL, map);
}

static VALUE rpg_tmx_map_back_color(VALUE self) {
    tmx_map *map = DATA_PTR(self);

    RPGcolor *color = ALLOC(RPGcolor);
    GLuint c = (GLuint) map->backgroundcolor;
    color->a = ((c >> 24) & 0xFF) / 255.0f;
    color->r = ((c >> 16) & 0xFF) / 255.0f;
    color->g = ((c >> 8) & 0xFF) / 255.0f;
    color->b = ((c >> 0) & 0xFF) / 255.0f;

    return Data_Wrap_Struct(rb_cColor, NULL, RUBY_DEFAULT_FREE, color);
}

static VALUE rpg_tmx_map_size(VALUE self) {
    tmx_map *map = DATA_PTR(self);
    RPGsize *size = ALLOC(RPGsize);
    size->width = map->width;
    size->height = map->height;
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_tmx_map_tile_size(VALUE self) {
    tmx_map *map = DATA_PTR(self);
    RPGsize *size = ALLOC(RPGsize);
    size->width = map->tile_width;
    size->height = map->tile_height;
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_tmx_map_orientation(VALUE self) {
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

static VALUE rpg_tmx_map_stagger_index(VALUE self) {
    tmx_map *map = DATA_PTR(self);
    enum tmx_stagger_index index = map->stagger_index;
    switch (index) {
        case SI_EVEN: return STR2SYM("even");
        case SI_ODD: return STR2SYM("odd");
        case SI_NONE:
        default: return STR2SYM("none");
    }
}

static VALUE rpg_tmx_map_stagger_axis(VALUE self) {
    tmx_map *map = DATA_PTR(self);
    enum tmx_stagger_axis axis = map->stagger_axis;
    switch (axis) {
        case SA_X: return STR2SYM("x");
        case SA_Y: return STR2SYM("y");
        case SA_NONE:
        default: return STR2SYM("none");
    }
}

static VALUE rpg_tmx_map_render_order(VALUE self) {
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

static VALUE rpg_tmx_map_tile(VALUE self, VALUE gid) {
    tmx_map *map = DATA_PTR(self);
    unsigned int i = NUM2UINT(gid);
    if (i >= map->tilecount) {
        return Qnil;
    }
    tmx_tile *tile = map->tiles[i];
    return tile ? Data_Wrap_Struct(rb_cTile, NULL, NULL, tile) : Qnil;
}

static VALUE rpg_tmx_map_property(VALUE self, VALUE key) {
    tmx_map *map = DATA_PTR(self);
    VALUE str = rb_sym2str(key);
    char *k = StringValueCStr(str);
    tmx_property *prop = tmx_get_property(map->properties, k);
    return rpg_tilemap_prop_value(prop);
}

static VALUE rpg_tmx_map_properties(VALUE self) {
    tmx_map *map = DATA_PTR(self);
    VALUE hash = rb_hash_new();
    tmx_property_foreach(map->properties, rpg_tilemap_prop_callback, &hash);
    return hash;
}

static VALUE rpg_tmx_map_tilesets(VALUE self) {
    tmx_map *map = DATA_PTR(self);
    VALUE ary = rb_ary_new();
    for (tmx_tileset_list *list = map->ts_head; list != NULL; list = list->next) {
        rb_ary_push(ary, Data_Wrap_Struct(rb_cTileset, NULL, NULL, list->tileset));
    }
    return ary;
}

static VALUE rpg_tmx_map_layers(VALUE self) {
    tmx_map *map = DATA_PTR(self);
    VALUE ary = rb_ary_new();
    for (tmx_layer *layer = map->ly_head; layer != NULL; layer = layer->next) {
        if (!layer->user_data.integer) {
            layer->user_data.integer = (int) map->tilecount;
        }
        rb_ary_push(ary, Data_Wrap_Struct(rb_cLayer, NULL, NULL, layer));
    }
    return ary;
}

static VALUE rpg_tmx_map_each_tileset(VALUE self) {
    RETURN_ENUMERATOR(self, 0, NULL);
    tmx_map *map = DATA_PTR(self);
    for (tmx_tileset_list *list = map->ts_head; list != NULL; list = list->next) {
        rb_yield(Data_Wrap_Struct(rb_cTileset, NULL, NULL, list->tileset));
    }
    return self;
}

static VALUE rpg_tmx_map_each_layer(VALUE self) {
    RETURN_ENUMERATOR(self, 0, NULL);
    tmx_map *map = DATA_PTR(self);
    for (tmx_layer *layer = map->ly_head; layer != NULL; layer = layer->next) {
        if (!layer->user_data.integer) {
            layer->user_data.integer = (int) map->tilecount;
        }
        rb_yield(Data_Wrap_Struct(rb_cLayer, NULL, NULL, layer));
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
    return tile->image ? Data_Wrap_Struct(rb_cImageInfo, NULL, NULL, tile->image) : Qnil;
}

static VALUE rpg_tmx_tile_collision(VALUE self) {
    tmx_tile *tile = DATA_PTR(self);
    rb_p(INT2NUM(tile->collision->obj_type));
    return tile->collision ? Data_Wrap_Struct(rb_cMapObject, NULL, NULL, tile->collision) : Qnil;
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
        VALUE frame = Data_Wrap_Struct(rb_cFrame, NULL, NULL, &tile->animation[i]);
        rb_ary_store(frames, i, frame);
    }
    return frames;
}

static VALUE rpg_tmx_tile_each_frame(VALUE self) {
    RETURN_ENUMERATOR(self, 0, NULL);
    tmx_tile *tile = DATA_PTR(self);
    long count = (long) tile->animation_len;
    for (long i = 0; i < count; i++) {
        rb_yield(Data_Wrap_Struct(rb_cFrame, NULL, NULL, &tile->animation[i]));
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

#pragma region Tileset

ATTR_READER(rpg_tmx_tileset_tile_count, tmx_tileset, tilecount, UINT2NUM)
ATTR_READER(rpg_tmx_tileset_offset_x, tmx_tileset, x_offset, INT2NUM)
ATTR_READER(rpg_tmx_tileset_offset_y, tmx_tileset, y_offset, INT2NUM)
ATTR_READER(rpg_tmx_tileset_tile_width, tmx_tileset, tile_width, UINT2NUM)
ATTR_READER(rpg_tmx_tileset_tile_height, tmx_tileset, tile_height, UINT2NUM)
ATTR_READER(rpg_tmx_tileset_spacing, tmx_tileset, spacing, UINT2NUM)
ATTR_READER(rpg_tmx_tileset_margin, tmx_tileset, margin, UINT2NUM)
ATTR_READER(rpg_tmx_tileset_name, tmx_tileset, name, rb_str_new_cstr)

static VALUE rpg_tmx_tileset_offset(VALUE self) {
    tmx_tileset *ts = DATA_PTR(self);
    RPGpoint *pnt = ALLOC(RPGpoint);
    pnt->x = ts->x_offset;
    pnt->y = ts->y_offset;
    return Data_Wrap_Struct(rb_cPoint, NULL, RUBY_DEFAULT_FREE, pnt);
}

static VALUE rpg_tmx_tileset_tile_size(VALUE self) {
    tmx_tileset *ts = DATA_PTR(self);
    RPGsize *size = ALLOC(RPGsize);
    size->width = (GLint) ts->tile_width;
    size->height = (GLint) ts->tile_height;
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_tmx_tileset_inspect(VALUE self) {
    tmx_tileset *ts = DATA_PTR(self);
    return rb_sprintf("<Tileset: name:%s>", ts->name);
}

static VALUE rpg_tmx_tileset_property(VALUE self, VALUE key) {
    tmx_tileset *set = DATA_PTR(self);
    VALUE str = rb_sym2str(key);
    char *k = StringValueCStr(str);
    tmx_property *prop = tmx_get_property(set->properties, k);
    return rpg_tilemap_prop_value(prop);
}

static VALUE rpg_tmx_tileset_properties(VALUE self) {
    tmx_tileset *set = DATA_PTR(self);
    VALUE hash = rb_hash_new();
    tmx_property_foreach(set->properties, rpg_tilemap_prop_callback, &hash);
    return hash;
}

static VALUE rpg_tmx_tileset_image(VALUE self) {
    tmx_tileset *set = DATA_PTR(self);
    return set->image ? Data_Wrap_Struct(rb_cImageInfo, NULL, NULL, set->image) : Qnil;
}

static VALUE rpg_tmx_tileset_tile(VALUE self, VALUE gid) {
    tmx_tileset *set = DATA_PTR(self);
    unsigned int i = NUM2UINT(gid);
    if (i > set->tilecount) {
        return Qnil;
    }  
    return Data_Wrap_Struct(rb_cTile, NULL, NULL, &set->tiles[i]);
}

#pragma endregion Tileset

#pragma region Layer

ATTR_READER(rpg_tmx_layer_name, tmx_layer, name, rb_str_new_cstr)
ATTR_READER(rpg_tmx_layer_alpha, tmx_layer, opacity, DBL2NUM)
ATTR_READER(rpg_tmx_layer_visible, tmx_layer, visible, RB_BOOL)
ATTR_READER(rpg_tmx_layer_offset_x, tmx_layer, offsetx, INT2NUM)
ATTR_READER(rpg_tmx_layer_offset_y, tmx_layer, offsety, INT2NUM)

static VALUE rpg_tmx_layer_opacity(VALUE self) {
    tmx_layer *layer = DATA_PTR(self);
    return INT2NUM((int) (layer->opacity * 255));
}

static VALUE rpg_tmx_layer_offset(VALUE self) {
    tmx_layer *l = DATA_PTR(self);
    RPGpoint *pnt = ALLOC(RPGpoint);
    pnt->x = l->offsetx;
    pnt->y = l->offsety;
    return Data_Wrap_Struct(rb_cPoint, NULL, RUBY_DEFAULT_FREE, pnt);
}

static VALUE rpg_tmx_layer_type(VALUE self) {
    tmx_layer *l = DATA_PTR(self);
    switch (l->type) {
        case L_LAYER: return STR2SYM("layer");
        case L_OBJGR: return STR2SYM("objects");
        case L_IMAGE: return STR2SYM("image");
        case L_GROUP: return STR2SYM("group");
        case L_NONE:
        default: return STR2SYM("none");
    }
}

static VALUE rpg_tmx_layer_contents(VALUE self) {
    tmx_layer *l = DATA_PTR(self);
    enum tmx_layer_type type = l->type;
    switch (type) {
        case L_LAYER: {
            int count = l->user_data.integer;
            VALUE ary = rb_ary_new_capa(count);
            for (int i = 0; i < count; i++) {
                rb_ary_push(ary, INT2NUM(l->content.gids[i]));
            }
            return ary;
        }   
        case L_OBJGR: {
            tmx_object_group *group = l->content.objgr;
            return Data_Wrap_Struct(rb_cGroup, NULL, NULL, group);
        }
        case L_IMAGE: {
            tmx_image *img = l->content.image;
            return img ? Data_Wrap_Struct(rb_cImageInfo, NULL, NULL, img) : Qnil;
        }
        case L_GROUP: {
            VALUE ary = rb_ary_new();
            for (tmx_layer *layer = l->content.group_head; layer != NULL; layer = layer->next) {
                rb_ary_push(ary, Data_Wrap_Struct(rb_cLayer, NULL, NULL, layer));
            }
            return ary;
        }
        case L_NONE:
        default: return Qnil;
    }
}

static VALUE rpg_tmx_layer_property(VALUE self, VALUE key) {
    tmx_layer *l = DATA_PTR(self);
    VALUE str = rb_sym2str(key);
    char *k = StringValueCStr(str);
    tmx_property *prop = tmx_get_property(l->properties, k);
    return rpg_tilemap_prop_value(prop);
}

static VALUE rpg_tmx_layer_properties(VALUE self) {
    tmx_layer *l = DATA_PTR(self);
    VALUE hash = rb_hash_new();
    tmx_property_foreach(l->properties, rpg_tilemap_prop_callback, &hash);
    return hash;
}

static VALUE rpg_tmx_layer_next(VALUE self) {
    tmx_layer *l = DATA_PTR(self);
    tmx_layer *next = l->next;
    return next ? Data_Wrap_Struct(rb_cLayer, NULL, NULL, next) : Qnil;
}

static VALUE rpg_tmx_layer_inspect(VALUE self) {
    tmx_layer *l = DATA_PTR(self);
    char *type;
    switch (l->type) {
        case L_LAYER: 
            type = "layer";
            break;
        case L_OBJGR: 
            type = "objects";
            break;
        case L_IMAGE: 
            type = "image";
            break;
        case L_GROUP: 
            type = "group";
            break;
        case L_NONE:
        default: 
            type = "none";
            break;
    }
    return rb_sprintf("<Layer: name:%s type:%s>", l->name, type);
}

#pragma endregion Layer

#pragma region Group

static VALUE rpg_tmx_group_color(VALUE self) {
    tmx_object_group *g = DATA_PTR(self);
    RPGcolor *color = ALLOC(RPGcolor);
    GLuint c = (GLuint) g->color;
    color->a = ((c >> 24) & 0xFF) / 255.0f;
    color->r = ((c >> 16) & 0xFF) / 255.0f;
    color->g = ((c >> 8) & 0xFF) / 255.0f;
    color->b = ((c >> 0) & 0xFF) / 255.0f;

    return Data_Wrap_Struct(rb_cColor, NULL, RUBY_DEFAULT_FREE, color);
}

static VALUE rpg_tmx_group_draw_order(VALUE self) {
    tmx_object_group *g = DATA_PTR(self);
    switch (g->draworder) {
        case G_INDEX: return STR2SYM("index");
        case G_TOPDOWN: return STR2SYM("topdown");
        case G_NONE:
        default: return STR2SYM("none");
    }
}

static VALUE rpg_tmx_group_object(VALUE self, VALUE index) {
    int i = NUM2INT(index);
    if (i < 0) {
        return Qnil;
    }
    tmx_object_group *g = DATA_PTR(self);
    int current = 0;
    for (tmx_object *obj = g->head; obj != NULL; obj = obj->next, current++) {
        if (current != i) {
            continue;
        }
        VALUE klass = rpg_tmx_obj_class(obj->obj_type);
        if (klass == Qnil) {
            continue;
        }
        return Data_Wrap_Struct(klass, NULL, NULL, obj);
    }
    return Qnil;
}

static VALUE rpg_tmx_group_objects(VALUE self) {
    tmx_object_group *g = DATA_PTR(self);
    VALUE ary = rb_ary_new();
    for (tmx_object *obj = g->head; obj != NULL; obj = obj->next) {
        VALUE klass = rpg_tmx_obj_class(obj->obj_type);
        if (klass == Qnil) {
            continue;
        }
        rb_ary_push(ary, Data_Wrap_Struct(klass, NULL, NULL, obj));
    }
    return ary;
}

static VALUE rpg_tmx_group_each(VALUE self) {
    RETURN_ENUMERATOR(self, 0, NULL);
    tmx_object_group *g = DATA_PTR(self);
    for (tmx_object *obj = g->head; obj != NULL; obj = obj->next) {
        VALUE klass = rpg_tmx_obj_class(obj->obj_type);
        if (klass == Qnil) {
            continue;
        }
        rb_yield(Data_Wrap_Struct(klass, NULL, NULL, obj));
    }
    return self;
}

#pragma endregion

#pragma region MapObject

ATTR_READER(rpg_tmx_obj_id, tmx_object, id, UINT2NUM)
ATTR_READER(rpg_tmx_obj_x, tmx_object, x, DBL2NUM)
ATTR_READER(rpg_tmx_obj_y, tmx_object, y, DBL2NUM)
ATTR_READER(rpg_tmx_obj_width, tmx_object, width, DBL2NUM)
ATTR_READER(rpg_tmx_obj_height, tmx_object, height, DBL2NUM)
ATTR_READER(rpg_tmx_obj_visible, tmx_object, visible, RB_BOOL)
ATTR_READER(rpg_tmx_obj_rotation, tmx_object, rotation, DBL2NUM)
ATTR_READER(rpg_tmx_obj_type, tmx_object, type, rb_str_new_cstr)

static VALUE rpg_tmx_obj_property(VALUE self, VALUE key) {
    tmx_object *o = DATA_PTR(self);
    VALUE str = rb_sym2str(key);
    char *k = StringValueCStr(str);
    tmx_property *prop = tmx_get_property(o->properties, k);
    return rpg_tilemap_prop_value(prop);
}

static VALUE rpg_tmx_obj_properties(VALUE self) {
    tmx_object *o = DATA_PTR(self);
    VALUE hash = rb_hash_new();
    tmx_property_foreach(o->properties, rpg_tilemap_prop_callback, &hash);
    return hash;
}

static VALUE rpg_tmx_obj_template(VALUE self) {
    tmx_object *o = DATA_PTR(self);
    return o->template_ref ? Data_Wrap_Struct(rb_cTemplate, NULL, NULL, o) : Qnil;
}

static VALUE rpg_tmx_obj_gid(VALUE self) {
    if (CLASS_OF(self) == rb_cMapObject) {
        tmx_object *o = DATA_PTR(self);
        return INT2NUM(o->content.gid);
    }
    return Qnil;
}

static VALUE rpg_tmx_shape_points(VALUE self) {
    tmx_object *o = DATA_PTR(self);
    tmx_shape *shape = o->content.shape;
    VALUE ary = rb_ary_new_capa(shape->points_len);

    for (long i = 0; i < shape->points_len; i++) {
        RPGvec2 *vec = ALLOC(RPGvec2);
        vec->x = (GLfloat) shape->points[i][0];
        vec->y = (GLfloat) shape->points[i][1];
        rb_ary_store(ary, i, Data_Wrap_Struct(rb_cVec2, NULL, RUBY_DEFAULT_FREE, vec));
    }
    return ary;
}

static VALUE rpg_tmx_shape_each_point(VALUE self) {
    tmx_object *o = DATA_PTR(self);
    tmx_shape *shape = o->content.shape;

    for (long i = 0; i < shape->points_len; i++) {
        RPGvec2 *vec = ALLOC(RPGvec2);
        vec->x = (GLfloat) shape->points[i][0];
        vec->y = (GLfloat) shape->points[i][1];
        rb_yield(Data_Wrap_Struct(rb_cVec2, NULL, RUBY_DEFAULT_FREE, vec));
    }
    return self;
}

static VALUE rpg_tmx_shape_count(VALUE self) {
    tmx_object *o = DATA_PTR(self);
    return INT2NUM(o->content.shape->points_len);
}

static VALUE rpg_tmx_shape_kind(VALUE self) {
    tmx_object *o = DATA_PTR(self);
    switch (o->obj_type)
    {
        case OT_ELLIPSE: return STR2SYM("ellipse");
        case OT_SQUARE: return STR2SYM("square");
        case OT_POLYLINE: return STR2SYM("polyline");
        case OT_POINT: return STR2SYM("point");
        case OT_POLYGON: 
        default: return STR2SYM("polygon");
    }
}

ATTR_READER(rpg_tmx_text_wrap, tmx_object, content.text->wrap, RB_BOOL)
ATTR_READER(rpg_tmx_text_bold, tmx_object, content.text->bold, RB_BOOL)
ATTR_READER(rpg_tmx_text_italic, tmx_object, content.text->italic, RB_BOOL)
ATTR_READER(rpg_tmx_text_underline, tmx_object, content.text->underline, RB_BOOL)
ATTR_READER(rpg_tmx_text_strikeout, tmx_object, content.text->strikeout, RB_BOOL)
ATTR_READER(rpg_tmx_text_kerning, tmx_object, content.text->kerning, RB_BOOL)
ATTR_READER(rpg_tmx_text_value, tmx_object, content.text->text, rb_str_new_cstr)
ATTR_READER(rpg_tmx_text_font_family, tmx_object, content.text->fontfamily, rb_str_new_cstr)
ATTR_READER(rpg_tmx_text_size, tmx_object, content.text->pixelsize, INT2NUM)

static VALUE rpg_tmx_text_color(VALUE self) {
    tmx_object *o = DATA_PTR(self);
    RPGcolor *color = ALLOC(RPGcolor);
    GLuint c = o->content.text->color;
    color->a = ((c >> 24) & 0xFF) / 255.0f;
    color->r = ((c >> 16) & 0xFF) / 255.0f;
    color->g = ((c >> 8) & 0xFF) / 255.0f;
    color->b = ((c >> 0) & 0xFF) / 255.0f;
    return Data_Wrap_Struct(rb_cColor, NULL, RUBY_DEFAULT_FREE, color);
}

static VALUE rpg_tmx_text_align_h(VALUE self) { // TODO: Use Align module values?
    tmx_object *o = DATA_PTR(self);
    switch (o->content.text->halign) {
        case HA_LEFT: STR2SYM("left");
        case HA_RIGHT: STR2SYM("right");
        case HA_CENTER: STR2SYM("center");
        default: return STR2SYM("none");
    }
}

static VALUE rpg_tmx_text_align_v(VALUE self) { // TODO: Use Align module values?
    tmx_object *o = DATA_PTR(self);
    switch (o->content.text->valign) {
        case VA_TOP: return STR2SYM("top");
        case VA_BOTTOM: return STR2SYM("bottom");
        case VA_CENTER: return STR2SYM("center");
        default: return STR2SYM("none");
    }
}

#pragma endregion MapObject

#pragma region Template

static VALUE rpg_tmx_template_tileset(VALUE self) {
    tmx_template *t = DATA_PTR(self);
    return t->tileset_ref ? Data_Wrap_Struct(rb_cTileset, NULL, NULL, t->tileset_ref->tileset) : Qnil;
}

static VALUE rpg_tmx_template_object(VALUE self) {
    tmx_template *t = DATA_PTR(self);
    if (t->object == NULL) {
        return Qnil;
    }
    VALUE klass = rpg_tmx_obj_class(t->object->obj_type);
    return Data_Wrap_Struct(klass, NULL, NULL, t->object);
}

#pragma endregion Template

#pragma region ImageInfo

ATTR_READER(rpg_tmx_img_source, tmx_image, source, rb_str_new_cstr)
ATTR_READER(rpg_tmx_img_width, tmx_image, width, UINT2NUM)
ATTR_READER(rpg_tmx_img_height, tmx_image, height, UINT2NUM)

static VALUE rpg_tmx_img_trans_color(VALUE self) {
    tmx_image *img = DATA_PTR(self);
    if (img->uses_trans) {
        RPGcolor *color = ALLOC(RPGcolor);
        GLuint c = img->trans;
        color->a = 1.0f;
        color->r = ((c >> 16) & 0xFF) / 255.0f;
        color->g = ((c >> 8) & 0xFF) / 255.0f;
        color->b = ((c >> 0) & 0xFF) / 255.0f;
        return Data_Wrap_Struct(rb_cColor, NULL, RUBY_DEFAULT_FREE, color);
    }
    return Qnil;
}

static VALUE rpg_tmx_img_image(VALUE self) {
    tmx_image *img = DATA_PTR(self);
    if (img->resource_image) {
        return Data_Wrap_Struct(rb_cImage, NULL, NULL, img->resource_image);
    }
    return Qnil;
}

#pragma endregion ImageInfo

void rpg_tmx_init(VALUE parent) {
    // Functions
    tmx_alloc_func = RPG_REALLOC;
    tmx_free_func = RPG_FREE;
    tmx_img_load_func = rpg_tilemap_image_load;
    tmx_img_free_func = rpg_tilemap_image_free;
    cache = tmx_make_resource_manager();    

    // Definitions
    VALUE map = rb_define_module_under(parent, "Map");
    rb_cTilemap = rb_define_class_under(map, "Tilemap", rb_cObject);
    rb_cTileset = rb_define_class_under(map, "Tileset", rb_cObject);
    rb_cLayer = rb_define_class_under(map, "Layer", rb_cObject);
    rb_cTile = rb_define_class_under(map, "Tile", rb_cObject);
    rb_cFrame = rb_define_class_under(rb_cTile, "Frame", rb_cObject);
    rb_cImageInfo = rb_define_class_under(map, "ImageInfo", rb_cObject);
    rb_cGroup = rb_define_class_under(map, "Group", rb_cObject);
    rb_cMapObject = rb_define_class_under(map, "MapObject", rb_cObject);
    rb_cText = rb_define_class_under(map, "Text", rb_cMapObject);
    rb_cShape = rb_define_class_under(map, "Shape", rb_cMapObject);
    rb_cTemplate = rb_define_class_under(map, "Template", rb_cObject);

    rb_define_singleton_method(map, "clear_cache", rpg_tmx_clear_cache, 0);
    rb_define_singleton_method(map, "cache_tileset", rpg_tmx_cache_tileset, 1);
    rb_define_singleton_method(map, "cache_template", rpg_tmx_cache_template, 1);

    // Allocation
    rb_define_alloc_func(rb_cTilemap, rpg_tmx_map_alloc);
    rb_define_alloc_func(rb_cTileset, rpg_tmx_tileset_alloc);
    rb_define_alloc_func(rb_cLayer, rpg_tmx_layer_alloc);
    rb_define_alloc_func(rb_cTile, rpg_tmx_tile_alloc);
    rb_define_alloc_func(rb_cFrame, rpg_tmx_layer_alloc);
    rb_define_alloc_func(rb_cTile, rpg_tmx_tile_alloc);
    rb_define_alloc_func(rb_cGroup, rpg_tmx_objgroup_alloc);
    rb_define_alloc_func(rb_cMapObject, rpg_tmx_obj_alloc);
    rb_define_alloc_func(rb_cTemplate, rpg_tmx_template_alloc);

    // Tilemap
    rb_define_singleton_method(rb_cTilemap, "from_file", rpg_tmx_map_from_file, 1);
    rb_define_method(rb_cTilemap, "width", rpg_tmx_map_width, 0);
    rb_define_method(rb_cTilemap, "height", rpg_tmx_map_height, 0);
    rb_define_method(rb_cTilemap, "size", rpg_tmx_map_size, 0);
    rb_define_method(rb_cTilemap, "tile_width", rpg_tmx_map_tile_width, 0);
    rb_define_method(rb_cTilemap, "tile_height", rpg_tmx_map_tile_height, 0);
    rb_define_method(rb_cTilemap, "tile_size", rpg_tmx_map_tile_size, 0);
    rb_define_method(rb_cTilemap, "back_color", rpg_tmx_map_back_color, 0);
    rb_define_method(rb_cTilemap, "orientation", rpg_tmx_map_orientation, 0);
    rb_define_method(rb_cTilemap, "stagger_axis", rpg_tmx_map_stagger_axis, 0);
    rb_define_method(rb_cTilemap, "stagger_index", rpg_tmx_map_stagger_index, 0);
    rb_define_method(rb_cTilemap, "render_order", rpg_tmx_map_render_order, 0);
    rb_define_method(rb_cTilemap, "tile_count", rpg_tmx_map_tile_count, 0);
    rb_define_method(rb_cTilemap, "side_length", rpg_tmx_map_side_length, 0);
    rb_define_method(rb_cTilemap, "tilesets", rpg_tmx_map_tilesets, 0);
    rb_define_method(rb_cTilemap, "layers", rpg_tmx_map_layers, 0);
    rb_define_method(rb_cTilemap, "each_tileset", rpg_tmx_map_each_tileset, 0);
    rb_define_method(rb_cTilemap, "each_layer", rpg_tmx_map_each_layer, 0);
    rb_define_method(rb_cTilemap, "property", rpg_tmx_map_property, 1);
    rb_define_method(rb_cTilemap, "properties", rpg_tmx_map_properties, 0);
    rb_define_method(rb_cTilemap, "[]", rpg_tmx_map_tile, 1);
    rb_define_method(rb_cTilemap, "tile", rpg_tmx_map_tile, 1);

    // Tile
    rb_define_method(rb_cTile, "tileset", rpg_tmx_tile_tileset, 0);
    rb_define_method(rb_cTile, "inspect", rpg_tmx_tile_inspect, 0);
    rb_define_method(rb_cTile, "id", rpg_tmx_tile_id, 0);
    rb_define_method(rb_cTile, "x", rpg_tmx_tile_x, 0);
    rb_define_method(rb_cTile, "y", rpg_tmx_tile_y, 0);
    rb_define_method(rb_cTile, "animated?", rpg_tmx_tile_animated_p, 0);
    rb_define_method(rb_cTile, "frame_count", rpg_tmx_tile_frame_count, 0);
    rb_define_method(rb_cTile, "frames", rpg_tmx_tile_frames, 0);
    rb_define_method(rb_cTile, "each_frame", rpg_tmx_tile_each_frame, 0);
    rb_define_method(rb_cTile, "property", rpg_tmx_tile_property, 1);
    rb_define_method(rb_cTile, "properties", rpg_tmx_tile_properties, 0);
    rb_define_method(rb_cTile, "image", rpg_tmx_tile_image, 0);
    rb_define_method(rb_cTile, "type", rpg_tmx_tile_type, 0);
    rb_define_method(rb_cTile, "collision", rpg_tmx_tile_collision, 0);

    // Frame
    rb_define_method(rb_cFrame, "tile_id", rpg_tmx_frame_id, 0);
    rb_define_method(rb_cFrame, "duration", rpg_tmx_frame_duration, 0);
    rb_define_method(rb_cFrame, "inspect", rpg_tmx_frame_inspect, 0);

    // Tileset
    rb_define_method(rb_cTileset, "tile_count", rpg_tmx_tileset_tile_count, 0);
    rb_define_method(rb_cTileset, "offset_x", rpg_tmx_tileset_offset_x, 0);
    rb_define_method(rb_cTileset, "offset_y", rpg_tmx_tileset_offset_y, 0);
    rb_define_method(rb_cTileset, "offset", rpg_tmx_tileset_offset, 0);
    rb_define_method(rb_cTileset, "tile_width", rpg_tmx_tileset_tile_width, 0);
    rb_define_method(rb_cTileset, "tile_height", rpg_tmx_tileset_tile_height, 0);
    rb_define_method(rb_cTileset, "tile_size", rpg_tmx_tileset_tile_size, 0);
    rb_define_method(rb_cTileset, "spacing", rpg_tmx_tileset_spacing, 0);
    rb_define_method(rb_cTileset, "margin", rpg_tmx_tileset_margin, 0);
    rb_define_method(rb_cTileset, "name", rpg_tmx_tileset_name, 0);
    rb_define_method(rb_cTileset, "inspect", rpg_tmx_tileset_inspect, 0);
    rb_define_method(rb_cTileset, "property", rpg_tmx_tileset_property, 1);
    rb_define_method(rb_cTileset, "properties", rpg_tmx_tileset_properties, 0);
    rb_define_method(rb_cTileset, "image", rpg_tmx_tileset_image, 0);
    rb_define_method(rb_cTileset, "[]", rpg_tmx_tileset_tile, 1);
    rb_define_method(rb_cTileset, "tile", rpg_tmx_tileset_tile, 1);

    // Layer
    rb_define_method(rb_cLayer, "inspect", rpg_tmx_layer_inspect, 0);
    rb_define_method(rb_cLayer, "property", rpg_tmx_layer_property, 1);
    rb_define_method(rb_cLayer, "properties", rpg_tmx_layer_properties, 0);
    rb_define_method(rb_cLayer, "name", rpg_tmx_layer_name, 0);
    rb_define_method(rb_cLayer, "alpha", rpg_tmx_layer_alpha, 0);
    rb_define_method(rb_cLayer, "opacity", rpg_tmx_layer_opacity, 0);
    rb_define_method(rb_cLayer, "visible?", rpg_tmx_layer_visible, 0);
    rb_define_method(rb_cLayer, "offset_x", rpg_tmx_layer_offset_x, 0);
    rb_define_method(rb_cLayer, "offset_y", rpg_tmx_layer_offset_y, 0);
    rb_define_method(rb_cLayer, "offset", rpg_tmx_layer_offset, 0);
    rb_define_method(rb_cLayer, "type", rpg_tmx_layer_type, 0);
    rb_define_method(rb_cLayer, "contents", rpg_tmx_layer_contents, 0);
    rb_define_method(rb_cLayer, "next_layer", rpg_tmx_layer_next, 0);

    // Group
    rb_include_module(rb_cGroup, rb_mEnumerable);
    rb_define_method(rb_cGroup, "color", rpg_tmx_group_color, 0);
    rb_define_method(rb_cGroup, "draw_order", rpg_tmx_group_draw_order, 0);
    rb_define_method(rb_cGroup, "objects", rpg_tmx_group_objects, 0);
    rb_define_method(rb_cGroup, "each", rpg_tmx_group_each, 0);
    rb_define_method(rb_cGroup, "[]", rpg_tmx_group_object, 1);
    rb_define_method(rb_cGroup, "object", rpg_tmx_group_object, 1);

    // MapObject
    rb_define_method(rb_cMapObject, "property", rpg_tmx_obj_property, 1);
    rb_define_method(rb_cMapObject, "properties", rpg_tmx_obj_properties, 0);
    rb_define_method(rb_cMapObject, "id", rpg_tmx_obj_id, 0);
    rb_define_method(rb_cMapObject, "x", rpg_tmx_obj_x, 0);
    rb_define_method(rb_cMapObject, "y", rpg_tmx_obj_y, 0);
    rb_define_method(rb_cMapObject, "width", rpg_tmx_obj_width, 0);
    rb_define_method(rb_cMapObject, "height", rpg_tmx_obj_height, 0);
    rb_define_method(rb_cMapObject, "visible?", rpg_tmx_obj_visible, 0);
    rb_define_method(rb_cMapObject, "rotation", rpg_tmx_obj_rotation, 0);
    rb_define_method(rb_cMapObject, "type", rpg_tmx_obj_type, 0);
    rb_define_method(rb_cMapObject, "template", rpg_tmx_obj_template, 0);
    rb_define_method(rb_cMapObject, "gid", rpg_tmx_obj_gid, 0);
   
    // Shape
    rb_define_method(rb_cShape, "kind", rpg_tmx_shape_kind, 0);
    rb_define_method(rb_cShape, "length", rpg_tmx_shape_count, 0);
    rb_define_method(rb_cShape, "each_point", rpg_tmx_shape_each_point, 0);
    rb_define_method(rb_cShape, "points", rpg_tmx_shape_points, 0);
    rb_define_alias(rb_cShape, "size", "length");

    // Text
    rb_define_method(rb_cText, "wrap?", rpg_tmx_text_wrap, 0);
    rb_define_method(rb_cText, "bold?", rpg_tmx_text_bold, 0);
    rb_define_method(rb_cText, "italic?", rpg_tmx_text_italic, 0);
    rb_define_method(rb_cText, "underline?", rpg_tmx_text_underline, 0);
    rb_define_method(rb_cText, "strikeout?", rpg_tmx_text_strikeout, 0);
    rb_define_method(rb_cText, "kerning?", rpg_tmx_text_kerning, 0);
    rb_define_method(rb_cText, "value", rpg_tmx_text_value, 0);
    rb_define_method(rb_cText, "font_family", rpg_tmx_text_font_family, 0);
    rb_define_method(rb_cText, "size", rpg_tmx_text_size, 0);
    rb_define_method(rb_cText, "align_h", rpg_tmx_text_align_h, 0);
    rb_define_method(rb_cText, "align_v", rpg_tmx_text_align_v, 0);
    rb_define_method(rb_cText, "color", rpg_tmx_text_color, 0);

    // Template
    rb_define_method(rb_cTemplate, "tileset", rpg_tmx_template_tileset, 0);
    rb_define_method(rb_cTemplate, "object", rpg_tmx_template_object, 0);

    // ImageInfo
    rb_define_method(rb_cImageInfo, "source", rpg_tmx_img_source, 0);
    rb_define_method(rb_cImageInfo, "width", rpg_tmx_img_width, 0);
    rb_define_method(rb_cImageInfo, "height", rpg_tmx_img_height, 0);
    rb_define_method(rb_cImageInfo, "transparent_color", rpg_tmx_img_trans_color, 0);
    rb_define_method(rb_cImageInfo, "image", rpg_tmx_img_image, 0);
}

