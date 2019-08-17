#include "./renderable.h"

#define VERTICES_IN_PARTICLE 4

#define RANDF ((GLfloat)(rand() / RAND_MAX))

static inline GLfloat rpg_deviate(RPGvec2 *range) {
    if (isfinite(range->x) && isfinite(range->y)) {
        GLfloat deviation = (range->y - range->x) * 0.5f;
        return range->x + deviation + (RANDF * deviation) - (RANDF * deviation);
    }
    return range->y;
}

VALUE rb_cEmitter;

typedef struct _RPGparticle {
    GLfloat x, y;
    RPGvec2 center;
    struct {
        GLfloat x;
        GLfloat y;
        GLfloat angular;
    } velocity;
    RPGcolor color;
    GLfloat fade;
    GLfloat scale;
    GLfloat zoom;
    GLfloat friction;
    GLfloat angle;
    GLfloat lifetime;
} RPGparticle;

typedef struct _RPGemitter {
    RPGrenderable base;

    RPGimage *image;
    RPGrect src_rect;
    GLfloat gravity;
    RPGvec2 angular_velocity;
    RPGvec2 center_x, center_y;
    RPGvec2 fade;
    RPGvec2 friction;
    RPGvec2 speed;
    RPGvec2 lifespan;
    RPGvec2 zoom;

    RPGvec2 interval;
    GLfloat time_until_emit;

    GLuint count;
    GLuint max_particles;
    GLuint next_particle_index;
    RPGparticle *particles;

    RPGcolor *colors;
    GLuint colors_offset;

    RPGvec2 *tex_coords;
    GLuint tex_coords_offset;

    RPGvec2 *vertices;
    GLuint vertices_offset;

    GLuint vbo;

} RPGemitter;

static void rpg_emitter_emit(void) {}

static void rpg_emitter_render(void *renderable) { RPGemitter *e = renderable; }

static VALUE rpg_emitter_alloc(VALUE klass) {
    RPGemitter *e = ALLOC(RPGemitter);
    memset(e, 0, sizeof(RPGemitter));
    RPG_RENDER_INIT(e->base);
    e->base.render = rpg_emitter_render;
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, e);
}

static VALUE rpg_emitter_dispose(int argc, VALUE *argv, VALUE self) {
    rb_call_super(0, NULL);

    RPGemitter *e = DATA_PTR(self);
    RPG_FREE(e->particles);
    RPG_FREE(e->colors);
    RPG_FREE(e->vertices);
    RPG_FREE(e->tex_coords);

    VALUE dispose_image;
    rb_scan_args(argc, argv, "01", &dispose_image);
    if (RTEST(dispose_image)) {
        rpg_image_free(e->image);
        e->image = NULL;
    }
    return Qnil;
}

static VALUE rpg_emitter_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE max, opts;
    rb_scan_args(argc, argv, "1:", &max, &opts);

    RPGemitter *e = DATA_PTR(self);
    // Initialize particle storage
    e->max_particles = NUM2UINT(max);
    e->particles = ALLOC_N(RPGparticle, e->max_particles);
    memset(e->particles, 0, e->max_particles * sizeof(RPGparticle));
    int n = VERTICES_IN_PARTICLE * e->max_particles;

    e->colors = ALLOC_N(RPGcolor, n);
    e->colors_offset = 0;                                           // TODO: Just take address from struct?
    e->tex_coords = ALLOC_N(RPGvec2, n);
    e->tex_coords_offset = sizeof(RPGcolor) * n;                    // TODO: Just take address from struct?
    e->vertices = ALLOC_N(RPGvec2, n);
    e->vertices_offset = (sizeof(RPGcolor) + sizeof(RPGvec2)) * n;  // TODO: Just take address from struct?

    // Generate VBO
    int data_size = (sizeof(RPGcolor) + sizeof(RPGvec2) + sizeof(RPGvec2)) * n;
    glGenBuffers(1, &e->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, e->vbo);
    glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_STREAM_DRAW);

    // Check created buffers
    int buffer_size = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);
    if (buffer_size != data_size) {
        rb_raise(rb_eRPGError, "GPU failed to allocate VBO storage (%d bytes)", data_size);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return Qnil;
}

void rpg_emitter_init(VALUE parent) {
    rb_cEmitter = rb_define_class_under(parent, "Emitter", rb_cRenderable);
    rb_define_alloc_func(rb_cEmitter, rpg_emitter_alloc);
    rb_define_method(rb_cEmitter, "dispose", rpg_emitter_dispose, -1);
    rb_define_method(rb_cEmitter, "initialize", rpg_emitter_initialize, -1);
}