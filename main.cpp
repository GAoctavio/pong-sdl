#define WORKING_DIR_FILE(X) ("/home/octavio/random/pong-sdl/pong-sdl/" X)
#include <iostream>
#include <vector>
extern "C"{
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glu.h>

// GLFW
//#include <GLFW/glfw3.h>

#include <SDL2/SDL.h>
}
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "game.h"

//lmao
#define CENTER_ROTATE(X,SCALEX,SCALEY,POSITIONX,POSITIONY) do{ X.setPos({POSITIONX,POSITIONY});\
                                           X.scale({SCALEX,SCALEY});\
                                           X.setPos({X.position.x/X._scale.x,X.position.y/X._scale.y});\
                                           X.order(SCALE,ROTATE,TRANSLATE);}while(0);



struct GameObject{
    Entity entity;
    glm::vec2 speed;
    glm::vec2 accel;
    GameObject(Sprite* sprite,glm::vec2 spd=glm::vec2(0.0f,0.0f),glm::vec2 acc=glm::vec2(0.0f,0.0f)) :entity(sprite){
        //this->entity=Entity(sprite);
        this->speed=spd;
        this->accel=acc;
    }
    void move(float dt){
        entity.move(speed*dt+0.5f*accel*SQUARE(dt));
    }
};



int screenWidth = (3.0/4.0)*1600, screenHeight = (3.0/4.0)*900;
int const originalScreenWidth = screenWidth;
int const originalScreenHeight= screenHeight;
void  do_ball_movement(GameObject& ball,float dt){
    if((ball.speed.x==0.0f && ball.speed.y == 0.0f)){
        Uint32 random=SDL_GetTicks();
        Uint32 speed=random%1000;
        if(random & 1) ball.speed={40.0f,1.0f};
        else ball.speed={-40.0f,-1.0f};
    }
    ball.move(dt);
}
glm::vec2 reflect_vector(glm::vec2 d,glm::vec2 n)//n must be normalized!
{
    return (d - 2*(glm::dot(d,n))*n);
}

