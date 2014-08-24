#ifndef PROGRAM_HPP_INCLUDED
#define PROGRAM_HPP_INCLUDED

#include "lib/glm.hpp"

#include <memory>
#include <string>

struct GLProgram;

class VertexShader;
class FragmentShader;

class Program {
public:
    Program();
    ~Program();

    Program(const Program &) = delete;
    Program& operator=(const Program &) = delete;

    bool attach(VertexShader & vs, FragmentShader & fs);
    void use();

    bool is_valid() const;

    void set_uniform(std::string name, int value);
    void set_uniform(std::string name, bool value);
    void set_uniform(std::string name, float value);
    void set_uniform(std::string name, glm::mat3 const & value);
    void set_uniform(std::string name, glm::mat4 const & value);
    void set_uniform(std::string name, glm::vec2 const & value);
    void set_uniform(std::string name, glm::vec3 const & value);
    void set_uniform(std::string name, glm::vec4 const & value);
private:
    std::unique_ptr<GLProgram> gl;

    bool linked;
};

#endif
