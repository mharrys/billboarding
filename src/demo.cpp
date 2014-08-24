#include "demo.hpp"

#include "fragmentshader.hpp"
#include "image.hpp"
#include "pngloader.hpp"
#include "vertexshader.hpp"

#include "lib/gl.hpp"

#include <iostream>

Demo::Demo()
    : controls(true),
      billboarding(true),
      gpu(false),
      spherical(false),
      cheap_method(false)
{
}

bool Demo::create(std::shared_ptr<Window> window)
{
    this->window = window;
    window->request_pointer_lock(true);

    update_dimension();

    if (!create_shaders()) {
        return false;
    }

    if (!create_billboards()) {
        return false;
    }

    create_floor();

    camera.far = 2000.0f;
    camera.translate_z(140.0f);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::cout << "F1: toggle billboarding." << std::endl;
    std::cout << "F2: toggle adjusting model-view matrix for cheap method on GPU or CPU." << std::endl;
    std::cout << "F3: toggle between spherical and cylindrical billboarding." << std::endl;
    std::cout << "F4: toggle between cheap and true method." << std::endl;
    std::cout << "F5: toggle controls." << std::endl;

    return true;
}

void Demo::update(seconds dt, seconds, Input & input)
{
    update_dimension();

    if (input.pressed(Key::F1)) {
        billboarding = !billboarding;
        std::cout << "billboarding: " << (billboarding ? "on" : "off") << std::endl;
        if (billboarding) {
            glEnable(GL_CULL_FACE);
        } else {
            glDisable(GL_CULL_FACE);
        }
    }

    if (input.pressed(Key::F2)) {
        gpu = !gpu;
        std::cout << "adjusting: " << (gpu ? "GPU" : "CPU")  << std::endl;
    }

    if (input.pressed(Key::F3)) {
        spherical = !spherical;
        std::cout << "type: " << (spherical ? "spherical" : "cylindrical") << std::endl;
    }

    if (input.pressed(Key::F4)) {
        cheap_method = !cheap_method;
        std::cout << "method: " << (cheap_method ? "cheap" : "true") << std::endl;
    }

    if (input.pressed(Key::F5)) {
        controls = !controls;
        window->request_pointer_lock(controls);
    }

    if (controls) {
        camera_control.update(dt, input, camera);
        camera.update_world_transform();
    }

    if (billboarding && !cheap_method) {
        for (unsigned int i = 0; i < billboards.size(); i++) {
            update_true(*billboards[i].get());
        }
    } else {
        // keep resetting the billboard orientations (for true method)
        for (unsigned int i = 0; i < billboards.size(); i++) {
            billboards[i]->mesh.orientation = glm::quat();
            billboards[i]->mesh.update_world_transform();
        }
    }
}

void Demo::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_floor();

    if (cheap_method) {
        render_cheap();
    } else {
        render_true();
    }

    gl_print_error();
}

void Demo::destroy()
{
    window->request_pointer_lock(false);
}

bool Demo::create_shaders()
{
    VertexShader basic_vs;
    basic_vs.compile_from_file("assets/shaders/basic.vs");

    FragmentShader basic_fs;
    basic_fs.compile_from_file("assets/shaders/basic.fs");

    VertexShader billboard_vs;
    billboard_vs.compile_from_file("assets/shaders/billboard_cheap.vs");

    VertexShader texture_vs;
    texture_vs.compile_from_file("assets/shaders/texture.vs");

    FragmentShader texture_fs;
    texture_fs.compile_from_file("assets/shaders/texture.fs");

    if (!basic_program.attach(basic_vs, basic_fs)) {
        return false;
    }

    if (!billboard_program.attach(billboard_vs, texture_fs)) {
        return false;
    }

    if (!texture_program.attach(texture_vs, texture_fs)) {
        return false;
    }

    return true;
}

