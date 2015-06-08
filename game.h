#ifndef GAME
#define GAME
extern "C"{
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glu.h>
}
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#define ABS(X) (((X)>0)? (X):(-(X)))
#define SIGN(X) ((X)/ABS((X)))
#define LERP(A,B,T) ( (A) + ( (B) - (A) ) * (T) )
#define INSIDE(X,A,B) ( (( (X)>=(A) ) && ( (X)<=(B) )) || ( ( (X)>=(B) ) && ( (X)<=(A) ) ) )
#define SQUARE(X) (((X)*(X)))
#define MIN(X,Y) (((X)>(Y))? (Y): (X))
#define MAX(X,Y) (((X)>(Y))? (X): (Y))
#define PI 3.14159265359f
constexpr float radians(float degrees){
    return degrees*(PI/180.0f);
}
struct Vertex{
    glm::vec3 position;
};
void sdldie(const char *msg)
{
    std::cout<<msg<<' '<<SDL_GetError()<<std::endl;
    SDL_Quit();
    exit(1);
}
void checkSDLError(int line = -1)
{
#ifndef NDEBUG
    const char *error = SDL_GetError();
    if (*error != '\0')
    {
        std::cout<<error<<std::endl<<" + line:"<<line<<std::endl;
        SDL_ClearError();
    }
#endif
}
struct framebuffer{
    GLuint fbo;
    GLuint texture;
    GLuint rbo;

    framebuffer(GLuint width,GLuint height){
        glGenFramebuffers(1, &this->fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
            glGenTextures(1, &this->texture);
            glBindTexture(GL_TEXTURE_2D, this->texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width,height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
            glBindTexture(GL_TEXTURE_2D,0);

            glGenRenderbuffers(1, &this->rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->rbo);
            glBindRenderbuffer(GL_RENDERBUFFER,0);


            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->rbo);

            if(!this->checkStatus())
                sdldie("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    };
    ~framebuffer(){
        glDeleteFramebuffers(1, &this->fbo);
    };
    GLint oldfb;
    inline void bind(){
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,&oldfb);
        glBindFramebuffer(GL_FRAMEBUFFER,this->fbo);
    }
    inline void unbind(){
        glBindFramebuffer(GL_FRAMEBUFFER,oldfb);
    }
    inline bool checkStatus(){
        return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    }
};

struct Sprite{
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
//  glm::mat4 matrix;
//  float rotated;
  Sprite(){

  }
  Sprite(std::vector<Vertex> _vertices,std::vector<GLuint> _indices)
  {
      this->vertices=_vertices;
      this->indices=_indices;
      //this->rotated=0.0f;
      this->setupMesh();
  }
//  void setPosition(glm::vec3 pos){
//      this->matrix=glm::translate(matrix,pos);
//  }
//  void setScale(glm::vec3 scale){
//      this->matrix=glm::scale(matrix,scale);
//  }
//  void rotate(float degrees){
//      this->matrix=glm::rotate(matrix,glm::radians(degrees),glm::vec3(0.0f,0.0f,1.0f));
//      this->rotated+=degrees;
//  }
  /*  Render data  */
  GLuint VAO, VBO, EBO;

  /*  Functions    */
  // Initializes all the buffer objects/arrays
  void setupMesh()
  {
      // Create buffers/arrays
      glGenVertexArrays(1, &this->VAO);
      glGenBuffers(1, &this->VBO);
      glGenBuffers(1, &this->EBO);

      glBindVertexArray(this->VAO);
      // Load data into vertex buffers
      glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
      // A great thing about structs is that their memory layout is sequential for all its items.
      // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
      // again translates to 3/2 floats which translates to a byte array.
      glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

      // Set the vertex attribute pointers
      // Vertex Positions
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
//      // Vertex Normals
//      glEnableVertexAttribArray(1);
//      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
//      // Vertex Texture Coords
//      glEnableVertexAttribArray(2);
//      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

//      glEnableVertexAttribArray(3);
//      glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));

//      glEnableVertexAttribArray(4);
//      glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));

      glBindVertexArray(0);
  }

  void Draw(Shader* shader)
  {
      shader=shader;//NOTE: to stop warning
      glBindVertexArray(this->VAO);
      glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);

  }
  ~Sprite(){
      glDeleteVertexArrays(1, &this->VAO);
      glDeleteBuffers(1, &this->VBO);
      glDeleteBuffers(1, &this->EBO);
  }
};
enum Order{
    TRANSLATE,
    ROTATE,
    SCALE
};
struct Hitbox{
    glm::vec2 bottomleft;
    glm::vec2 width_height;
    Hitbox(glm::vec2 bl,glm::vec2 wh){
        this->bottomleft=bl;
        this->width_height=wh;
    }
    Hitbox(){}
};
Hitbox getBiggestHitbox(std::vector<Vertex> vertices){
    auto aux=vertices.begin();
    glm::vec2 bottomleft(aux->position.x,aux->position.y);
    glm::vec2 topright(aux->position.x,aux->position.y);
    for(auto i=vertices.begin()+1;i!=vertices.end();i++){
        if(i->position.x < bottomleft.x) bottomleft.x=i->position.x;
        if(i->position.y < bottomleft.y) bottomleft.y=i->position.y;
        if(i->position.x > topright.x) topright.x=i->position.x;
        if(i->position.y > topright.y) topright.y=i->position.y;
    }
    return Hitbox(bottomleft,topright-bottomleft);
}
struct Entity{
    Sprite* sprite;
    glm::vec3 position;
    glm::vec3 oldPosition;
    float _rotate;
    float _oldRotate;
    glm::vec3 _scale;
    glm::mat4 model;
    Order _order[3];

