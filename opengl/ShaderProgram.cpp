#include "ShaderProgram.hpp"
#include <fstream>

namespace ogl
{

ShaderProgram::ShaderProgram(const std::string &file_vs,
                             const std::string &file_gs,
                             const std::string &file_fs) 
{
  // Load and compile the shader code
  GLuint vertexShader = this->createShader(GL_VERTEX_SHADER,file_vs);
  GLuint geometryShader = this->createShader(GL_GEOMETRY_SHADER_EXT,file_gs);
  GLuint fragmentShader = this->createShader(GL_FRAGMENT_SHADER,file_fs);

  // Create a shader program
  mProgram = glCreateProgram();
  ogl::printOpenGLError();

  // Add and link the shaders to a program
  glAttachShader(mProgram,vertexShader);
  glAttachShader(mProgram,geometryShader);
  glAttachShader(mProgram,fragmentShader);
  glLinkProgram(mProgram);

  // If any error occurred, print it now
  GLint infologLength = 0;
  glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH,&infologLength);
  if (infologLength > 1)
  {
    GLint charsWritten  = 0;
    GLchar *infoLog = new GLchar[infologLength];
    glGetProgramInfoLog(mProgram, infologLength, &charsWritten, infoLog);
    fprintf(stderr, "Linker error in shader:\n %s %s\n",infoLog, infoLog);
    delete[] infoLog;
  }

  // Free the memory of the compiled shader sources
  // We only need the shader program for rendering!
  glDeleteShader(vertexShader);
  glDeleteShader(geometryShader);
  glDeleteShader(fragmentShader);
}

GLuint ShaderProgram::createShader(GLenum shaderType, const std::string &file)
{
  GLuint shaderHandle;
  // Load and compile shader
  {
    std::ifstream in(file, std::ios::binary | std::ios::in);
    if (!in.is_open())
      std::cerr<<"Could not open shader source file "<<file<<std::endl;

    //load file content to string
    std::string source((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    shaderHandle = glCreateShader(shaderType);
    ogl::printOpenGLError();
    const char* s = source.c_str();
    glShaderSource(shaderHandle,1,&s,NULL);
    glCompileShader(shaderHandle);
  }

  // Check status of shader
  {
    GLint status;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE)
    {
      GLint infoLogLength;
      glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infoLogLength);

      GLchar* strInfoLog = new GLchar[infoLogLength + 1];
      glGetShaderInfoLog(shaderHandle, infoLogLength, NULL, strInfoLog);

      fprintf(stderr, "Compilation error in shader:\n %s %s\n",file.c_str(), strInfoLog);
      delete[] strInfoLog;
    }
  }

  return shaderHandle;
}

ShaderProgram::~ShaderProgram()
{
  glDeleteProgram(mProgram);
}
}