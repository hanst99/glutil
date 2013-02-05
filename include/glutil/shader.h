#pragma once


#include <GL/glew.h>
#include <SFML/OpenGL.h>

#include <iostream>
#include <string>
#include <algorithm>
#include <stdexcept>

namespace glutil {
    class Shader {
        private:
            GLuint shader_id;
        public:
            Shader(GLuint);
            ~Shader();
            Shader(const Shader&) = delete;
            Shader(Shader&&);
            friend class Program;
            static Shader loadFromStream(std::istream&, GLenum type);
            static Shader loadFromFile(const std::string&, GLenum type);
    };

    class Program {
        private:
            Program(GLuint);
            GLuint program_id;
        public:
            Program(const Program&) = delete;
            Program(Program&&);
           ~Program();
           GLint getUniformLocation(const std::string&);
           GLint getAttribLocation(const std::string&);
           void use(); 
           static Program fromFiles(const std::string& vertFile, const std::string& fragFile);
           static Program fromStreams(std::istream& vertStream,std::istream& fragStream);
           template <class Iterator>
            static Program fromShaders(Iterator begin, Iterator end);
    };


    template <class Iterator>
        Program Program::fromShaders(Iterator begin, Iterator end) {
            GLuint program_id = glCreateProgram();
            Program result{program_id};
            std::for_each(begin,end,[program_id](const Shader& shader){
                    glAttachShader(program_id, shader.shader_id);
                    });
            glLinkProgram(program_id);
            GLint link_ok;
            glGetProgramiv(program_id, GL_LINK_STATUS, &link_ok);
            if(link_ok == GL_FALSE) {
                GLint info_log_length;
                glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
                // info_log_length is > 0 because it includes the \0 terminator
                --info_log_length;
                std::string info_log(info_log_length, 'a');
                glGetProgramInfoLog(program_id, info_log_length,&info_log_length,&info_log.front());
                throw std::runtime_error{std::string{__func__} + "Failed to link program!\n" + info_log};
            }
            std::for_each(begin,end,[program_id](const Shader& shader) {
                    glDetachShader(program_id,shader.shader_id);
                    });
            return result;
        }
}

