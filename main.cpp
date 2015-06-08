

#include<string>
std::string WORKING_DIR;
#define WORKING_DIR_FILE(X) ((WORKING_DIR+X).c_str())
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
#include <SDL2/SDL_mixer.h>

#include <unistd.h>//getcwd
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
        if(random & 1) ball.speed={40.0f,-20.0f};
        else ball.speed={-40.0f,-20.0f};
    }
    ball.move(dt);
}
glm::vec2 reflect_vector(glm::vec2 d,glm::vec2 n)//n must be normalized!
{
    return (d - 2*(glm::dot(d,n))*n);
}
/*
         TOP
TOPLEFT TOPMIDDLE TOPRIGHT
        MIDDLE
BOTTOMLEFT BOTTOMMIDDLE BOTTOMRIGHT
        BOTTOM
*/
enum digit_flags{
    DIGIT_TOP         = 1,
    DIGIT_MIDDLE      = 2,
    DIGIT_BOTTOM      = 4,
    DIGIT_TOPLEFT     = 8,
    DIGIT_TOPRIGHT    = 16,
    DIGIT_BOTTOMLEFT  = 32,
    DIGIT_BOTTOMRIGHT = 64,
    DIGIT_TOPLEFT_MIDDLERIGHT = 128,
    DIGIT_TOPRIGHT_MIDDLELEFT = 256,
    DIGIT_BOTTOMLEFT_MIDDLERIGHT = 512,
    DIGIT_BOTTOMRIGHT_MIDDLELEFT = 1024,
    DIGIT_TOPLEFT_BOTTOMRIGHT=2048,
    DIGIT_TOPRIGHT_BOTTOMLEFT=4096,
    DIGIT_TOPLEFT_MIDDLE=8192,
    DIGIT_TOPRIGHT_MIDDLE=16384,
    DIGIT_BOTTOMLEFT_MIDDLE=32768,
    DIGIT_BOTTOMRIGHT_MIDDLE=65536,
    DIGIT_TOPMIDDLE=131072,
    DIGIT_BOTTOMMIDDLE=262144
};

