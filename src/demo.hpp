#ifndef DEMO_HPP_INCLUDED
#define DEMO_HPP_INCLUDED

#include "firstpersoncontrol.hpp"
#include "mesh.hpp"
#include "perspectivecamera.hpp"
#include "program.hpp"
#include "texture2d.hpp"
#include "world.hpp"

struct Billboard {
    Mesh mesh;
    Texture2D texture;
};

class Demo : public World {
public:
    Demo();
    bool create(std::shared_ptr<Window> window) override;
    void update(seconds delta, seconds elapsed, Input & input) override;
    void render() override;
    void destroy() override;
private:
    bool create_shaders();
    bool create_billboards();
    void create_floor();

    void update_dimension();
    void update_true(Billboard & billboard);

    void render_cheap();
    void render_true();

    void draw_floor();
    void draw_true(Billboard & billboard);
    void draw_cheap_cpu(Billboard & billboard);
    void draw_cheap_gpu(Billboard & billboard);

    std::shared_ptr<Window> window;
    int width;
    int height;

    Program basic_program;
    Program billboard_program;
    Program texture_program;

    PerspectiveCamera camera;
    FirstPersonControl camera_control;

    Mesh floor;
    std::vector<std::unique_ptr<Billboard>> billboards;

    bool controls;
    bool billboarding;
    bool gpu;
    bool spherical;
    bool cheap_method;
};

#endif
