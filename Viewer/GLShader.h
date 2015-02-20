#ifndef GLSHADER_H
#define GLSHADER_H

#include <iostream>
#include <cstdio>
#include <cstring>

#include <GL3/gl3w.h>
  
/**
 * @brief  OpenGL Shader program convenience class.
 */
class GLShader
{
public:
    GLShader();
    ~GLShader();
    
    /** Bind shader program
     */
    void    Bind();
    
    /** Bind shader program attribute location.
     * @param attribIndex   Index of attribute.
     * @param attribName    Name of attribute.
     * @param linkProg      Link program after binding attribute.
     */
    bool    BindAttribLocation( unsigned int attribIndex, const char* attribName, bool linkProg = false );
    
    /** Create empty shader program.
     *    You have to attach shaders and link the program by yourself.
     */
    void    CreateEmptyProgram();
    
    /** Create shader program from file.
     * @param vShaderName  File name of vertex shader.
     * @param fShaderName  File name of fragment shader.
     */
    bool    CreateProgramFromFile( const char* vShaderName, 
                                   const char* fShaderName );
    
    /** Create shader program from file.
     * @param vShaderName  File name of vertex shader.
     * @param gShaderName  File name of geometry shader.
     * @param fShaderName  File name of fragment shader.
     */
    bool    CreateProgramFromFile( const char* vShaderName, 
                                   const char* gShaderName, 
                                   const char* fShaderName );
    
    /** Create shader program from file.
     * @param vShaderName  File name of vertex shader.     
     * @param tcShaderName File name of tessellation control shader.
     * @param teShaderName File name of tessellation evaluation shader.
     * @param fShaderName  File name of fragment shader.
     */
    bool    CreateProgramFromFile( const char* vShaderName,
                                   const char* tcShaderName,
                                   const char* teShaderName,
                                   const char* fShaderName );

    /** Create shader program from file.
     * @param vShaderName  File name of vertex shader.     
     * @param tcShaderName File name of tessellation control shader.
     * @param teShaderName File name of tessellation evaluation shader.
     * @param gShaderName  File name of geometry shader.
     * @param fShaderName  File name of fragment shader.
     */
    bool    CreateProgramFromFile( const char* vShaderName,
                                   const char* tcShaderName,
                                   const char* teShaderName,
                                   const char* gShaderName, 
                                   const char* fShaderName );
                                                           
    /** Create shader program from strings.
     * @param vShaderText  String text of vertex shader.
     * @param vShaderLen   Length of vertex shader text.
     * @param fShaderText  String text of fragment shader.
     * @param fShaderLen   Length of fragment shader text.
     */
    bool    CreateProgramFromString( const char* vShaderText, const unsigned int vShaderLen,
                                     const char* fShaderText, const unsigned int fShaderLen );
    
    /** Attach shader from file.
     * @param shaderName  File name of shader.
     * @param shaderType  Type of shader.
     * @param linkProg    Link program after attaching shader.
     */
    bool    AttachShaderFromFile( const char* shaderName, GLenum shaderType, bool linkProg = false );
    
    /** Attach shader from string.
     * @param shaderText  String text of shader.
     * @param shaderLen   Length of shader text.
     * @param shaderType  Type of shader.
     * @param linkProg    Link program after attaching shader.
     */
    bool    AttachShaderFromString( const char* shaderText, const unsigned int shaderLen, GLenum shaderType, bool linkProg = false );
    
    /** Get shader program handle.
     */
    GLuint  GetProgHandle();
    
    /** Get uniform location of shader variable.
     * @param name  Uniform variable name
     */
    GLint   GetUniformLocation(const char* name);  
    
    /** Link shader program.
     *    Linking is only necessary if you build the shader program
     *    by yourself.
     */ 
    bool    Link();
    
    void    PrintInfo(FILE* fptr = stderr);
    
    /** Remove all shaders from shader program.
     */
    void    RemoveAllShaders();
    
    /** Release shader program.
     */
    void    Release();   
    
    
protected: 
    GLuint  createShaderFromFile(const char* shaderFilename, GLenum type);      
        
    /** Create shader from string.
     * @param shaderText  Shader text.
     * @param shaderLen   Length of shader text.
     * @param type        Shader type.
     * \return Shader id.
     */ 
    GLuint  createShaderFromString(const char* shaderText, const unsigned int shaderLen, GLenum type);   
    GLuint  readShaderFromFile(const char* shaderFilename, std::string &shaderContent );
      
    /** Print shader information log.
     * @param shader  Shader id.
     */
    bool    printShaderInfoLog( GLuint shader );
    
    /** Print program information log.
     */
    bool    printProgramInfoLog();   
    

private:
    GLuint       progHandle;
};

#endif // GLSHADER_H
