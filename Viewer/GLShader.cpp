
#include "GLShader.h"

#include <fstream>
#include <sstream>

GLShader::GLShader() {
    progHandle = 0;
}

GLShader::~GLShader() {
    RemoveAllShaders();
}

bool GLShader::CreateProgramFromFile(const char* vShaderName, const char* fShaderName) {
    GLuint vShader = createShaderFromFile(vShaderName,GL_VERTEX_SHADER);
    if (vShader==0)  return false;
    GLuint fShader = createShaderFromFile(fShaderName,GL_FRAGMENT_SHADER);
    if (fShader==0)  return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader );
    glAttachShader(progHandle, fShader );
    return Link();
}

bool GLShader::CreateProgramFromFile(const char* vShaderName, 
        const char* gShaderName, const char* fShaderName) {
        
    GLuint vShader = createShaderFromFile(vShaderName,GL_VERTEX_SHADER);
    if (vShader==0)  return false;
    GLuint gShader = createShaderFromFile(gShaderName,GL_GEOMETRY_SHADER);
    if (gShader==0)  return false;
    GLuint fShader = createShaderFromFile(fShaderName,GL_FRAGMENT_SHADER);
    if (fShader==0)  return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader );
    glAttachShader(progHandle, gShader );
    glAttachShader(progHandle, fShader );
    return Link();
}

bool GLShader::CreateProgramFromFile( const char* vShaderName,
        const char* tcShaderName, const char* teShaderName,
        const char* fShaderName ) {
 
    GLuint vShader = createShaderFromFile(vShaderName,GL_VERTEX_SHADER);
    if (vShader==0)  return false;
    GLuint tcShader = createShaderFromFile(tcShaderName,GL_TESS_CONTROL_SHADER);
    if (tcShader==0)  return false;
    GLuint teShader = createShaderFromFile(teShaderName,GL_TESS_EVALUATION_SHADER);
    if (teShader==0)  return false;
    GLuint fShader = createShaderFromFile(fShaderName,GL_FRAGMENT_SHADER);
    if (fShader==0)  return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader );
    glAttachShader(progHandle, tcShader );
    glAttachShader(progHandle, teShader );
    glAttachShader(progHandle, fShader );
    return Link();
}

bool GLShader::CreateProgramFromFile( const char* vShaderName,
        const char* tcShaderName, const char* teShaderName,
        const char* gShaderName, const char* fShaderName ) {
 
    GLuint vShader = createShaderFromFile(vShaderName,GL_VERTEX_SHADER);
    if (vShader==0)  return false;
    GLuint tcShader = createShaderFromFile(tcShaderName,GL_TESS_CONTROL_SHADER);
    if (tcShader==0)  return false;
    GLuint teShader = createShaderFromFile(teShaderName,GL_TESS_EVALUATION_SHADER);
    if (teShader==0)  return false;
    GLuint gShader = createShaderFromFile(gShaderName,GL_GEOMETRY_SHADER);
    if (gShader==0)  return false;
    GLuint fShader = createShaderFromFile(fShaderName,GL_FRAGMENT_SHADER);
    if (fShader==0)  return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader );
    glAttachShader(progHandle, tcShader );
    glAttachShader(progHandle, teShader );
    glAttachShader(progHandle, gShader );
    glAttachShader(progHandle, fShader );
    return Link();
}

bool GLShader::CreateProgramFromString(const char* vShaderText, const unsigned int vShaderLen,
                                             const char* fShaderText, const unsigned int fShaderLen) {
    GLuint vShader = createShaderFromString(vShaderText,vShaderLen,GL_VERTEX_SHADER);
    if (vShader==0)  return false;
    GLuint fShader = createShaderFromString(fShaderText,fShaderLen,GL_FRAGMENT_SHADER);
    if (fShader==0)  return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader);
    glAttachShader(progHandle, fShader);
    return Link();
}

bool GLShader::AttachShaderFromFile( const char* shaderName, GLenum shaderType, bool linkProg ) {
    if (progHandle==0) {
        return false;  
    }
    GLuint shaderHandle = createShaderFromFile(shaderName,shaderType);
    if (shaderHandle==0) {
        return false;
    }
    glAttachShader(progHandle,shaderHandle);
    if (linkProg) {
        return Link();
    }
    return true;
}

bool GLShader::AttachShaderFromString( const char* shaderText, const unsigned int shaderLen, GLenum shaderType, bool linkProg ) {
    if (progHandle==0) {
        return false;
    }
    GLuint shaderHandle = createShaderFromString(shaderText,shaderLen,shaderType);
    if (shaderHandle==0) {
        return false;
    }
    glAttachShader(progHandle,shaderHandle);
    if (linkProg) {
        return Link();
    }
    return true;
}

