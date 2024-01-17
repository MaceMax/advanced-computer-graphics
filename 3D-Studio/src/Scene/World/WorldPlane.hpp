#include "3dStudio.hpp"

class WorldPlane {
   public:
    WorldPlane();

    void Draw();

   private:
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    GLuint length;
};