bool drawdigit_firstcall=true;
void drawdigit(unsigned char n,Shader* shader,glm::vec2 pos=glm::vec2(0.0f,0.0f),glm::vec2 scale=glm::vec2(1.0f,1.0f),
               int flags=DIGIT_TOPLEFT_BOTTOMRIGHT|DIGIT_TOP|DIGIT_TOPLEFT|DIGIT_TOPRIGHT|DIGIT_MIDDLE
                        |DIGIT_BOTTOMLEFT|DIGIT_BOTTOMRIGHT|DIGIT_BOTTOM){
    static GLfloat nummatrix[] = {
        -0.5f,0.5f,0.0f,//0
        0.5f,0.5f,0.0f,//1
        -0.5f,0.4f,0.0f,//2
        0.5f,0.4f,0.0f,//3
        -0.5f,0.05f,0.0f,//4
        0.5f,0.05f,0.0f,//5
        -0.5f,-0.05f,0.0f,//6
        0.5f,-0.05f,0.0f,//7
        -0.5f,-0.4f,0.0f,//8
        0.5f,-0.4f,0.0f,//9
        -0.5f,-0.5f,0.0f,//10
        0.5f,-0.5f,0.0f,//11
        -0.4f,0.5f,0.0f,//12
        0.4f,0.5f,0.0f,//13
        -0.5f,0.0f,0.0f,//14
        -0.4f,0.0f,0.0f,//15
        0.4f,0.0f,0.0f,//16
        0.5f,0.0f,0.0f,//17
        -0.4f,-0.5f,0.0f,//18
        0.4f,-0.5f,0.0f,//19
        -0.4f,0.05f,0.0f,//20
        0.4f,0.05f,0.0f,//21
        -0.4f,-0.05f,0.0f,//22
        0.4f,-0.05f,0.0f,//23
        -0.05f,0.05f,0.0f,//24
        0.05f,0.05f,0.0f,//25
        -0.05f,-0.05f,0.0f,//26
        0.05f,-0.05f,0.0f,//27
        -0.05f,0.5f,0.0f,//28
        0.05f,0.5f,0.0f,//29
        -0.05f,-0.5f,0.0f,//30
        0.05f,-0.5f,0.0f,//31
        -0.05f,0.0f,0.0f,//32
        0.05f,0.0f,0.0f,//33

    };
    static GLuint topleft_indices[]{
        0,12,14,
        14,12,15
    };
    static GLuint topright_indices[]{
        13,1,16,
        16,1,17
    };
    static GLuint bottomleft_indices[]{
        14,15,10,
        10,15,18
    };
    static GLuint bottomright_indices[]{
        16,17,19,
        19,17,11
    };
    static GLuint top_indices[]{
        0,1,2,
        2,1,3
    };
    static GLuint middle_indices[]{
        4,5,6,
        6,5,7
    };
    static GLuint bottom_indices[]{
        8,9,10,
        10,9,11
    };
    static GLuint topleft_to_middleright_indices[]{
        0,12,2,
        2,12,5,
        2,5,23,
        23,5,7
    };
    static GLuint topright_to_middleleft_indices[]{
        13,1,3,
        13,3,4,
        3,4,22,
        4,6,22
    };
    static GLuint bottomleft_to_middleright_indices[]{
        21,5,7,
        21,7,8,
        18,8,7,
        8,10,18
    };
    static GLuint bottomright_to_middleleft_indices[]{
        4,20,6,
        20,6,9,
        6,9,19,
        9,19,11
    };
    static GLuint topleft_to_bottomright_indices[]{
        0,12,2,
        2,12,9,
        2,9,19,
        9,19,11
    };
    static GLuint topright_to_bottomleft_indices[]{
        13,1,3,
        13,3,8,
        3,8,18,
        8,10,18
    };
    static GLuint topleft_to_middle_indices[]{
        0,12,2,
        12,2,25,
        2,25,26,
        25,26,27
    };
    static GLuint topright_to_middle_indices[]{
        13,1,3,
        13,3,24,
        3,24,27,
        24,26,27
    };
    static GLuint bottomleft_to_middle_indices[]{
        24,25,27,
        24,27,8,
        27,8,18,
        8,10,18
    };
    static GLuint bottomright_to_middle_indices[]{
        24,25,26,
        25,26,9,
        26,9,19,
        9,19,11
    };
    static GLuint topmiddle_indices[]{
        28,29,33,
        28,32,33
    };
    static GLuint bottommiddle_indices[]{
        32,33,31,
        32,30,31
    };

    static std::vector<Vertex> vertices(
                                        (Vertex*)nummatrix,
                                        ((Vertex*)nummatrix) + (sizeof(nummatrix)/sizeof(Vertex))
                                        );
    static std::vector<GLuint> indices;
    indices.reserve(6);

#define createdigit(NAME) \
    if(drawdigit_firstcall)\
    {\
        indices.clear();\
        for(unsigned int i=0;i<(sizeof(NAME##_indices)/sizeof(NAME##_indices[0]));i++)\
            indices.push_back(NAME##_indices[i]);\
    }\
    static Sprite sprite_##NAME(vertices,indices);\
    static Entity NAME(&sprite_##NAME);

    createdigit(topright);
    createdigit(topleft);
    createdigit(top);
    createdigit(middle);
    createdigit(bottomleft);
    createdigit(bottomright);
    createdigit(bottom);
    createdigit(topright_to_middleleft);
    createdigit(topleft_to_middleright);
    createdigit(bottomleft_to_middleright);
    createdigit(bottomright_to_middleleft);
    createdigit(topleft_to_bottomright);
    createdigit(topright_to_bottomleft);

    createdigit(topleft_to_middle);
    createdigit(topright_to_middle);
    createdigit(bottomleft_to_middle);
    createdigit(bottomright_to_middle);
    createdigit(topmiddle);
    createdigit(bottommiddle);
#undef createdigit
#define set_and_draw(NAME)\
    NAME.setPos(pos);\
    NAME.scale(scale);\
    NAME.draw(shader);

    drawdigit_firstcall=false;

    switch(n)
    {//giant switch for every digit
        case 0:
            set_and_draw(top);
            set_and_draw(topleft);
            set_and_draw(topright);
            set_and_draw(bottomleft);
            set_and_draw(bottomright);
            set_and_draw(bottom);
        case 1:
            set_and_draw(topright);
            set_and_draw(bottomright);
            break;
        case 2:
            set_and_draw(top);
            set_and_draw(topright);
            set_and_draw(middle);
            set_and_draw(bottomleft);
            set_and_draw(bottom);
            break;
        case 3:
            set_and_draw(top);
            set_and_draw(topright);
            set_and_draw(middle);
            set_and_draw(bottomright);
            set_and_draw(bottom);
            break;
        case 4:
            set_and_draw(topleft);
            set_and_draw(topright);
            set_and_draw(middle);
            set_and_draw(bottomright);
            break;
        case 5:
            set_and_draw(top);
            set_and_draw(topleft);
            set_and_draw(middle);
            set_and_draw(bottomright);
            set_and_draw(bottom);
            break;
        case 6:
            set_and_draw(top);
            set_and_draw(topleft);
            set_and_draw(middle);
            set_and_draw(bottomleft);
            set_and_draw(bottomright);
            set_and_draw(bottom);
            break;
        case 7:
            set_and_draw(top);
            set_and_draw(topright);
            set_and_draw(middle);
            set_and_draw(bottomright);
            break;

        case 8:
            set_and_draw(topright);
            set_and_draw(topleft);
            set_and_draw(bottomleft);
            set_and_draw(bottomright);
            set_and_draw(top);
            set_and_draw(middle);
            set_and_draw(bottom);
            break;
        case 9:
            set_and_draw(top);
            set_and_draw(topleft);
            set_and_draw(topright);
            set_and_draw(middle);
            set_and_draw(bottomright);
            set_and_draw(bottom);
            break;
        default:
            if(flags & DIGIT_TOP){
                set_and_draw(top);}
            if(flags & DIGIT_MIDDLE){
                set_and_draw(middle);}
            if(flags & DIGIT_BOTTOM){
                set_and_draw(bottom);}
            if(flags & DIGIT_TOPLEFT){
                set_and_draw(topleft);}
            if(flags & DIGIT_TOPRIGHT){
                set_and_draw(topright);}
            if(flags & DIGIT_BOTTOMLEFT){
                set_and_draw(bottomleft);}
            if(flags & DIGIT_BOTTOMRIGHT){
                set_and_draw(bottomright);}
            if(flags & DIGIT_TOPLEFT_MIDDLERIGHT){
                set_and_draw(topleft_to_middleright);}
            if(flags & DIGIT_TOPRIGHT_MIDDLELEFT){
                set_and_draw(topright_to_middleleft);}
            if(flags & DIGIT_BOTTOMLEFT_MIDDLERIGHT){
                set_and_draw(bottomleft_to_middleright);}
            if(flags & DIGIT_BOTTOMRIGHT_MIDDLELEFT){
                set_and_draw(bottomright_to_middleleft);}
            if(flags & DIGIT_TOPLEFT_BOTTOMRIGHT){
                set_and_draw(topleft_to_bottomright);}
            if(flags & DIGIT_TOPRIGHT_BOTTOMLEFT){
                set_and_draw(topright_to_bottomleft);}
            if(flags & DIGIT_TOPLEFT_MIDDLE){
                set_and_draw(topleft_to_middle);}
            if(flags & DIGIT_TOPRIGHT_MIDDLE){
                set_and_draw(topright_to_middle);}
            if(flags & DIGIT_BOTTOMLEFT_MIDDLE){
                set_and_draw(bottomleft_to_middle);}
            if(flags & DIGIT_BOTTOMRIGHT_MIDDLE){
                set_and_draw(bottomright_to_middle);}
            if(flags & DIGIT_TOPMIDDLE){
                set_and_draw(topmiddle);
            }
            if(flags & DIGIT_BOTTOMMIDDLE){
                set_and_draw(bottommiddle);
            }
    }
#undef set_and_draw
}
template<typename T>
inline void destructor(T a){
    a.T::~T();
}



#define DESTRUCTOR(A) destructor<decltype(A)>((A))
int main()
{
    char cdirectory[2048];
    getcwd(cdirectory,2048);
    if(cdirectory[0]=='\0'){
        std::cout<<"coudlnt get working dir"<<std::endl;
        return 1;
    }
    WORKING_DIR=cdirectory;
    WORKING_DIR=WORKING_DIR.substr(0,WORKING_DIR.find_last_of("/\\"));
    WORKING_DIR=WORKING_DIR.substr(0,WORKING_DIR.find_last_of("/\\"));
#ifdef __gnu_linux__
    WORKING_DIR+="/";
#endif
#ifdef __WIN32
    WORKING_DIR+="\\";
#endif
    SDL_Window *mainwindow; /* Our window handle */
    SDL_GLContext maincontext; /* Our opengl context handle */
    Mix_Chunk *pong = NULL;
    Mix_Chunk *pong2 = NULL;
    Mix_Chunk *pong3 = NULL;
    if( SDL_Init( SDL_INIT_VIDEO| SDL_INIT_AUDIO ) < 0 )
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
            }
        }
    }
    //Initialize SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
        std::cout<<"SDL_mixer could not initialize! SDL_mixer Error: "<<Mix_GetError();
        return 1;
    }
     //Load sound effects
    pong = Mix_LoadWAV(WORKING_DIR_FILE("assets/sounds/pong.wav"));
    if( pong == NULL ) {
        std::cout<< "Failed to load scratch sound effect! SDL_mixer Error: "<<Mix_GetError();
        return 1;
    }
    pong2 = Mix_LoadWAV(WORKING_DIR_FILE("assets/sounds/pong2.wav"));
    if( pong2 == NULL ) {
        std::cout<< "Failed to load scratch sound effect! SDL_mixer Error: "<<Mix_GetError();
        return 1;
    }
    pong3 = Mix_LoadWAV(WORKING_DIR_FILE("assets/sounds/pong3.wav"));
    if( pong3 == NULL ) {
        std::cout<< "Failed to load scratch sound effect! SDL_mixer Error: "<<Mix_GetError();
        return 1;
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
        ball.entity.setPos({0.0f,8.0f});

        roof.setPos({-1.0f,1.10f});
        floor.setPos({-1.0f,-1.10f});
        roof.scale({4.0f,1.0f});
        floor.scale({4.0f,1.0f});

        leftwall.entity.setPos({-1.0f,0.0f});
        rightwall.entity.setPos({1.0f,0.0f});
        leftwall.entity.scale({0.025f,2.0f});
        rightwall.entity.scale({0.025f,2.0f});

        player1.entity.scale({0.025f, 0.49f});
        player2.entity.scale({0.025f, 0.49f});
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
    Shader shader(WORKING_DIR_FILE("assets/shaders/shader.vert"),WORKING_DIR_FILE("assets/shaders/shader.frag"));
    Shader fb_shader(WORKING_DIR_FILE("assets/shaders/framebuffer_shader.vert"),WORKING_DIR_FILE("assets/shaders/framebuffer_shader.frag"));
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
                if(keystates[SDL_GetScancodeFromKey(SDLK_j)])
                {
                    player1.entity.rotate(15.0f );
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
                ball.speed=glm::normalize(glm::reflect(glm::vec2(pos.x-oldpos.x,pos.y-oldpos.y),glm::vec2(0.0f,1.0f)))*glm::length(ball.speed);
                Mix_PlayChannel( -1, pong2, 0 );
            }
            if(ball_roof.checkCollision()){
                handleCollision(ball_roof);
                ball.speed=glm::normalize(glm::reflect(glm::vec2(pos.x-oldpos.x,pos.y-oldpos.y),glm::vec2(0.0f,-1.0f)))*glm::length(ball.speed);
                Mix_PlayChannel( -1, pong2, 0 );
            }
            if(ball_p1.checkCollision()){
                handleCollision(ball_p1);
                ball.speed=glm::normalize(glm::reflect(glm::vec2(pos.x-oldpos.x,pos.y-oldpos.y),glm::vec2(1.0f,0.0f)) + p1_speed_gain)
                        *
                        (glm::length(ball.speed)+glm::length(p1_speed_gain));
                Mix_PlayChannel( -1, pong, 0 );
            }
            if(ball_p2.checkCollision()){
                handleCollision(ball_p2);
                ball.speed=glm::normalize(glm::reflect(glm::vec2(pos.x-oldpos.x,pos.y-oldpos.y),glm::vec2(-1.0f,0.0f)) + p2_speed_gain)
                        *
                        (glm::length(ball.speed)+glm::length(p2_speed_gain));
                Mix_PlayChannel( -1, pong, 0 );
            }

            if(ball_leftwall.checkCollision()){
                points_p2++;
                ball.entity.setPos({0.0f,8.0f});//scale adjusted due the order it uses...
                ball.speed={0.0f,0.0f};
                Mix_PlayChannel( -1, pong3, 0 );
            }
            if(ball_rightwall.checkCollision()){
                points_p1++;
                ball.entity.setPos({0.0f,8.0f});
                ball.speed={0.0f,0.0f};
                Mix_PlayChannel( -1, pong3, 0 );
            }
            if(((ball.speed.y/ball.speed.x)>3.0f) || ((ball.speed.y/ball.speed.x)<-3.0f)){
                ball.speed.y/=2.0f;
                ball.speed.x*=4.0f;
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
                glViewport(0,0,originalScreenWidth,originalScreenHeight);
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glEnable(GL_DEPTH_TEST);
                glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

                glm::mat4 projection;//= glm::ortho(-1.0f,1.0f,-1.0f,1.0f,0.0f,1.0f);
                GLint projection_uniform=glGetUniformLocation(shader.Program, "projection");
                glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));


                glm::vec2 position;
                position=glm::vec2(-3.0f*1.06255f*0.125f,0.0f);
                if(points_p1>=5 || points_p2>=5){
                    drawdigit(-1,&shader,position,{0.125f,0.125f},DIGIT_TOP|DIGIT_MIDDLE|DIGIT_TOPLEFT|DIGIT_TOPRIGHT|DIGIT_BOTTOMLEFT);
                    position+=glm::vec2(1.0625f*0.125f,0.0f);
                    drawdigit((points_p1>=5)?1:2,&shader,position,{0.125f,0.125f});
                    position+=glm::vec2(1.0625f*0.125f,0.0f);
                    position+=glm::vec2(1.0625f*0.125f,0.0f);
                    drawdigit(-1,&shader,position,{0.125f,0.125f},DIGIT_TOPLEFT |DIGIT_BOTTOMLEFT |DIGIT_BOTTOMLEFT_MIDDLE|
                                                                  DIGIT_TOPRIGHT|DIGIT_BOTTOMRIGHT|DIGIT_BOTTOMRIGHT_MIDDLE);
                    position+=glm::vec2(1.0625f*0.125f,0.0f);
                    drawdigit(-1,&shader,position,{0.125f,0.125f},DIGIT_TOPMIDDLE|DIGIT_BOTTOMMIDDLE);
                    position+=glm::vec2(1.0625f*0.125f,0.0f);
                    drawdigit(-1,&shader,position,{0.125f,0.125f},DIGIT_BOTTOMLEFT |DIGIT_TOPLEFT|DIGIT_TOPLEFT_BOTTOMRIGHT|
                                                                  DIGIT_BOTTOMRIGHT|DIGIT_TOPRIGHT);
                    position+=glm::vec2(1.0625f*0.125f,0.0f);
                    drawdigit(5,&shader,position,{0.125f,0.125f});
                    started=false;
                }

                player1.entity.draw(&shader);
                player2.entity.draw(&shader);
                if(started) ball.entity.draw(&shader);
                roof.draw(&shader);
                floor.draw(&shader);
                leftwall.entity.draw(&shader);
                rightwall.entity.draw(&shader);




                unsigned int aux=points_p2;
                position=glm::vec2(0.25f,0.5f);
                //NOTE: when one of the points hits 20 I should put a you win screen
                first_point_p2=points_p2? false:true;
                while((aux/10) || (aux%10) || first_point_p2){
                    drawdigit(aux%10,&shader,position,{0.125f,0.125f});
                    position.x-=1.5f*0.125f;
                    aux=aux/10;
                    first_point_p2=false;//endless loop if I dont
                }

                aux=points_p1;
                position={-0.25f,0.5f};
                first_point_p1=points_p1? false:true;
                while((aux/10) || (aux%10) || first_point_p1){
                    drawdigit(aux%10,&shader,position,{0.125f,0.125f});
                    position.x-=1.5f*0.125f;
                    aux=aux/10;
                    first_point_p1=false;
                }


            fb.unbind();
        SDL_GetWindowSize(mainwindow,&screenWidth,&screenHeight);
        glViewport(0,0,screenWidth,screenHeight);
        glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        fb_shader.Use();
            glm::mat4 screenscaler;
            float aspectRatio=(float)screenWidth/(float)screenHeight;
            float inverseAspectRatio=(float)screenHeight/(float)screenWidth;
            if(aspectRatio>1.0f)
            screenscaler = glm::perspective(radians(59.2f),aspectRatio,0.1f,1.0f);
            else
            screenscaler = glm::perspective(radians(59.2f),inverseAspectRatio,0.1f,1.0f);
            GLint model_uniform=glGetUniformLocation(fb_shader.Program, "model");
            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(screenscaler));

            glBindVertexArray(quadVAO);//should scale the scale to the % of resolution
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, fb.texture);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindTexture(GL_TEXTURE_2D,0);
            glBindVertexArray(0);


        SDL_Delay(1);
        SDL_GL_SwapWindow(mainwindow);
        if(points_p1>=5 || points_p2>=5){
            points_p1=0;
            points_p2=0;
            ball.speed={0.0f,0.0f};
            SDL_Delay(3000);
        }
    }
    DESTRUCTOR(fb);
    DESTRUCTOR(shader);
    DESTRUCTOR(fb_shader);
    Mix_FreeChunk(pong);
    Mix_FreeChunk(pong2);
    Mix_FreeChunk(pong3);
    SDL_GL_DeleteContext(maincontext);
    SDL_DestroyWindow(mainwindow);
    SDL_Quit();

    return 0;
}

