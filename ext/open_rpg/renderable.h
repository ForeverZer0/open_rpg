#ifndef OPEN_RPG_RENDERABLE_H
#define OPEN_RPG_RENDERABLE_H 1

#include "./common.h"
#include "./batch.h"

void rpg_renderable_init(VALUE parent);
void rpg_sprite_init(VALUE parent);
void rpg_sprite_render(void *sprite);
void rpg_viewport_init(VALUE parent);
void rpg_viewport_render(void *viewport);
void rpg_plane_render(void *renderable);
void rpg_plane_init(VALUE parent);

#define RPG_RENDER_TEXTURE(TEXTURE, VAO)                                                                                                   \
    glActiveTexture(GL_TEXTURE0);                                                                                                          \
    glBindTexture(GL_TEXTURE_2D, TEXTURE);                                                                                                 \
    glBindVertexArray(VAO);                                                                                                                \
    glDrawArrays(GL_TRIANGLES, 0, 6);                                                                                                      \
    glBindVertexArray(0)

#define RPG_BASE_UNIFORMS(obj)                                                                                                             \
    glUniform4f(_color, obj->base.color.r, obj->base.color.g, obj->base.color.b, obj->base.color.a);                                       \
    glUniform4f(_tone, obj->base.tone.r, obj->base.tone.g, obj->base.tone.b, obj->base.tone.gr);                                           \
    glUniform1f(_alpha, obj->base.alpha);                                                                                                  \
    glUniform1f(_hue, obj->base.hue);                                                                                                      \
    glUniform4f(_flash, obj->base.flash.color.r, obj->base.flash.color.g, obj->base.flash.color.b, obj->base.flash.color.a);               \
    glUniformMatrix4fv(_model, 1, GL_FALSE, (float *)&obj->base.model);                                                                    \
    glBlendEquation(obj->base.blend.equation);                                                                                             \
    glBlendFunc(obj->base.blend.src_factor, obj->base.blend.dst_factor)

#define RPG_RENDER_INIT(r)                                                                                                                 \
    r.scale.x = 1.0f;                                                                                                                      \
    r.scale.y = 1.0f;                                                                                                                      \
    r.blend.equation = GL_FUNC_ADD;                                                                                                        \
    r.blend.src_factor = GL_SRC_ALPHA;                                                                                                     \
    r.blend.dst_factor = GL_ONE_MINUS_SRC_ALPHA;                                                                                           \
    r.visible = GL_TRUE;                                                                                                                   \
    r.alpha = 1.0f;                                                                                                                        \
    r.updated = GL_TRUE

#define MAT4_ORTHO(mat4, left, right, top, bottom, near, far)                                                                              \
    mat4.m11 = 2.0f / (right - left);                                                                                                      \
    mat4.m12 = mat4.m13 = mat4.m14 = 0.0f;                                                                                                 \
    mat4.m22 = 2.0f / (top - bottom);                                                                                                      \
    mat4.m21 = mat4.m23 = mat4.m24 = 0.0f;                                                                                                 \
    mat4.m33 = 1.0f / (near - far);                                                                                                        \
    mat4.m31 = mat4.m32 = mat4.m34 = 0.0f;                                                                                                 \
    mat4.m41 = (left + right) / (GLfloat)(left - right);                                                                                   \
    mat4.m42 = (top + bottom) / (GLfloat)(bottom - top);                                                                                   \
    mat4.m43 = near / (GLfloat)(near - far);                                                                                               \
    mat4.m44 = 1.0f

#define MAT4_SET(_mat, _m11, _m12, _m13, _m14, _m21, _m22, _m23, _m24, _m31, _m32, _m33, _m34, _m41, _m42, _m43, _m44)                     \
    _mat.m11 = _m11;                                                                                                                       \
    _mat.m12 = _m12;                                                                                                                       \
    _mat.m13 = _m13;                                                                                                                       \
    _mat.m14 = _m14;                                                                                                                       \
    _mat.m21 = _m21;                                                                                                                       \
    _mat.m22 = _m22;                                                                                                                       \
    _mat.m23 = _m23;                                                                                                                       \
    _mat.m24 = _m24;                                                                                                                       \
    _mat.m31 = _m31;                                                                                                                       \
    _mat.m32 = _m32;                                                                                                                       \
    _mat.m33 = _m33;                                                                                                                       \
    _mat.m34 = _m34;                                                                                                                       \
    _mat.m41 = _m41;                                                                                                                       \
    _mat.m42 = _m42;                                                                                                                       \
    _mat.m43 = _m43;                                                                                                                       \
    _mat.m44 = _m44

#endif /* OPEN_RPG_RENDERABLE_H */