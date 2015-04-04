#ifndef DEMO_HPP_INCLUDED
#define DEMO_HPP_INCLUDED

#include "assets.hpp"

#include "gust.hpp"

typedef gst::ModelNode Billboard;

class Demo : public gst::World {
public:
    Demo(std::shared_ptr<gst::Logger> logger, std::shared_ptr<gst::Window> window);
    bool create() final;
    void update(float delta, float elapsed) final;
    void destroy() final;
private:
    void create_scene();
    void create_billboards();
    void update_input(float delta);
    void update_billboards();
    void update_true(Billboard & billboard);

    std::shared_ptr<gst::Logger> logger;
    std::shared_ptr<gst::Window> window;

    gst::Renderer renderer;
    gst::Scene scene;

    gst::Resolution render_size;
    gst::ProgramPool programs;
    gst::FirstPersonControl controls;

    std::vector<std::shared_ptr<Billboard>> billboards;

    bool billboarding;
    bool spherical;
    bool cheap_method;
};

#endif
