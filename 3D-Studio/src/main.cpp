#define GLFW_INCLUDE_NONE
#include "Application.hpp"
#include "CallbackManager.h"

/**
 * 3D Studio for Computer Graphics course at the Umeå University.
 * Created by: Max Thorén (c20mtn)
 * First version: 2023-12-31
 */

int main(int argc, char** argv) {
    Application app("3D Studio", 1600, 1200);
    app.Run();

    return 0;
}