bool firstcall=true;
void drawdigit(unsigned char n,Shader* shader,glm::vec2 pos=glm::vec2(0.0f,0.0f),glm::vec2 scale=glm::vec2(1.0f,1.0f)){
    static GLfloat nummatrix[] = {
        //top Horizontal bar
      -0.5f, 0.5f, 0.0f,
      -0.5f, 0.4f, 0.0f,
       0.5f, 0.5f, 0.0f,
       0.5f, 0.4f, 0.0f,
       //middle horizontal bar
      -0.5f, 0.05f, 0.0f,
      -0.5f, -0.05f, 0.0f,
       0.5f, 0.05f, 0.0f,
       0.5f,-0.05f, 0.0f,
       //bottom horizontal bar
      -0.5f, -0.5f, 0.0f,
      -0.5f, -0.4f, 0.0f,
       0.5f, -0.5f, 0.0f,
       0.5f, -0.4f, 0.0f,
       //upleft vertical bar
       -0.5f, 0.5f, 0.0f,
       -0.4f, 0.5f, 0.0f,
       -0.5f, 0.0f, 0.0f,
       -0.4f, 0.0f, 0.0f,
       //upright vertical bar
        0.5f, 0.5f, 0.0f,
        0.4f, 0.5f, 0.0f,
        0.5f, 0.0f, 0.0f,
        0.4f, 0.0f, 0.0f,
        //bottom left vertical bar
        -0.5f, -0.5f, 0.0f,
        -0.4f, -0.5f, 0.0f,
        -0.5f, -0.0f, 0.0f,
        -0.4f, -0.0f, 0.0f,
        //bottom right vertical bar
        0.5f, -0.5f, 0.0f,
        0.4f, -0.5f, 0.0f,
        0.5f, -0.0f, 0.0f,
        0.4f, -0.0f, 0.0f,

    };
    static GLuint upleft_indices[]{
        12, 13, 14,
        15, 14, 13
    };
    static GLuint upright_indices[]{
        16, 17, 18,
        17, 18, 19
    };
    static GLuint bottomleft_indices[]{
        20, 21, 22,
        23, 21, 22
    };
    static GLuint bottomright_indices[]{
        24, 25, 26,
        27, 26, 25
    };
    static GLuint top_indices[]{
        0, 1, 2,
        3, 2 ,1
    };
    static GLuint middle_indices[]{
       4, 5, 6,
       7, 6, 5
    };
    static GLuint bottom_indices[]{
       8, 9, 10,
       11, 10, 9
    };
    static std::vector<Vertex> vertices(
                                        (Vertex*)nummatrix,
                                        ((Vertex*)nummatrix) + (sizeof(nummatrix)/sizeof(Vertex))
                                        );
    static std::vector<GLuint> indices;
    indices.reserve(6);

    if(firstcall)
    {
        indices.clear();
        for(unsigned int i=0;i<(sizeof(upright_indices)/sizeof(upright_indices[0]));i++)
        {
            indices.push_back(upright_indices[i]);
        }
    }
    static Sprite sprite_upright(vertices,indices);
    static Entity upright(&sprite_upright);

    if(firstcall)
    {
        indices.clear();
        for(unsigned int i=0;i<(sizeof(upleft_indices)/sizeof(upleft_indices[0]));i++)
        {
            indices.push_back(upleft_indices[i]);
        }
    }
    static Sprite sprite_upleft(vertices,indices);
    static Entity upleft(&sprite_upleft);


    if(firstcall)
    {
        indices.clear();
        for(unsigned int i=0;i<(sizeof(bottomleft_indices)/sizeof(bottomleft_indices[0]));i++){
            indices.push_back(bottomleft_indices[i]);
        }
    }
    static Sprite sprite_bottomleft(vertices,indices);
    static Entity bottomleft(&sprite_bottomleft);


    if(firstcall)
    {
        indices.clear();
        for(unsigned int i=0;i<(sizeof(bottomright_indices)/sizeof(bottomright_indices[0]));i++){
            indices.push_back(bottomright_indices[i]);
        }
    }
    static Sprite sprite_bottomright(vertices,indices);
    static Entity bottomright(&sprite_bottomright);


    if(firstcall)
    {
        indices.clear();
        for(unsigned int i=0;i<(sizeof(top_indices)/sizeof(top_indices[0]));i++){
            indices.push_back(top_indices[i]);
        }
    }
    static Sprite sprite_top(vertices,indices);
    static Entity top(&sprite_top);


    if(firstcall)
    {
        indices.clear();
        for(unsigned int i=0;i<(sizeof(middle_indices)/sizeof(middle_indices[0]));i++){
            indices.push_back(middle_indices[i]);
        }
    }
    static Sprite sprite_middle(vertices,indices);
    static Entity middle(&sprite_middle);

    if(firstcall)
    {
        indices.clear();
        for(unsigned int i=0;i<(sizeof(bottom_indices)/sizeof(bottom_indices[0]));i++){
            indices.push_back(bottom_indices[i]);
        }
    }
    static Sprite sprite_bottom(vertices,indices);
    static Entity bottom(&sprite_bottom);


    firstcall=false;
    upright.setPos(pos);
    upleft.setPos(pos);
    bottomleft.setPos(pos);
    bottomright.setPos(pos);
    top.setPos(pos);
    middle.setPos(pos);
    bottom.setPos(pos);

    upright.scale(scale);
    upleft.scale(scale);
    bottomleft.scale(scale);
    bottomright.scale(scale);
    top.scale(scale);
    middle.scale(scale);
    bottom.scale(scale);
    switch(n)
    {//giant switch for every digit
        case 0:
            top.draw(shader);
            upleft.draw(shader);
            upright.draw(shader);
            bottomleft.draw(shader);
            bottomright.draw(shader);
            bottom.draw(shader);
        case 1:
            upright.draw(shader);
            bottomright.draw(shader);
            break;
        case 2:
            top.draw(shader);
            upright.draw(shader);
            middle.draw(shader);
            bottomleft.draw(shader);
            bottom.draw(shader);
            break;
        case 3:
            top.draw(shader);
            upright.draw(shader);
            middle.draw(shader);
            bottomright.draw(shader);
            bottom.draw(shader);
            break;
        case 4:
            upleft.draw(shader);
            upright.draw(shader);
            middle.draw(shader);
            bottomright.draw(shader);
            break;
        case 5:
            top.draw(shader);
            upleft.draw(shader);
            middle.draw(shader);
            bottomright.draw(shader);
            bottom.draw(shader);
            break;
        case 6:
            top.draw(shader);
            upleft.draw(shader);
            middle.draw(shader);
            bottomleft.draw(shader);
            bottomright.draw(shader);
            bottom.draw(shader);
            break;
        case 7:
            top.draw(shader);
            upright.draw(shader);
            middle.draw(shader);
            bottomright.draw(shader);
            break;
        case 9:
            top.draw(shader);
            upleft.draw(shader);
            upright.draw(shader);
            middle.draw(shader);
            bottomright.draw(shader);
            bottom.draw(shader);
            break;
        default://case 8
            upright.draw(shader);
            upleft.draw(shader);
            bottomleft.draw(shader);
            bottomright.draw(shader);
            top.draw(shader);
            middle.draw(shader);
            bottom.draw(shader);
            break;
    }



}
template<typename T>
inline void destructor(T a){
    a.T::~T();
}
int main(int argc, char *argv[])
{
    SDL_Window *mainwindow; /* Our window handle */
    SDL_GLContext maincontext; /* Our opengl context handle */
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        sdldie("SDL could not initialize! SDL Error: %s\n");
    }
    else
    {
        //Use OpenGL 3.3 core
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

        //Create window
        mainwindow = SDL_CreateWindow( "pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |SDL_WINDOW_RESIZABLE );
        if( mainwindow == NULL )
        {
            sdldie("Unable to create window");
        }
        else
        {
            //Create context
            maincontext = SDL_GL_CreateContext( mainwindow );
            if( maincontext == NULL ){
                sdldie("OpenGL context could not be created! SDL Error: %s\n");
            }
            else
            {
                //Initialize GLEW
                glewExperimental = GL_TRUE;
                GLenum glewError = glewInit();
                if( glewError != GLEW_OK )
                {
                    std::cout<<"Error initializing GLEW! %s\n"<<glewGetErrorString( glewError );
                }

                //Use Vsync
                if( SDL_GL_SetSwapInterval( 1 ) < 0 )
                {
                    std::cout<<"Warning: Unable to set VSync! SDL Error: %s\n"<<SDL_GetError();
                }
                SDL_DisplayMode current;
                int should_be_zero = SDL_GetCurrentDisplayMode(0, &current); //@HACK:should check for multiple monitors
                if(should_be_zero != 0)
                  sdldie("Could not get display mode for video display");
                screenWidth=(3.0f/4.0f)*current.w;
                screenHeight=(3.0f/4.0f)*current.h;
                (*(int*)(&originalScreenHeight))=screenHeight;
                (*(int*)(&originalScreenWidth))=screenWidth;

                //glViewport(0, 0, screenWidth, screenHeight);
            }
        }
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, screenWidth, screenHeight);

    /* Clear our buffer with a red background */
    glClearColor ( 1.0, 0.0, 0.0, 1.0 );
    glClear ( GL_COLOR_BUFFER_BIT );
    /* Swap our back buffer to the front */
    SDL_GL_SwapWindow(mainwindow);
    /* Wait 2 seconds */
    SDL_Delay(100);

    /* Same as above, but green */
    glClearColor ( 0.0, 1.0, 0.0, 1.0 );
    glClear ( GL_COLOR_BUFFER_BIT );
    SDL_GL_SwapWindow(mainwindow);
    SDL_Delay(100);

    /* Same as above, but blue */
    glClearColor ( 0.0, 0.0, 1.0, 1.0 );
    glClear ( GL_COLOR_BUFFER_BIT );
    SDL_GL_SwapWindow(mainwindow);
    SDL_Delay(100);


    GLfloat vertices[] = {
         0.5f,  0.5f, 0.0f,  // Top Right
         0.5f, -0.5f, 0.0f,  // Bottom Right
        -0.5f, -0.5f, 0.0f,  // Bottom Left
        -0.5f,  0.5f, 0.0f,   // Top Left
    };
    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3,  // First Triangle
        1, 2, 3   // Second Triangle
    };

    std::vector<Vertex> vertices2;
    std::vector<GLuint> indices2;
    {
        for(unsigned int i=0;i<(sizeof(vertices)/sizeof(vertices[0]));i+=3){
            Vertex aux;
            aux.position={vertices[i+0],vertices[i+1],vertices[i+2]};
            vertices2.push_back(aux);
        }
        for(unsigned int i=0;i<(sizeof(indices)/sizeof(indices[0]));i++){
            indices2.push_back(indices[i]);
        }

    }
    Sprite sprite(vertices2,indices2);

    GLfloat quadVertices[] = {   // Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // Positions   // TexCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    // Setup cube VAO
    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
            //glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glBindVertexArray(0);


    unsigned int points_p1=0,points_p2=0;
    bool first_point_p1=true,first_point_p2=true;

    GameObject player1(&sprite,{0.0f,2.0f});
    GameObject player2(&sprite,{0.0f,2.0f});
    GameObject ball(&sprite,{0.0f,0.0f});
    Entity roof(&sprite);
    Entity floor(&sprite);
    GameObject leftwall(&sprite,{0.0f,0.0f});
    GameObject rightwall(&sprite,{0.0f,0.0f});
    {
        player1.entity.setPos({-0.95f,0.0f});
        player2.entity.setPos({0.95f,0.0f});
        ball.entity.setPos({0.0f,0.0f});

        roof.setPos({-1.0f,1.10f});
        floor.setPos({-1.0f,-1.10f});
        roof.scale({4.0f,1.0f});
        floor.scale({4.0f,1.0f});

        leftwall.entity.setPos({-1.0f,0.0f});
        rightwall.entity.setPos({1.0f,0.0f});
        leftwall.entity.scale({0.025f,2.0f});
        rightwall.entity.scale({0.025f,2.0f});

        player1.entity.scale({0.025f, 0.5f});
        player2.entity.scale({0.025f, 0.5f});
        ball.entity.scale({0.0625f,0.0625f});
        ball.entity.order(SCALE,ROTATE,TRANSLATE);

    }

    std::vector<CollisionChecker> collisions;
    {
        collisions.push_back(CollisionChecker(&player1.entity,&roof));
        collisions.push_back(CollisionChecker(&player1.entity,&floor));
        collisions.push_back(CollisionChecker(&player2.entity,&roof));
        collisions.push_back(CollisionChecker(&player2.entity,&floor));
    }


    CollisionChecker ball_floor(&ball.entity,&floor);
    CollisionChecker ball_roof(&ball.entity,&roof);
    CollisionChecker ball_p1(&ball.entity,&player1.entity);
    CollisionChecker ball_p2(&ball.entity,&player2.entity);
    CollisionChecker ball_leftwall(&ball.entity,&leftwall.entity);
    CollisionChecker ball_rightwall(&ball.entity,&rightwall.entity);

    SDL_StartTextInput();
    bool quit = false;
    bool started=false;
    glm::vec2 p1_speed_gain(0.0f,0.0f);
    glm::vec2 p2_speed_gain(0.0f,0.0f);
    unsigned int i=0;
    Uint32 lastFrame=0;
    Uint32 deltaTime=0;
    float framerate=0.0f;
    float dt;


    framebuffer fb(originalScreenWidth,originalScreenHeight);
    Shader shader(WORKING_DIR_FILE("shader.vert"),WORKING_DIR_FILE("shader.frag"));
    Shader fb_shader(WORKING_DIR_FILE("framebuffer_shader.vert"),WORKING_DIR_FILE("framebuffer_shader.frag"));

    while(!quit)
    {
        SDL_PumpEvents();
        Uint32 currentFrame =            SDL_GetTicks();//miliseconds
        deltaTime           =  currentFrame - lastFrame;
        lastFrame           =              currentFrame;
        dt                  =         deltaTime/1000.0f;
        framerate          +=         1000.0f/deltaTime;

        p1_speed_gain={0.0f,0.0f};
        p2_speed_gain={0.0f,0.0f};
        {
            const Uint8 *keystates =                                     SDL_GetKeyboardState( NULL );
            quit                   = keystates[SDL_GetScancodeFromKey(SDLK_q)] || SDL_QuitRequested();
            started                =         started || keystates[SDL_GetScancodeFromKey(SDLK_SPACE)];

            if(started){
                if(keystates[SDL_GetScancodeFromKey(SDLK_w   )])
                {
                    player1.move( dt );
                    p1_speed_gain={ 0.0f , 1.0f };
                }
                if(keystates[SDL_GetScancodeFromKey(SDLK_s   )])
                {
                    player1.move(-dt );
                    p1_speed_gain={ 0.0f ,-1.0f };
                }
                if(keystates[SDL_GetScancodeFromKey(SDLK_UP  )])
                {
                    player2.move( dt );
                    p2_speed_gain={ 0.0f , 1.0f };
                }
                if(keystates[SDL_GetScancodeFromKey(SDLK_DOWN)])
                {
                    player2.move(-dt );
                    p2_speed_gain={ 0.0f ,-1.0f };
                }
                //@ERROR: if we rotate into a collision, handleCollision wont work
                if(keystates[SDL_GetScancodeFromKey(SDLK_j)])
                {
                    player1.entity.rotate(45.0f );
                }

                do_ball_movement(ball,dt);
            }

        }
        {
            unsigned int size=collisions.size();
            for(unsigned int i=0;i<size;i++)
            {
                if(collisions[i].checkCollision())
                    handleCollision(collisions[i]);
            }
        }
        {
            glm::vec3 pos=ball.entity.position;
            glm::vec3 oldpos=ball.entity.oldPosition;
            if(ball_floor.checkCollision()){
                handleCollision(ball_floor);
                //reflect_ball(ball,);
                ball.speed=glm::normalize(glm::reflect(glm::vec2(pos.x-oldpos.x,pos.y-oldpos.y),glm::vec2(0.0f,1.0f)))*glm::length(ball.speed);
            }
            if(ball_roof.checkCollision()){
                handleCollision(ball_roof);
                //reflect_ball(ball,);
                ball.speed=glm::normalize(glm::reflect(glm::vec2(pos.x-oldpos.x,pos.y-oldpos.y),glm::vec2(0.0f,-1.0f)))*glm::length(ball.speed);
            }
            if(ball_p1.checkCollision()){
                handleCollision(ball_p1);
                //reflect_ball(ball,);
                ball.speed=glm::normalize(glm::reflect(glm::vec2(pos.x-oldpos.x,pos.y-oldpos.y),glm::vec2(1.0f,0.0f)) + p1_speed_gain)
                        *
                        (glm::length(ball.speed)+glm::length(p1_speed_gain));
            }
            if(ball_p2.checkCollision()){
                handleCollision(ball_p2);
                //reflect_ball(ball,);
                ball.speed=glm::normalize(glm::reflect(glm::vec2(pos.x-oldpos.x,pos.y-oldpos.y),glm::vec2(-1.0f,0.0f)) + p2_speed_gain)
                        *
                        (glm::length(ball.speed)+glm::length(p2_speed_gain));
            }
//            if(ABS(ball.speed.y)>50.0f){
//                ball.speed.x+=SIGN(ball.speed.x)*(ABS(ball.speed.y)-ABS(50.0f));
//                ball.speed.y=SIGN(ball.speed.y)*40.0f;
//            }
            if(ball_leftwall.checkCollision()){
                points_p2++;
                ball.entity.setPos({0.0f,0.0f});
                ball.speed={0.0f,0.0f};
            }
            if(ball_rightwall.checkCollision()){
                points_p1++;
                ball.entity.setPos({0.0f,0.0f});
                ball.speed={0.0f,0.0f};
            }
        }
        if(i==100){
            i=0;
            framerate/=100.0f;
            std::cout<<framerate<<std::endl;
            std::cout<<points_p1<<'-'<<points_p2<<std::endl;
            framerate=0.0f;
        }
        i+=1;
        shader.Use();
          fb.bind();
//        glBindFramebuffer(GL_FRAMEBUFFER,fbo);
            glViewport(0,0,originalScreenWidth,originalScreenHeight);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


            float aspectRatio=(float)screenWidth/(float)screenHeight;
            glm::mat4 projection;//= glm::ortho(-1.0f,1.0f,-1.0f,1.0f,0.0f,1.0f);
            GLint projection_uniform=glGetUniformLocation(shader.Program, "projection");
            switch(projection_uniform){
                case GL_INVALID_VALUE:
                    std::cout<<"No existe este valor"<<std::endl;
                    break;
                case GL_INVALID_OPERATION:
                    std::cout<<"program is not a program object"<<std::endl;
                    break;
            }
            glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));

            player1.entity.draw(&shader);
            player2.entity.draw(&shader);
            ball.entity.draw(&shader);
            roof.draw(&shader);
            floor.draw(&shader);
            leftwall.entity.draw(&shader);
            rightwall.entity.draw(&shader);

            unsigned int aux=points_p1;
            glm::vec2 position(0.25f,0.5f);
            //NOTE: when one of the points hits 20 I should put a you win screen
            first_point_p1=points_p1? false:true;
            while((aux/10) || (aux%10) || first_point_p1){
                drawdigit(aux%10,&shader,position,{0.125f,0.125f});
                position.x-=1.5f*0.125f;
                aux=aux/10;
                first_point_p1=false;//endless loop if I dont
            }

            aux=points_p2;
            position={-0.25f,0.5f};
            first_point_p2=points_p2? false:true;
            while((aux/10) || (aux%10) || first_point_p2){
                drawdigit(aux%10,&shader,position,{0.125f,0.125f});
                position.x-=1.5f*0.125f;
                aux=aux/10;
                first_point_p2=false;//endless loop if I dont
            }
           fb.unbind();
