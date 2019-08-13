#ifndef OPEN_RPG_H
#define OPEN_RPG_H 1

#include "./common.h"

#include "./app.h"
#include "./font.h"
#include "./graphics.h"
#include "./image.h"
#include "./input.h"
#include "./math.h"
#include "./renderable.h"
#include "./shader.h"
#include "./table.h"
#include "./tilemap.h"

static VALUE rpg_empty_method(VALUE obj);

void rpg_color_init(VALUE parent);
void rpg_tone_init(VALUE parent);
void rpg_geometry_init(VALUE parent);

#endif /* OPEN_RPG_H */
