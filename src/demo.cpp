#include "demo.hpp"

Demo::Demo(std::shared_ptr<gst::Logger> logger, std::shared_ptr<gst::Window> window)
    : logger(logger),
      window(window),
      renderer(gst::Renderer::create(logger)),
      render_size(window->get_size()),
      programs(logger),
      controls(false, 2.5f, 130.0f),
      billboarding(true),
      spherical(false),
      cheap_method(false)
{
}

bool Demo::create()
{
    window->set_pointer_lock(true);

    create_scene();
    create_floor();
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
    scene = gst::Scene::create_perspective({ 45.0f, render_size, 0.1f, 2000.0f });
    scene.get_eye().position.z = 140.0f;
}

void Demo::create_floor()
{
    auto basic_program = programs.create(BASIC_VS, BASIC_FS);
    auto pass = std::make_shared<gst::BasicPass>(basic_program);
    pass->set_cull_face(gst::CullFace::BACK);
    pass->set_depth_test(true);

    gst::ImageFactory image_factory(logger);
    auto image = image_factory.create_from_file(GRASS_PNG);
    auto texture = std::make_shared<gst::Texture2D>(gst::Texture2D::create_from_image(image));

    auto material = gst::Material::create_free();
    const auto unit = 0;
    material.get_textures()[unit] = texture;
    material.get_uniform("color_map") = unit;
    material.get_uniform("alpha") = 1.0f;

    gst::MeshFactory mesh_factory(logger);
    auto mesh = mesh_factory.create_quad(2000.0f, 2000.0f);
    mesh.set_tex_coords({
        glm::vec2( 0.0f, 80.0f),
        glm::vec2(80.0f, 80.0f),
        glm::vec2( 0.0f,  0.0f),
        glm::vec2(80.0f,  0.0f),
    });
    auto model = gst::Model(mesh, material, pass);
    auto model_node = std::make_shared<gst::ModelNode>(model);
    model_node->rotate_x(-90.0f);
    model_node->position.y = -30.0f;

    scene.add(model_node);
}

void Demo::create_billboards()
{
    auto basic_program = programs.create(CHEAP_VS, BASIC_FS);
    auto pass = std::make_shared<gst::BasicPass>(basic_program);
    pass->set_blend_mode(gst::BlendMode::INTERPOLATIVE);
    pass->set_cull_face(gst::CullFace::BACK);
    pass->set_depth_test(true);

    std::vector<glm::vec3> tree_positions = {
        glm::vec3( -50.0f, 3.0f,   50.0f),
        glm::vec3(-300.0f, 0.0f, -800.0f),
        glm::vec3(  30.0f, 0.0f, -420.0f),
        glm::vec3( 120.0f, 8.0f, -200.0f),
    };

    std::vector<std::string> tree_paths = {
        TREE1_PNG, TREE2_PNG, TREE3_PNG, TREE4_PNG
    };

    gst::ImageFactory image_factory(logger);
    std::vector<std::shared_ptr<gst::Texture2D>> tree_textures;

    for (auto path : tree_paths) {
        auto image = image_factory.create_from_file(path);
        auto texture = std::make_shared<gst::Texture2D>(gst::Texture2D::create_from_image(image));
        tree_textures.push_back(texture);
    }

    const auto unit = 0;
    billboard_material = gst::Material::create_free();
    billboard_material.get_uniform("color_map") = unit;
    billboard_material.get_uniform("alpha") = 1.0f;
    billboard_material.get_uniform("scale") = glm::vec3(0.1f);
    billboard_material.get_uniform("billboarding") = billboarding;
    billboard_material.get_uniform("spherical") = spherical;
    billboard_material.get_uniform("cheap_method") = cheap_method;

    gst::MeshFactory mesh_factory(logger);
    for (int i = 0; i < 4; i++) {
        auto texture = tree_textures[i];
        billboard_material.get_textures()[unit] = texture;

        auto size = texture->get_size();
        auto mesh = mesh_factory.create_quad(size.get_width(), size.get_height());
        auto model = gst::Model(mesh, billboard_material, pass);
        auto billboard = std::make_shared<Billboard>(model);
        billboard->position = tree_positions[i];
        billboard->scale = glm::vec3(0.1f);

        billboards.push_back(billboard);
        scene.add(billboard);
    }
}

void Demo::update_input(float delta)
{
    auto input = window->get_input();

    if (input.pressed(gst::Key::F1)) {
        billboarding = !billboarding;
        billboard_material.get_uniform("billboarding") = billboarding;

        std::string status = billboarding ? "on" : "off";
        logger->log("billboarding: " + status);
    }

    if (input.pressed(gst::Key::F2)) {
        spherical = !spherical;
        billboard_material.get_uniform("spherical") = spherical;

        std::string status = spherical ? "spherical" : "cylindrical";
        logger->log("type: " + status);
    }

    if (input.pressed(gst::Key::F3)) {
        cheap_method = !cheap_method;
        billboard_material.get_uniform("cheap_method") = cheap_method;

        std::string status = cheap_method ? "cheap" : "true";
        logger->log("method: " + status);
    }

    controls.update(delta, input, scene.get_eye());
}

void Demo::update_billboards()
{
    for (auto billboard : billboards) {
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