//         glBindFramebuffer(GL_FRAMEBUFFER, 0);
        SDL_GetWindowSize(mainwindow,&screenWidth,&screenHeight);
        glViewport(0,0,screenWidth,screenHeight);
        glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        fb_shader.Use();
            glm::mat4 screenscaler;
            screenscaler = glm::perspective(glm::radians(59.0f),aspectRatio,0.1f,1.0f);
            GLint model_uniform=glGetUniformLocation(fb_shader.Program, "model");
            switch(model_uniform)
            {
                case GL_INVALID_VALUE:
                    std::cout<<"No existe este valor"<<std::endl;
                    break;
                case GL_INVALID_OPERATION:
                    std::cout<<"program is not a program object"<<std::endl;
                    break;
            }
            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(screenscaler));

            glBindVertexArray(quadVAO);//should scale the scale to the % of resolution
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, fb.texture);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindTexture(GL_TEXTURE_2D,0);
            glBindVertexArray(0);


        SDL_Delay(1);
        SDL_GL_SwapWindow(mainwindow);
    }

    destructor<framebuffer>(fb);
    destructor<Shader>(shader);
    destructor<Shader>(shader);
    SDL_GL_DeleteContext(maincontext);
    SDL_DestroyWindow(mainwindow);
    SDL_Quit();

    return 0;
}

