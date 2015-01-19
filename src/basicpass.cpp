#include "basicpass.hpp"

#include "program.hpp"

void BasicPass::apply(gst::MatrixState & matrices, gst::LightNodes &)
{
    uniforms("model_view") = matrices.model_view;
    uniforms("projection") = matrices.projection;
    program->update(uniforms);
}
