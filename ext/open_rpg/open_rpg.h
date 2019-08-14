#ifndef OPEN_RPG_H
#define OPEN_RPG_H 1

#include "./common.h"

#include "./app.h"
#include "./font.h"
#include "./graphics.h"
#include "./input.h"
#include "./renderable.h"
#include "./shader.h"
#include "./table.h"
#include "./tilemap.h"

void rpg_color_init(VALUE parent);
void rpg_tone_init(VALUE parent);
void rpg_geometry_init(VALUE parent);
void rpg_numerics_init(VALUE parent);
void rpg_image_init(VALUE parent);

#endif /* OPEN_RPG_H */
