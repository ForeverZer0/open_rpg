#ifndef OPEN_RPG_RENDERABLE_H
#define OPEN_RPG_RENDERABLE_H 1

#include "./batch.h"
#include "./rpg.h"

void rpg_sprite_render(void *sprite);
void rpg_viewport_render(void *viewport);
void rpg_plane_render(void *renderable);

void rpg_renderable_init(VALUE parent);
void rpg_sprite_init(VALUE parent);
void rpg_viewport_init(VALUE parent);
void rpg_plane_init(VALUE parent);

#define RPG_RENDER_TEXTURE(TEXTURE, VAO)                                                                                                   \
    glActiveTexture(GL_TEXTURE0);                                                                                                          \
    glBindTexture(GL_TEXTURE_2D, TEXTURE);                                                                                                 \
    glBindVertexArray(VAO);                                                                                                                \
    glDrawArrays(GL_TRIANGLES, 0, 6);                                                                                                      \
    glBindVertexArray(0)

#define RPG_BASE_UNIFORMS(obj)                                                                                                             \
    glUseProgram(rpgPROGRAM);                                                                                                                \
    glUniform4f(rpgUNIFORM_COLOR, obj->base.color.r, obj->base.color.g, obj->base.color.b, obj->base.color.a);                                       \
    glUniform4f(rpgUNIFORM_TONE, obj->base.tone.r, obj->base.tone.g, obj->base.tone.b, obj->base.tone.gr);                                           \
    glUniform1f(rpgUNIFORM_ALPHA, obj->base.alpha);                                                                                                  \
    glUniform1f(rpgUNIFORM_HUE, obj->base.hue);                                                                                                      \
    glUniform4f(rpgUNIFORM_FLASH, obj->base.flash.color.r, obj->base.flash.color.g, obj->base.flash.color.b, obj->base.flash.color.a);               \
    glUniformMatrix4fv(rpgUNIFORM_MODEL, 1, GL_FALSE, (float *)&obj->base.model);                                                                    \
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

#endif /* OPEN_RPG_RENDERABLE_H */