/*!
 */
void GLShader::Bind() {
    glUseProgram(progHandle);
}

bool GLShader::BindAttribLocation( unsigned int attribIndex, const char* attribName, bool linkProg ) {
    glBindAttribLocation( progHandle, attribIndex, attribName ); 
    if (linkProg) {
        return Link();
    }
    return true;    
}

void GLShader::CreateEmptyProgram() {
   if (progHandle!=0) {
       RemoveAllShaders();
   }
   progHandle = glCreateProgram();
}

void GLShader::Release() {
    glUseProgram(0);
}

GLuint GLShader::GetProgHandle() {
    return progHandle;
}

GLint GLShader::GetUniformLocation(const char* name) {
    return glGetUniformLocation(progHandle,name);
}

bool GLShader::Link() {
    glLinkProgram(progHandle);
    bool status = printProgramInfoLog();
    glUseProgram(0);
    return (status==GL_TRUE);
}

GLuint GLShader::readShaderFromFile(const char* shaderFilename, std::string &shaderContent ) {
    if (shaderFilename[0]=='\0') {
        return 0;
    }

    std::ifstream in(shaderFilename);  
    if (!in) {
       const unsigned int length = 256;
       char msg[length];
       sprintf(msg,"GLShader::readShaderFromFile() ... Cannot open file \"%s\"",shaderFilename);
       fprintf(stderr,"Error: %s\n",msg);
       return 0;
    }

    std::stringstream shaderData;
    shaderData << in.rdbuf();
    in.close();
    shaderContent = shaderData.str();
    return shaderContent.size();
}

GLuint GLShader::createShaderFromFile(const char* shaderFilename, GLenum type ) {
    std::string shaderText;
    GLuint iShaderLen = readShaderFromFile(shaderFilename,shaderText);
    if (iShaderLen==0) {
        return 0;
    }

    GLuint shader = glCreateShader(type);
    const char *strShaderVar = shaderText.c_str();
    glShaderSource(shader,1,(const GLchar**)&strShaderVar, (GLint*)&iShaderLen);
    glCompileShader(shader);

    if (!printShaderInfoLog(shader)) {
        std::stringstream iss(shaderText);
        std::string sLine;
        int lineCounter = 1;
        while(std::getline(iss,sLine)) {
            fprintf(stderr,"%4d : %s\n",(lineCounter++),sLine.c_str());
        }
    }
    return shader;
}

GLuint GLShader::createShaderFromString(const char* shaderText, const unsigned int shaderLen, GLenum type) {    
    if (shaderLen==0) {
        return 0;
    }

    GLuint shader = glCreateShader(type);
    glShaderSource(shader,1,(const GLchar**)&shaderText, (GLint*)&shaderLen);
    glCompileShader(shader);
   
    if (!printShaderInfoLog(shader)) {
        std::stringstream iss(shaderText);
        std::string sLine;
        int lineCounter = 1;
        while(std::getline(iss,sLine)) {
            fprintf(stderr,"%4d : %s\n",(lineCounter++),sLine.c_str());
        }
    }
    return shader;
}

bool GLShader::printShaderInfoLog( GLuint shader ) {
    int infoLogLen   = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    int compileStatus = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS,  &compileStatus);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 1)  {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
        printf("InfoLog : %s\n", infoLog);
        delete [] infoLog;
    }
    return (compileStatus == GL_TRUE);
}

bool GLShader::printProgramInfoLog() {
    int infoLogLen   = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    int linkStatus = 0;
    glGetProgramiv(progHandle, GL_INFO_LOG_LENGTH, &infoLogLen);
    glGetProgramiv(progHandle, GL_LINK_STATUS, &linkStatus);

    if (infoLogLen > 1)  {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetProgramInfoLog(progHandle, infoLogLen, &charsWritten, infoLog);
        printf("ProgramInfoLog : %s\n", infoLog);
        delete [] infoLog;
    }
    return (linkStatus == GL_TRUE);
}

void GLShader::RemoveAllShaders() {
    if (progHandle==0) {
        return;
    }
      
    if (!glIsProgram(progHandle)) {
        return;
    }
   
    const GLsizei numShaders = 1024;
    GLsizei numReturned;
    GLuint shaders[numShaders];
    glUseProgram(0);
   
    glGetAttachedShaders(progHandle, numShaders, &numReturned, shaders);
    for (GLsizei i = 0; i < numReturned; i++) {
        glDetachShader(progHandle, shaders[i]);
        glDeleteShader(shaders[i]);
    }
    glDeleteProgram(progHandle);
    progHandle = 0;
}
