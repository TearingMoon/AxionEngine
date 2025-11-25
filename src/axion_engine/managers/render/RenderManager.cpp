#include "RenderManager.hpp"

RenderManager::RenderManager(EngineContext &ctx) : ContextAware(ctx)
{
    printf("- RenderManager initialized. \n");
}

RenderManager::~RenderManager()
{
    printf("- RenderManager destroyed. \n");
}

void RenderManager::Update()
{
    
}
