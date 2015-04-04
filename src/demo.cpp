#include "demo.hpp"

#include <random>

Demo::Demo(std::shared_ptr<gst::Logger> logger, std::shared_ptr<gst::Window> window)
    : logger(logger),
      window(window),
      renderer(gst::Renderer::create(logger)),
      render_size(window->get_size()),
      programs(logger),
      controls(true, 2.5f, 15.0f),
      billboarding(true),
      spherical(true),
      cheap_method(false)
{
}

bool Demo::create()
{
    window->set_pointer_lock(true);

    create_scene();
    create_billboards();

    logger->log("F1: toggle billboarding.");
    logger->log("F2: toggle between spherical and cylindrical billboarding.");
    logger->log("F3: toggle between cheap and true method.");

    return true;
}

void Demo::update(float delta, float)
{
    update_input(delta);
    update_billboards();
    scene.update();

    renderer.render(scene);
}

void Demo::destroy()
{
    window->set_pointer_lock(false);
}

void Demo::create_scene()
{
    scene = gst::Scene::create_perspective({ 45.0f, render_size, 0.1f, 4000.0f });
    scene.get_eye().position = glm::vec3(36.0f, 52.0f, 88.0f);
}

void Demo::create_billboards()
{
    gst::MeshFactory mesh_factory(logger);
    gst::ImageFactory image_factory(logger);

    auto image = image_factory.create_from_file(PEPPER_PNG, true);
    auto texture = std::make_shared<gst::Texture2D>(gst::Texture2D::create_from_image(image));

    auto program = programs.create(CHEAP_VS, BASIC_FS);
    auto pass = std::make_shared<gst::BasicPass>(program);
    pass->set_blend_mode(gst::BlendMode::INTERPOLATIVE);
    pass->set_depth_test(true);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::piecewise_linear_distribution<> d;

    auto step = 8.0f;
    auto points = 10;
    for (auto x = 0; x < points; x++) {
        for (auto y = 0; y < points; y++) {
            for (auto z = 0; z < points; z++) {
                const auto unit = 0;
                auto material = gst::Material::create_free();
                material.get_textures()[unit] = texture;
                material.get_uniform("map") = unit;
                material.get_uniform("diffuse") = glm::vec4(d(gen), d(gen), d(gen), 1.0f) + 0.1f;
                material.get_uniform("scale") = glm::vec3(1.0f);

                auto mesh = mesh_factory.create_quad(1.0f, 1.0f);
                auto model = gst::Model(mesh, material, pass);
                auto billboard = std::make_shared<Billboard>(model);
                billboard->position = glm::vec3(x, y, z) * step;

                billboards.push_back(billboard);
                scene.add(billboard);
            }
        }
    }
}

void Demo::update_input(float delta)
{
    auto input = window->get_input();

    if (input.pressed(gst::Key::F1)) {
        billboarding = !billboarding;
        std::string status = billboarding ? "on" : "off";
        logger->log("billboarding: " + status);
    }

    if (input.pressed(gst::Key::F2)) {
        spherical = !spherical;
        std::string status = spherical ? "spherical" : "cylindrical";
        logger->log("type: " + status);
    }

    if (input.pressed(gst::Key::F3)) {
        cheap_method = !cheap_method;
        std::string status = cheap_method ? "cheap" : "true";
        logger->log("method: " + status);
    }

    controls.update(delta, input, scene.get_eye());
}

void Demo::update_billboards()
{
    for (auto billboard : billboards) {
        auto & material = billboard->get_material();
        material.get_uniform("billboarding") = billboarding;
        material.get_uniform("spherical") = spherical;
        material.get_uniform("cheap_method") = cheap_method;

        if (billboarding && !cheap_method) {
            update_true(*billboard.get());
        } else {
            // keep resetting the billboard orientations (for true method)
            billboard->orientation = glm::quat();
        }
    }
}

void Demo::update_true(Billboard & billboard)
{
    // cylindrical
    glm::vec3 direction_proj = scene.get_eye().position - billboard.position;
    direction_proj.y = 0.0f;
    direction_proj = glm::normalize(direction_proj);
    billboard.orientation = rotation_between(Z_UNIT, direction_proj);

    if (spherical) {
        glm::vec3 direction = scene.get_eye().position - billboard.position;
        direction = glm::normalize(direction);
        billboard.orientation = rotation_between(direction_proj, direction) * billboard.orientation;
    }
}
