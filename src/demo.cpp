#include "demo.hpp"

Demo::Demo(std::shared_ptr<gst::Logger> logger, gst::Window window)
    : logger(logger),
      window(window),
      billboarding(true),
      spherical(false),
      cheap_method(false)
{
}

bool Demo::create()
{
    const auto size = window.get_size();

    render_state = std::make_shared<gst::RenderState>(size);
    renderer = gst::Renderer(logger, render_state);

    auto camera = std::make_shared<gst::PerspectiveCamera>(45.0f, size, 0.1f, 2000.0f);
    scene = gst::Scene(camera);
    scene.eye->translate_z(140.0f);

    gst::ProgramFactory program_factory(logger);
    gst::ProgramPool programs(program_factory);

    create_floor(programs);
    create_billboards(programs);

    controls.movement_speed = 130.0f;
    window.set_pointer_lock(true);

    logger->log("F1: toggle billboarding.");
    logger->log("F2: toggle between spherical and cylindrical billboarding.");
    logger->log("F3: toggle between cheap and true method.");

    return true;
}

void Demo::update(gst::seconds dt, gst::seconds)
{
    update_input(dt);
    update_billboards();
    scene.update();

    renderer.render(scene);
    renderer.check_errors();
}

void Demo::destroy()
{
    window.set_pointer_lock(false);
}

void Demo::create_floor(gst::ProgramPool & programs)
{
    auto basic_pass = std::make_shared<BasicPass>();
    basic_pass->program = programs.create(BASIC_VS, BASIC_FS);
    basic_pass->cull_face = gst::CullFace::BACK;
    basic_pass->depth_test = true;

    auto render_technique = std::make_shared<gst::Technique>();
    render_technique->passes = { basic_pass };

    gst::ImageFactory image_factory(logger);
    auto grass_image = image_factory.create_from_file(GRASS);

    gst::TextureParam tex_param;
    tex_param.source_format = gst::PixelFormat::RGBA;
    auto grass_texture = gst::Texture(render_state, grass_image, tex_param);

    gst::Effect effect;
    effect.uniforms("color_map") = 0;
    effect.uniforms("alpha") = 1.0f;
    effect.textures = { grass_texture };
    effect.techniques = { render_technique };

    gst::MeshFactory mesh_factory(logger, render_state);
    auto mesh = mesh_factory.create_quad(2000.0f, 2000.0f);
    mesh.tex_coords.data = {
        glm::vec2( 0.0f, 80.0f),
        glm::vec2(80.0f, 80.0f),
        glm::vec2( 0.0f,  0.0f),
        glm::vec2(80.0f,  0.0f),
    };
    mesh.tex_coords.buffer_data();

    auto model = std::make_shared<gst::Model>(mesh, effect);
    auto model_node = std::make_shared<gst::ModelNode>(model);
    model_node->rotate_x(-90.0f);
    model_node->position.y = -30.0f;

    scene.add(model_node);
}

void Demo::create_billboards(gst::ProgramPool & programs)
{
    auto basic_pass = std::make_shared<BasicPass>();
    basic_pass->program = programs.create(CHEAP_VS, BASIC_FS);
    basic_pass->blend_mode = gst::BlendMode::INTERPOLATIVE;
    basic_pass->cull_face = gst::CullFace::BACK;
    basic_pass->depth_test = true;

    auto render_technique = std::make_shared<gst::Technique>();
    render_technique->passes = { basic_pass };

    std::vector<glm::vec3> tree_positions = {
        glm::vec3( -50.0f, 3.0f,   50.0f),
        glm::vec3(-300.0f, 0.0f, -800.0f),
        glm::vec3(  30.0f, 0.0f, -420.0f),
        glm::vec3( 120.0f, 8.0f, -200.0f),
    };

    std::vector<std::string> tree_textures = {
        TREE1, TREE2, TREE3, TREE4
    };

    gst::ImageFactory image_factory(logger);
    gst::MeshFactory mesh_factory(logger, render_state);

    gst::TextureParam tex_param;
    tex_param.internal_format = gst::TextureFormat::RGBA;
    tex_param.source_format = gst::PixelFormat::RGBA;
    tex_param.wrap_s = gst::WrapMode::CLAMP_TO_EDGE;
    tex_param.wrap_t = tex_param.wrap_s;

    billboard_effect.uniforms("color_map") = 0;
    billboard_effect.uniforms("alpha") = 1.0f;
    billboard_effect.uniforms("scale") = glm::vec3(0.1f);
    billboard_effect.uniforms("billboarding") = billboarding;
    billboard_effect.uniforms("spherical") = spherical;
    billboard_effect.uniforms("cheap_method") = cheap_method;
    billboard_effect.techniques = { render_technique };

    for (int i = 0; i < 4; i++) {
        auto tree_image = image_factory.create_from_file(tree_textures[i]);
        auto tree_texture = gst::Texture(render_state, tree_image, tex_param);
        billboard_effect.textures = { tree_texture };

        auto quad_size = tree_image.get_size();
        auto mesh = mesh_factory.create_quad(quad_size.get_width(), quad_size.get_height());

        auto model = std::make_shared<gst::Model>(mesh, billboard_effect);
        auto billboard = std::make_shared<Billboard>(model);

        billboard->position = tree_positions[i];
        billboard->scale = glm::vec3(0.1f);

        billboards.push_back(billboard);
        scene.add(billboard);
    }
}

void Demo::update_input(gst::seconds dt)
{
    auto input = window.get_input();

    if (input.pressed(gst::Key::F1)) {
        billboarding = !billboarding;
        billboard_effect.uniforms("billboarding") = billboarding;

        std::string status = billboarding ? "on" : "off";
        logger->log("billboarding: " + status);
    }

    if (input.pressed(gst::Key::F2)) {
        spherical = !spherical;
        billboard_effect.uniforms("spherical") = spherical;

        std::string status = spherical ? "spherical" : "cylindrical";
        logger->log("type: " + status);
    }

    if (input.pressed(gst::Key::F3)) {
        cheap_method = !cheap_method;
        billboard_effect.uniforms("cheap_method") = cheap_method;

        std::string status = cheap_method ? "cheap" : "true";
        logger->log("method: " + status);
    }

    controls.update(dt, input, *scene.eye.get());
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
    glm::vec3 direction_proj = scene.eye->position - billboard.position;
    direction_proj.y = 0.0f;
    direction_proj = glm::normalize(direction_proj);
    billboard.orientation = rotation_between(Z_UNIT, direction_proj);

    if (spherical) {
        glm::vec3 direction = scene.eye->position - billboard.position;
        direction = glm::normalize(direction);
        billboard.orientation = rotation_between(direction_proj, direction) * billboard.orientation;
    }
}
