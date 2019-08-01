#include "./common.h"

void rpg_shader_free(void *data) {
    RPGshader *shader = data;
    if (shader->geometry) {
        glDeleteShader(shader->geometry);
    }
    if (shader->fragment) {
        glDeleteShader(shader->fragment);
    }
    if (shader->vertex) {
        glDeleteShader(shader->vertex);
    }
    if (shader->program) {
        glDeleteProgram(shader->program);
    }
    xfree(shader);
}

void rpg_image_free(void *data) {
    rb_p(rb_str_new2("free"));
    if (data) {
        RPGimage *img = data;
        if (img->texture) {
            glDeleteTextures(1, &img->texture);
        }
        if (img->fbo) {
            glDeleteFramebuffers(1, &img->fbo);
        }
    }
    xfree(data);
}

void rpg_sprite_free(void *data) {
    RPGsprite *sprite = data;
    if (sprite->vao != quad_vao) {
        glDeleteVertexArrays(1, &sprite->vao);
    }
    if (sprite->vbo != quad_vbo) {
        glDeleteBuffers(1, &sprite->vbo);
    }
    xfree(data);
}

void rpg_viewport_free(void *data) {
    RPGviewport *v = data;
    if (v->batch) {
        if (v->batch->items) {
            xfree(v->batch->items);
        }
        xfree(v->batch);
    }
    if (v->fbo) {
        glDeleteFramebuffers(1, &v->fbo);
    }
    if (v->texture) {
        glDeleteTextures(1, &v->texture);
    }
    xfree(data);
}

void rpg_sprite_mark(void *data) {
    RPGsprite *sprite = data;
    if (sprite->image_value != Qnil) {
        rb_gc_mark(sprite->image_value);
    }
    if (sprite->viewport_value != Qnil) {
        rb_gc_mark(sprite->viewport_value);
    }
}

char *rpg_read_file(const char *fname, size_t *length) {
    char *buffer = NULL;
    FILE *file = fopen(fname, "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        long len = ftell(file);
        fseek(file, 0, SEEK_SET);
        buffer = xmalloc(len);
        if (buffer) {
            *length = fread(buffer, 1, len, file);
        }
        fclose(file);
    }
    return buffer;
}