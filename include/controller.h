#ifndef _CONTROLLER_
#define _CONTROLLER_

#include <defines.h>

struct SceneData;

class Controller {
    SceneData& data;


    GLint lightCarried = -1;
    GLuint lightCurrent = 0;

    public:
        Controller(SceneData& data);



};



#endif