bool Demo::create_billboards()
{
    TextureSetting texture_setting = {
        TextureFormat::RGBA, // internal
        TextureFormat::RGBA, // source
        {
            { FilterParameter::MIN_FILTER, FilterMode::LINEAR },
            { FilterParameter::MAG_FILTER, FilterMode::LINEAR }
        },
        {
            { WrapParameter::WRAP_S, WrapMode::CLAMP_TO_EDGE },
            { WrapParameter::WRAP_T, WrapMode::CLAMP_TO_EDGE }
        }
    };

    std::vector<glm::vec3> tree_positions = {
        glm::vec3( -50.0f, 3.0f,   50.0f),
        glm::vec3(-300.0f, 0.0f, -800.0f),
        glm::vec3(  30.0f, 0.0f, -420.0f),
        glm::vec3( 120.0f, 8.0f, -200.0f)
    };

    for (int i = 1; i <= 4; i++) {
        std::unique_ptr<Billboard> billboard(new Billboard());

        auto image = PNGLoader::load("assets/textures/tree" + std::to_string(i) + ".png");
        if (!image) {
            return false;
        }
        billboard->texture.make(*image.get(), texture_setting);

        const float w = image->width;
        const float h = image->height;
        billboard->mesh.positions = {
            glm::vec3(-w,  h, 0.0f),
            glm::vec3(-w, -h, 0.0f),
            glm::vec3( w, -h, 0.0f),
            glm::vec3( w,  h, 0.0f),
        };

        billboard->mesh.tex_coords = {
            glm::vec2(0.0f, 0.0f),
            glm::vec2(0.0f, 1.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(1.0f, 0.0f),
        };

        billboard->mesh.indices = {
            0, 1, 2,
            0, 2, 3,
        };

        billboard->mesh.update_positions = true;
        billboard->mesh.update_tex_coords = true;
        billboard->mesh.update_indices = true;

        billboard->mesh.position = tree_positions[i - 1];
        billboard->mesh.scale = glm::vec3(0.1f);
        billboard->mesh.update_world_transform();

        billboards.push_back(std::move(billboard));
    }

    return true;
}

void Demo::create_floor()
{
    floor.positions = {
        glm::vec3(-500, 1000.0f, 0.0f),
        glm::vec3(-500, -200.0f, 0.0f),
        glm::vec3( 500, -200.0f, 0.0f),
        glm::vec3( 500, 1000.0f, 0.0f),
    };

    floor.colors = {
        glm::vec3(0.1f),
        glm::vec3(0.2f),
        glm::vec3(0.2f),
        glm::vec3(0.1f)
    };

    floor.indices = {
        0, 1, 2,
        0, 2, 3
    };

    floor.update_positions = true;
    floor.update_colors = true;
    floor.update_indices = true;

    floor.rotate_x(-90.0f);
    floor.position.y = -30.0f;
    floor.update_world_transform();
}

void Demo::update_dimension()
{
    auto dimension = window->dimension();
    if (width != dimension.first || height != dimension.second) {
        width = dimension.first;
        height = dimension.second;
        glViewport(0, 0, width, height);
        camera_control.window_width = width;
        camera_control.window_height = height;
    }
}

void Demo::update_true(Billboard & billboard)
{
    auto & mesh = billboard.mesh;

    // cylindrical
    glm::vec3 direction_proj = camera.world_position() - mesh.world_position();
    direction_proj.y = 0.0f;
    direction_proj = glm::normalize(direction_proj);
    mesh.orientation = rotation_between(Z_UNIT, direction_proj);

    if (spherical) {
        glm::vec3 direction = camera.world_position() - mesh.world_position();
        direction = glm::normalize(direction);
        mesh.orientation = rotation_between(direction_proj, direction) * mesh.orientation;
    }

    billboard.mesh.update_world_transform();
}

void Demo::render_cheap()
{
    if (gpu) {
        billboard_program.use();
        billboard_program.set_uniform("billboarding", billboarding);
        billboard_program.set_uniform("spherical", spherical);
        for (unsigned int i = 0; i < billboards.size(); i++) {
            draw_cheap_gpu(*billboards[i].get());
        }
    } else {
        texture_program.use();
        for (unsigned int i = 0; i < billboards.size(); i++) {
            draw_cheap_cpu(*billboards[i].get());
        }
    }
}

void Demo::render_true()
{
    texture_program.use();
    for (unsigned int i = 0; i < billboards.size(); i++) {
        draw_true(*billboards[i].get());
    }
}

void Demo::draw_floor()
{
    basic_program.use();

    glm::mat4 m = floor.world_transform();
    glm::mat4 v = camera.view();
    glm::mat4 p = camera.projection();

    glm::mat4 mvp = p * v * m;

    basic_program.set_uniform("mvp", mvp);

    floor.draw();
}

void Demo::draw_true(Billboard & billboard)
{
    billboard.texture.bind(0);

    glm::mat4 m = billboard.mesh.world_transform();
    glm::mat4 v = camera.view();
    glm::mat4 p = camera.projection();

    glm::mat4 mvp = p * v * m;

    texture_program.set_uniform("mvp", mvp);

    billboard.mesh.draw();
}

void Demo::draw_cheap_cpu(Billboard & billboard)
{
    billboard.texture.bind(0);

    glm::mat4 m = billboard.mesh.world_transform();
    glm::mat4 v = camera.view();
    glm::mat4 p = camera.projection();

    glm::mat4 mv = v * m;
    if (billboarding) {
        mv[0][0] = billboard.mesh.scale.x;
        mv[0][1] = 0.0f;
        mv[0][2] = 0.0f;
        if (spherical) {
            mv[1][0] = 0.0f;
            mv[1][1] = billboard.mesh.scale.y;
            mv[1][2] = 0.0f;
        }
        mv[2][0] = 0.0f;
        mv[2][1] = 0.0f;
        mv[2][2] = billboard.mesh.scale.z;
    }
    glm::mat4 mvp = p * mv;

    texture_program.set_uniform("mvp", mvp);

    billboard.mesh.draw();
}

void Demo::draw_cheap_gpu(Billboard & billboard)
{
    billboard.texture.bind(0);

    glm::mat4 m = billboard.mesh.world_transform();
    glm::mat4 v = camera.view();
    glm::mat4 p = camera.projection();

    glm::mat4 mv = v * m;

    billboard_program.set_uniform("mv", mv);
    billboard_program.set_uniform("projection", p);
    billboard_program.set_uniform("scale", billboard.mesh.scale);

    billboard.mesh.draw();
}
