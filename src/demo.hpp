#ifndef DEMO_HPP_INCLUDED
#define DEMO_HPP_INCLUDED

#include "assets.hpp"
#include "basicpass.hpp"
#include "cameranode.hpp"
#include "effect.hpp"
#include "firstpersoncontrol.hpp"
#include "image.hpp"
#include "imagefactory.hpp"
#include "input.hpp"
#include "light.hpp"
#include "lightnode.hpp"
#include "logger.hpp"
#include "mesh.hpp"
#include "meshfactory.hpp"
#include "model.hpp"
#include "modelnode.hpp"
#include "pass.hpp"
#include "perspectivecamera.hpp"
#include "programfactory.hpp"
#include "programpool.hpp"
#include "renderer.hpp"
#include "renderstate.hpp"
#include "scene.hpp"
#include "technique.hpp"
#include "uniformgroup.hpp"
#include "window.hpp"
#include "world.hpp"

typedef gst::ModelNode Billboard;

class Demo : public gst::World {
public:
    Demo(std::shared_ptr<gst::Logger> logger, gst::Window window);
    bool create() override;
    void update(gst::seconds delta, gst::seconds elapsed) override;
    void destroy() override;
private:
    void create_floor(gst::ProgramPool & programs);
    void create_billboards(gst::ProgramPool & programs);
    void update_input(gst::seconds dt);
    void update_billboards();
    void update_true(Billboard & billboard);

    std::shared_ptr<gst::Logger> logger;
    gst::Window window;

    std::shared_ptr<gst::RenderState> render_state;
    gst::Renderer renderer;
    gst::Scene scene;
    gst::FirstPersonControl controls;

    gst::Effect billboard_effect;
    std::vector<std::shared_ptr<Billboard>> billboards;

    bool billboarding;
    bool spherical;
    bool cheap_method;
};

#endif
