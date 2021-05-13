#pragma once
#include "common/icg_common.h"
#include "OpenGP/Eigen/Image.h"

#define STBI_ASSERT(x) 
#define STBI_MALLOC
#define STBI_REALLOC
#define STBI_FREE

// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Quad{
private:
    GLuint _vao; ///< vertex array object
    GLuint _pid; ///< GLSL shader program ID 
    GLuint _vbo_vpoint; ///< memory buffer
    GLuint _vbo_vtexcoord; ///< memory buffer
    GLuint _tex; ///< Texture ID
public:
	void init(const char* texture_filename) {
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        ///--- Compile the shaders
        _pid = OpenGP::load_shaders("Quad/vshader.glsl", "Quad/fshader.glsl");
        if(!_pid) exit(EXIT_FAILURE);       
        glUseProgram(_pid);
        
        ///--- Vertex one vertex Array
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
     
        ///--- Vertex coordinates
        {
            const GLfloat vpoint[] = { 
                                       /*V3*/ -1.0f, -1.0f, 0.0f,
                                       /*V4*/ +1.0f, -1.0f, 0.0f,
									   /*V1*/ - 1.0f, -3.0f, 0.0f,
									   /*V2*/ +1.0f, -3.0f, 0.0f, };
            ///--- Buffer
            glGenBuffers(1, &_vbo_vpoint);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo_vpoint);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);
        
            ///--- Attribute
            GLuint vpoint_id = glGetAttribLocation(_pid, "vpoint");
            glEnableVertexAttribArray(vpoint_id);
            glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }
        
        ///--- Texture coordinates
        {
            const GLfloat vtexcoord[] = { /*V1*/ 0.0f, 0.0f, 
                                          /*V2*/ 1.0f, 0.0f, 
                                          /*V3*/ 0.0f, 1.0f,
                                          /*V4*/ 1.0f, 1.0f}; 
            
            ///--- Buffer
            glGenBuffers(1, &_vbo_vtexcoord);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo_vtexcoord);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);
        
            ///--- Attribute
            GLuint vtexcoord_id = glGetAttribLocation(_pid, "vtexcoord");
            glEnableVertexAttribArray(vtexcoord_id);
            glVertexAttribPointer(vtexcoord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }
        
        ///--- Load texture
		glGenTextures(1, &_tex);
		glBindTexture(GL_TEXTURE_2D, _tex);

		// could wrap instead of clamp
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		int width, height, nrChannels;
		unsigned char *data = stbi_load(texture_filename, &width, &height, &nrChannels, 0);

		// uploads texture to GPU
        /*glTexImage2D(GL_TEXTURE_2D, /*level* 0, GL_RGB32F,
                     image.cols(), image.rows(), 0,
                     GL_RGB, GL_FLOAT, image.data());*/
		if (data){
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		} else {
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);

        GLuint tex_id = glGetUniformLocation(_pid, "tex");
        glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);

        ///--- to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }
       
    void cleanup(){
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &_vbo_vpoint);
        glDeleteBuffers(1, &_vbo_vtexcoord);
        glDeleteProgram(_pid);
        glDeleteVertexArrays(1, &_vao);
        glDeleteTextures(1, &_tex);
    }
    
    void draw(const mat4& M){
        glUseProgram(_pid);
        glBindVertexArray(_vao);
            ///--- Uniform for animation
            float t = glfwGetTime();
            glUniform1f(glGetUniformLocation(_pid, "time"), t);
        
            ///--- Bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _tex);

			///--- Upload transformation
			GLuint M_id = glGetUniformLocation(_pid, "M");
			glUniformMatrix4fv(M_id, 1, false, M.data());

            ///--- Draw
			// strip makes unnecessary to specify triangle
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(0);        
        glUseProgram(0);
    }
};
