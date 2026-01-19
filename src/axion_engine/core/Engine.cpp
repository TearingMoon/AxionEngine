#include "Engine.hpp"

namespace Axion
{

Engine::Engine(EngineConfig config, WindowConfig windowConfig) 
    : sdlManager_(context_)
    , window_(context_)
{
    config_ = config;

    logger_ = std::make_unique<Logger>(config_.maxLogFiles);
    context_.logger = logger_.get();

    analyzer_ = std::make_unique<Analyzer>(context_);
    context_.analyzer = analyzer_.get();

    events_ = std::make_unique<EventBus>();
    context_.events = events_.get();

    sdlManager_.InitSDL();

    window_.Start(windowConfig);
    context_.window = &window_;

    logger_->Separator("Initializing Engine Managers");

    time_ = std::make_unique<TimeManager>(context_);
    context_.time = time_.get();

    input_ = std::make_unique<InputManager>(context_);
    context_.input = input_.get();

    scene_ = std::make_unique<SceneManager>(context_);
    context_.scene = scene_.get();

    physics_ = std::make_unique<PhysicsManager>(context_);
    context_.physics = physics_.get();

    render_ = std::make_unique<RenderManager>(context_);
    context_.render = render_.get();

    assets_ = std::make_unique<AssetsManager>(context_);
    context_.assets = assets_.get();

    logger_->Separator("Engine Managers Initialized");
}

Engine::~Engine()
{
    sdlManager_.QuitSDL();
}

void Engine::Run()
{
    state_ = EngineState::Running;

    while (state_ == EngineState::Running)
    {
        EventLoop();
        AppLoop();
        SDL_Delay(config_.updateDelay);
    }
}

void Engine::EventLoop()
{
    input_->BeginFrame();
    while (SDL_PollEvent(&event_))
    {
        input_->ProcessEvent(event_);

        if (event_.type == SDL_QUIT)
        {
            logger_->Info("Quit event received. Stopping engine.");
            state_ = EngineState::Stopped;
        }
    }
    input_->EndFrame();
}

void Engine::AppLoop()
{
    time_->Update();
    analyzer_->Update();
    
    // 1. Process pending spawn/destroy from previous frame
    scene_->ProcessRequests();
    
    // 2. Fixed timestep physics update
    physics_->Update(time_->GetDeltaTime());
    
    // 3. Game logic update
    scene_->Update();
    
    // 4. Render
    render_->Update();
}

} // namespace Axion
