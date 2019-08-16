#include "./renderable.h"
#include "./internal.h"

VALUE rb_cSpriteSheet;

static VALUE rpg_sheet_alloc(VALUE klass) {
    RPGspritesheet *s = ALLOC(RPGspritesheet);
    memset(s, 0, sizeof(RPGspritesheet));
    RPG_RENDER_INIT(s->sprite.base);
    s->sprite.base.render = rpg_sprite_render;
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, s);
}

static VALUE rpg_sheet_set_src_rect(VALUE self, VALUE value) {
    rb_warn("Setting SpriteSheet source rectangle has no effect.");
    return value;
}

static VALUE rpg_sheet_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE viewport, image, a1, a2;
    rb_scan_args(argc, argv, "31", &viewport, &image, &a1, &a2);

    // Viewport/Image
    rb_call_super(1, &viewport);
    if (NIL_P(image)) {
        rb_raise(rb_eArgError, "image cannot be nil");
    }
    RPGspritesheet *s = DATA_PTR(self);
    s->sprite.image = DATA_PTR(image);

    // Cell Size
    if (argc == 3) {
        RPGsize *sz = DATA_PTR(a1);
        memcpy(&s->cellsize, sz, sizeof(RPGsize));
    } else {
        s->cellsize.width = NUM2INT(a1);
        s->cellsize.height = NUM2INT(a2);
    }
    s->sprite.src_rect.width = s->cellsize.width;
    s->sprite.src_rect.height = s->cellsize.height;
    check_dimensions(s->cellsize.width, s->cellsize.height);

    s->cellcount.x = s->sprite.image->width / s->cellsize.width;
    s->cellcount.y = s->sprite.image->height / s->cellsize.height;
    GLfloat tw = (GLfloat) s->cellsize.width / s->sprite.image->width;
    GLfloat th = (GLfloat) s->cellsize.height / s->sprite.image->height;

    // Generate VBOs and VAOs
    size_t n = s->cellcount.x * s->cellcount.y;
    s->vaos = RPG_MALLOC(n * sizeof(GLuint));
    s->vbos = RPG_MALLOC(n * sizeof(GLuint));
    glGenBuffers(n, s->vbos);
    glGenVertexArrays(n, s->vaos);

    // Pre-calculate a VAO for every cell using GL_STATIC_DRAW instead of GL_DYNAMIC_DRAW
    GLfloat l, t, r, b;
    GLuint vao, vbo;
    for (int x = 0; x < s->cellcount.x; x++) {
        for (int y = 0; y < s->cellcount.y; y++) {

            int i = x + (y * s->cellcount.x);
            // Texture coordinates
            l = x * tw;
            t = y * th;
            r = l + tw;
            b = t + th;
            vao = s->vaos[i];
            vbo = s->vbos[i];

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            GLfloat vertices[VERTICES_COUNT] = {0.0f, 1.0f, l, b, 1.0f, 0.0f, r, t, 0.0f, 0.0f, l, t,
                                      0.0f, 1.0f, l, b, 1.0f, 1.0f, r, b, 1.0f, 0.0f, r, t};
            glBufferData(GL_ARRAY_BUFFER, VERTICES_SIZE, vertices, GL_STATIC_DRAW);
            glBindVertexArray(vao);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VERTICES_STRIDE, NULL);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0); 
        }
    }
    // Set initial VAO/VBO
    s->sprite.base.updated = GL_TRUE;
    s->sprite.vao = s->vaos[0];
    s->sprite.vbo = s->vbos[0];
    return Qnil;
}

ATTR_READER(rpg_sheet_cell_width, RPGspritesheet, cellsize.width, INT2NUM)
ATTR_READER(rpg_sheet_cell_height, RPGspritesheet, cellsize.height, INT2NUM)
ATTR_READER(rpg_sheet_columns, RPGspritesheet, cellcount.x, INT2NUM)
ATTR_READER(rpg_sheet_rows, RPGspritesheet, cellcount.y, INT2NUM)

static VALUE rpg_sheet_cell_size(VALUE self) {
    RPGspritesheet *s = DATA_PTR(self);
    RPGsize *sz = ALLOC(RPGsize);
    memcpy(sz, &s->cellsize, sizeof(RPGsize));
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, sz);
}

static VALUE rpg_sheet_cell_count(VALUE self) {
    RPGspritesheet *s = DATA_PTR(self);
    return INT2NUM(s->cellcount.x * s->cellcount.y);
}

