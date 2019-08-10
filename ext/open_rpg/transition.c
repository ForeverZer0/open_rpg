#include "./transition.h"
#include "./graphics.h"
#include "./image.h"

VALUE rb_cTransition;

void rpg_transition_init(VALUE parent) {
    rb_cTransition = rb_define_module_under(parent, "Transition");
    rb_define_singleton_method(rb_cTransition, "execute", rpg_transition_execute, 2);


}

static VALUE rpg_transition_execute(VALUE module, VALUE shader, VALUE frames) {
    rb_need_block();

    int f = NUM2INT(frames);
    if (NIL_P(shader) || f < 1) {
        rb_yield(Qnil);
        return Qnil;
    }

    VALUE from_img, to_img;

    GLuint current;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);

    from_img = rpg_graphics_capture(rb_mGraphics);
    RPGimage *from = DATA_PTR(from_img);

    rb_yield(Qnil);

    to_img = rpg_graphics_capture(rb_mGraphics);
    RPGimage *to = DATA_PTR(to_img);


    rpg_image_save(from_img, rb_str_new_cstr("from.png"), INT2NUM(RPG_FORMAT_PNG));
    rpg_image_save(to_img, rb_str_new_cstr("to.png"), INT2NUM(RPG_FORMAT_PNG));



    RPGshader *s = DATA_PTR(shader);
    GLdouble time = glfwGetTime();
    GLdouble max = time + (f * game_tick);

    glUseProgram(s->program);
    GLint percent = glGetUniformLocation(s->program, "progress");


///////////////////////////////////////////
    GLuint vao;
    GLuint vbo;

    // Create a shared vertex array for drawing a quad texture with two triangles
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    float vertices[VERTICES_COUNT] = 
    {
        -1.0f, 1.0f, 0.0f, 1.0f, 
        1.0f, -1.0f, 1.0f, 0.0f, 
        -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f, 
        1.0f, 1.0f, 1.0f, 1.0f, 
        1.0f, -1.0f, 1.0f, 0.0f
    };
    glBufferData(GL_ARRAY_BUFFER, VERTICES_SIZE, vertices, GL_STATIC_DRAW);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VERTICES_STRIDE, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


//////////////////////////////////////



    glBindVertexArray(vao);  

    glActiveTexture(GL_TEXTURE0);                                                                                                         
    glBindTexture(GL_TEXTURE_2D, from->texture); 

    glActiveTexture(GL_TEXTURE1);                                                                                                         
    glBindTexture(GL_TEXTURE_2D, to->texture);   

    glUniform1f(glGetUniformLocation(s->program, "ratio"), (GLfloat) game_width / game_height);
    glUniform2f(glGetUniformLocation(s->program, "center"), 0.5f, 0.5f);



glUniform1i(glGetUniformLocation(s->program, "from"), 0);
glUniform1i(glGetUniformLocation(s->program, "to"), 1);


    double done = f * game_tick;

 
    while (time < max) {
        glClear(GL_COLOR_BUFFER_BIT);

        
        double x = 1.0 - ((max - time) /done);

        glUniform1f(percent, (GLfloat) x );

        rb_p(DBL2NUM(x));

        glDrawArrays(GL_TRIANGLES, 0, 6); 

        glfwPollEvents();
        glfwSwapBuffers(game_window);
        time = glfwGetTime();
    }
    glBindTexture(GL_TEXTURE_2D, 0); 
    glUseProgram(current);
                                                                                       
    glBindVertexArray(0);
    // TODO: At frames to frame_count

    return Qnil;
}