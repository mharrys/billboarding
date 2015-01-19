#ifndef BASICPASS_HPP_INCLUDED
#define BASICPASS_HPP_INCLUDED

#include "pass.hpp"
#include "uniformgroup.hpp"

class BasicPass : public gst::Pass {
public:
    void apply(gst::MatrixState & matrices, gst::LightNodes & light_nodes) override;
private:
    gst::UniformGroup uniforms;
};

#endif
