#include "AssetsManager.hpp"

AssetsManager::AssetsManager(EngineContext &ctx) : ContextAware(ctx)
{
    printf("- AssetsManager initialized.\n");
}

AssetsManager::~AssetsManager()
{
    printf("- AssetsManager destroyed. \n");
}
