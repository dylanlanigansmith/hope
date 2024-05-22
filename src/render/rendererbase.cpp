#include "rendererbase.hpp"
#include "web.hpp"

bool RendererBase::mobile()
{    
    return plat_mobile;
}

void RendererBase::init_platform()
{
    plat_mobile = is_mobile();
}
