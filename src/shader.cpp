#include <glutil/shader.h>
#include <iterator>
#include <stdexcept>
#include <fstream>
#include <algorithm>

namespace glutil {
    Shader::Shader(GLuint shader_id) : shader_id{shader_id} {
        if(!glIsShader(shader_id)) {
            throw std::runtime_error{std::string{__func__} + ":Not a valid shader id!"};
        }
    }
    Shader::~Shader() {
        if(shader_id) glDeleteShader(shader_id);
    }
    Shader::Shader(Shader&& from) : shader_id{from.shader_id} {
        from.shader_id = 0;
    }
    Shader Shader::loadFromStream(std::istream& in, GLenum type) {
        try {
            GLuint shader_id = glCreateShader(type);
            Shader result{shader_id};
            if(!shader_id) {
                throw std::runtime_error("Failed to create shader!");    
            }
            in >> std::noskipws;
            std::string shader_source{std::istream_iterator<char>{in},std::istream_iterator<char>{}};
            const char* shader_source_content = shader_source.data();
            GLint shader_source_length = shader_source.length();
            glShaderSource(shader_id, 1, &shader_source_content, &shader_source_length);
            glCompileShader(shader_id);
            GLint compile_ok;
            glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_ok);
            if(compile_ok == GL_FALSE) {
                GLint info_log_length;
                glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
                // info_log_length is > 0 because it includes the \0 terminator
                --info_log_length;
                std::string info_log(info_log_length, 'a');
                glGetShaderInfoLog(shader_id,info_log_length,&info_log_length, &info_log.front());
                throw std::runtime_error{std::string{"Failed to compile shader!\n"} + info_log};
            }
            return result;
        } catch(std::runtime_error& e) {
            throw std::runtime_error(std::string(__func__) + ":" + e.what());
        }
    }

    Shader Shader::loadFromFile(const std::string& filename, GLenum type) {
        std::ifstream ifs{filename};
        if(!ifs) {
            throw std::runtime_error(std::string{__func__} + "Couldn't open file: " + filename);
        }
        try {
            return Shader::loadFromStream(ifs, type);
        } catch(std::runtime_error& e) {
            throw std::runtime_error{std::string{__func__} + ":In file: " + filename + "\n" + e.what()};
        }
    }

    Program::Program(GLuint program_id) : program_id{program_id} {} 
    Program::Program(Program&& from) : program_id{from.program_id} {
        from.program_id = 0;
    }
    Program::~Program() {
        if(program_id) glDeleteProgram(program_id);
    }

    void Program::use() {
        glUseProgram(program_id);
    }

    GLint Program::getUniformLocation(const std::string& name) {
        return glGetUniformLocation(program_id, name.c_str());
    }

    GLint Program::getAttribLocation(const std::string& name) {
        return glGetAttribLocation(program_id, name.c_str());
    }

    Program Program::fromFiles(const std::string& vertFile, const std::string& fragFile) {
        Shader shaders[]{Shader::loadFromFile(vertFile, GL_VERTEX_SHADER),Shader::loadFromFile(fragFile, GL_FRAGMENT_SHADER)}; 
        return Program::fromShaders(std::begin(shaders),std::end(shaders));
    }

    Program Program::fromStreams(std::istream& vertStream, std::istream& fragStream) {
        Shader shaders[]={Shader::loadFromStream(vertStream, GL_VERTEX_SHADER), Shader::loadFromStream(fragStream,GL_FRAGMENT_SHADER)};
        return Program::fromShaders(std::begin(shaders),std::end(shaders));
    }

}
