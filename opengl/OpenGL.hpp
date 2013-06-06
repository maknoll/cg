#ifndef OPENGL_HEADER_INCLUDE_ONCE
#define OPENGL_HEADER_INCLUDE_ONCE

#include <GL/glew.h>
#include <GLUT/GLUT.h>
#include <iostream>
#include <string>
#include "Math.hpp"

namespace ogl
{

#define printOpenGLError() printOglError(__FILE__, __LINE__)
  inline int printOglError(const std::string &file, int line)
  {
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
      std::cerr<<"glError in file "<<file<<" @ line "<<line<<":"<<gluErrorString(glErr)<<std::endl;
      retCode = 1;
    }
    return retCode;
  }

  inline void printContextInformation()
  {
    std::cout<<"OpenGL Context Information:"<<std::endl;
    std::cerr<<"Renderer: "<<glGetString(GL_RENDERER)<<std::endl;
    std::cerr<<"Vendor: "<<glGetString(GL_VENDOR)<<std::endl;
    std::cerr<<"Version: "<<glGetString(GL_VERSION)<<std::endl;

    GLint profile;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK,&profile);

    switch (profile)
    {
    case GL_CONTEXT_CORE_PROFILE_BIT:
      std::cout<<"Profile: Core"<<std::endl;
      break;
    case GL_CONTEXT_COMPATIBILITY_PROFILE_BIT:
      std::cout<<"Profile: Compatibility"<<std::endl;
      break;
    default:
      std::cout<<"Profile: unknown ("<<profile<<")"<<std::endl;
      break;
    }

    std::cout<<std::endl;
  }
}

#endif //OPENGL_HEADER_INCLUDE_ONCE