static VALUE rpg_sheet_dispose(int argc, VALUE *argv, VALUE self) {
    RPGspritesheet *s = DATA_PTR(self);
    size_t n = s->cellcount.x * s->cellcount.y;
    glDeleteBuffers(n, s->vbos);
    glDeleteVertexArrays(n, s->vaos);
    s->sprite.vao = 0;
    s->sprite.vbo = 0;
    rb_call_super(argc, argv);
    return Qnil;
}

static VALUE rpg_sheet_select(VALUE self, VALUE cx, VALUE cy) {
    RPGspritesheet *s = DATA_PTR(self);
    int x = NUM2INT(cx);
    int y = NUM2INT(cy);

    if (x < 0 || x >= s->cellcount.x || y < 0 || y >= s->cellcount.y) {
        s->sprite.vao = 0;
        s->sprite.vbo = 0;
    } else {
        int i = x + (y * s->cellcount.x);
        s->sprite.vao = s->vaos[i];
        s->sprite.vbo = s->vbos[i];
    }
 
    s->sprite.src_rect.x = x * s->cellsize.width;
    s->sprite.src_rect.y = y * s->cellsize.height;
    s->current.x = x;
    s->current.y = y;

    return self;
}

static VALUE rpg_sheet_get_index(VALUE self) {
    RPGspritesheet *s = DATA_PTR(self);
    return INT2NUM(s->current.x + (s->current.y * s->cellcount.x));
}

static VALUE rpg_sheet_set_index(VALUE self, VALUE value) {
    RPGspritesheet *s = DATA_PTR(self);
    int i = NUM2INT(i);
    rpg_sheet_select(self, INT2NUM(i % s->cellcount.x), INT2NUM(i / s->cellcount.x));
    return value;
}

static VALUE rpg_sheet_selected(VALUE self) {
    RPGspritesheet *s = DATA_PTR(self);
    RPGpoint *pnt = ALLOC(RPGpoint);
    memcpy(pnt, &s->current, sizeof(RPGpoint));
    return Data_Wrap_Struct(rb_cPoint, NULL, RUBY_DEFAULT_FREE, pnt);
}

static VALUE rpg_sheet_set_image(VALUE self, VALUE image) {
    RPGspritesheet *s = DATA_PTR(self);
    if (NIL_P(image)) {
        s->sprite.image = NULL;
    } else {
        int w = s->cellcount.x * s->cellsize.width;
        int h = s->cellcount.y * s->cellsize.height;
        RPGimage *img = DATA_PTR(image);
        if (w != img->width || h != img->height) {
            rb_warn("image dimensions do not match original");
        }
    }
    return image;
}

void rpg_sprite_sheet_init(VALUE parent) {
    rb_cSpriteSheet = rb_define_class_under(parent, "SpriteSheet", rb_cSprite);
    rb_define_alloc_func(rb_cSpriteSheet, rpg_sheet_alloc);
    rb_define_method(rb_cSpriteSheet, "initialize", rpg_sheet_initialize, -1);
    rb_define_method(rb_cSpriteSheet, "dispose", rpg_sheet_dispose, -1);
    rb_define_method(rb_cSpriteSheet, "src_rect=", rpg_sheet_set_src_rect, 1);
    rb_define_method(rb_cSpriteSheet, "image=", rpg_sheet_set_image, 1);
    rb_define_method(rb_cSpriteSheet, "cell_width", rpg_sheet_cell_width, 0);
    rb_define_method(rb_cSpriteSheet, "cell_height", rpg_sheet_cell_height, 0);
    rb_define_method(rb_cSpriteSheet, "cell_size", rpg_sheet_cell_size, 0);
    rb_define_method(rb_cSpriteSheet, "cell_count", rpg_sheet_cell_count, 0);
    rb_define_method(rb_cSpriteSheet, "columns", rpg_sheet_columns, 0);
    rb_define_method(rb_cSpriteSheet, "rows", rpg_sheet_rows, 0);
    rb_define_method(rb_cSpriteSheet, "selected", rpg_sheet_selected, 0);
    rb_define_method(rb_cSpriteSheet, "select", rpg_sheet_select, 2);
    rb_define_method(rb_cSpriteSheet, "index", rpg_sheet_get_index, 0);
    rb_define_method(rb_cSpriteSheet, "index=", rpg_sheet_set_index, 1);
}