    Hitbox hitbox;//object space

    Entity(){

    }
    Entity(Sprite* _sprite){
        this->sprite=_sprite;
        this->position=glm::vec3(0.0f);
        this->oldPosition=glm::vec3(0.0f);
        this->_rotate=0.0f;
        this->_scale=glm::vec3(1.0f);
        this->model=glm::mat4();

        this->_order[0]=TRANSLATE;
        this->_order[1]=ROTATE;
        this->_order[2]=SCALE;

        this->hitbox=getBiggestHitbox(this->sprite->vertices);
    }
    void move(glm::vec2 pos){
        this->oldPosition=this->position;
        this->position.x+=pos.x;
        this->position.y+=pos.y;
    }
    void setPos(glm::vec2 pos){
        this->oldPosition=this->position;
        this->position.x=pos.x;
        this->position.y=pos.y;
    }
    void rotate(float rot){
        _oldRotate=_rotate;
        _rotate+=rot;
    }
    void scale(glm::vec2 scal){
        this->_scale.x=scal.x;
        this->_scale.y=scal.y;

        //@hack: not sure
//        this->hitbox.width_height.x*=scal.x;
//        this->hitbox.width_height.y*=scal.y;
//        glm::vec3 aux(this->hitbox.bottomleft.x,this->hitbox.bottomleft.y,0.0f);
//        glm::mat4 mataux = glm::scale(glm::mat4(),glm::vec3(scal.x,scal.y,1.0f));
//        aux = mataux * aux;
//        this->hitbox.bottomleft;
//        this->hitbox.bottomleft.x=this->position.x-this->hitbox.width_height.x/2;
//        this->hitbox.bottomleft.y=this->position.y-this->hitbox.width_height.y/2;
    }
    void order(Order ord0,Order ord1,Order ord2){
        this->_order[0]=ord0;
        this->_order[1]=ord1;
        this->_order[2]=ord2;
    }
    Hitbox getHitbox(){
        glm::mat4 model;
        bool rotated=false;
        switch (this->_order[0]){
            case TRANSLATE:
                model=glm::translate(model,this->position);
                break;
             case ROTATE:
                model=glm::rotate(model,glm::radians(_rotate),glm::vec3(0.0f,0.0f,1.0f));
                if(_rotate!=0.0f)rotated=true;
                break;
             case SCALE:
                model=glm::scale(model,_scale);
                break;
        }
        switch (this->_order[1]){
            case TRANSLATE:
                model=glm::translate(model,this->position);
                break;
             case ROTATE:
                model=glm::rotate(model,glm::radians(_rotate),glm::vec3(0.0f,0.0f,1.0f));
                if(_rotate!=0.0f)rotated=true;
                break;
             case SCALE:
                model=glm::scale(model,_scale);
                break;
        }
        switch (this->_order[2]){
            case TRANSLATE:
                model=glm::translate(model,this->position);
                break;
             case ROTATE:
                model=glm::rotate(model,glm::radians(_rotate),glm::vec3(0.0f,0.0f,1.0f));
                if(_rotate!=0.0f)rotated=true;
                break;
             case SCALE:
                model=glm::scale(model,_scale);
                break;
        }

        glm::vec4 bottomleft(this->hitbox.bottomleft.x,this->hitbox.bottomleft.y,0.0f,1.0f);
        glm::vec4 topright = bottomleft + glm::vec4(this->hitbox.width_height.x,this->hitbox.width_height.y,0.0f,0.0f);
        //simplemente re calculamos el hitbox con los 2 vertex que se poseen despues de transformar

        glm::vec4 bottomright=bottomleft+glm::vec4(this->hitbox.width_height.x,0.0f,0.0f,0.0f);
        glm::vec4 topleft=bottomleft+glm::vec4(0.0f,this->hitbox.width_height.y,0.0f,0.0f);
        bottomright=model*bottomright;
        topleft=model*topleft;
        bottomleft=model*bottomleft;
        topright=model*topright;
            //std::cout<<"HACER EL CASO QUE SE HAYA ROTADO\n";assert(0);}//@TODO
        if(rotated){
            std::vector<Vertex> vertices;
            vertices.reserve(4);
            Vertex aux;
            aux.position=glm::vec3(bottomleft.x,bottomleft.y,0.0f);
            vertices.push_back(aux);
            aux.position=glm::vec3(topright.x,topright.y,0.0f);
            vertices.push_back(aux);
            aux.position=glm::vec3(bottomright.x,bottomright.y,0.0f);
            vertices.push_back(aux);
            aux.position=glm::vec3(topleft.x,topleft.y,0.0f);
            vertices.push_back(aux);
            return getBiggestHitbox(vertices);
        }
        glm::vec2 bottomleft2(bottomleft.x,bottomleft.y);
        glm::vec2 topright2(topright.x,topright.y);
        return Hitbox(bottomleft2,topright2-bottomleft2);
    }
    void draw(Shader* shader){
        this->model=glm::mat4();
        switch (this->_order[0]){
            case TRANSLATE:
                this->model=glm::translate(this->model,this->position);
                break;
             case ROTATE:
                this->model=glm::rotate(this->model,glm::radians(_rotate),glm::vec3(0.0f,0.0f,1.0f));
                break;
             case SCALE:
                this->model=glm::scale(this->model,_scale);
                break;
        }
        switch (this->_order[1]){
            case TRANSLATE:
                this->model=glm::translate(this->model,this->position);
                break;
             case ROTATE:
                this->model=glm::rotate(this->model,glm::radians(_rotate),glm::vec3(0.0f,0.0f,1.0f));
                break;
             case SCALE:
                this->model=glm::scale(this->model,_scale);
                break;
        }
        switch (this->_order[2]){
            case TRANSLATE:
                this->model=glm::translate(this->model,this->position);
                break;
             case ROTATE:
                this->model=glm::rotate(this->model,glm::radians(_rotate),glm::vec3(0.0f,0.0f,1.0f));
                break;
             case SCALE:
                this->model=glm::scale(this->model,_scale);
                break;
        }
        GLint model_uniform= glGetUniformLocation(shader->Program, "model");
        switch(model_uniform){
            case GL_INVALID_VALUE:
                std::cout<<"No existe este valor"<<std::endl;
                break;
            case GL_INVALID_OPERATION:
                std::cout<<"program is not a program object"<<std::endl;
                break;
        }
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(this->model));
        this->sprite->Draw(shader);
    }
};
struct CollisionChecker{
    Entity* e1;
    Entity* e2;
    CollisionChecker(Entity* e1,Entity* e2){
        this->e1=e1;
        this->e2=e2;
    }
    bool checkCollision(){
        Hitbox HB1=e1->getHitbox();
        Hitbox HB2=e2->getHitbox();
//        if (rect1.x < rect2.x + rect2.width &&
//           rect1.x + rect1.width > rect2.x &&
//           rect1.y < rect2.y + rect2.height &&
//           rect1.height + rect1.y > rect2.y) {
//            // collision detected!
//        }
        if ((HB1.bottomleft.x < (HB2.bottomleft.x + HB2.width_height.x))&&
            ((HB1.bottomleft.x + HB1.width_height.x) > HB2.bottomleft.x)&&
            (HB1.bottomleft.y < (HB2.bottomleft.y+HB2.width_height.y))&&
            ((HB1.bottomleft.y+HB1.width_height.y)>HB2.bottomleft.y))
            return true;
        return false;
    }
};
void handleCollision(CollisionChecker collisionChecker){
    {
        glm::vec3 oldpos,pos,aux;
        oldpos=collisionChecker.e1->oldPosition;
        pos=collisionChecker.e1->position;
        for(float scale=0.9f;scale>0.0f;scale-=0.01f){//@NOTE: if the speed is high enough, it breaks due not enought precision
            aux=LERP(oldpos,pos,scale);
            collisionChecker.e1->position={aux.x,aux.y,0.0f};
            if(!collisionChecker.checkCollision())
            {
                return;
            }
        }
        collisionChecker.e1->position=oldpos;
    }


    if(collisionChecker.checkCollision()){
        float oldpos,pos,aux;
        oldpos=collisionChecker.e1->_oldRotate;
        pos=collisionChecker.e1->_rotate;
        for(float scale=0.9f;scale>0.0f;scale-=0.01f){//@NOTE: if the speed is high enough, it breaks due not enought precision
            aux=LERP(oldpos,pos,scale);
            collisionChecker.e1->_rotate=aux;
            if(!collisionChecker.checkCollision())
            {
                return;
            }
        }
        collisionChecker.e1->_rotate=oldpos;
    }
//    std::cout<<"couldnt handle collision\n";
//    assert(0);
}
#endif // GAME

