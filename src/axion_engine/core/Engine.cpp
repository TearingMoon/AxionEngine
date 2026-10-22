#include "Engine.hpp"
#include <thread>

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
    context_.timeProvider = time_.get();  // ISP interface

    input_ = std::make_unique<InputManager>(context_);
    context_.input = input_.get();
    context_.inputProvider = input_.get();  // ISP interface

    scene_ = std::make_unique<SceneManager>(context_);
    context_.scene = scene_.get();
    context_.sceneProvider = scene_.get();  // ISP interface

    physics_ = std::make_unique<PhysicsManager>(context_);
    context_.physics = physics_.get();

    render_ = std::make_unique<RenderManager>(context_);
    context_.render = render_.get();

    assets_ = std::make_unique<AssetsManager>(context_);
    context_.assets = assets_.get();
    context_.assetProvider = assets_.get();  // ISP interface

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
        const auto frameStart = std::chrono::steady_clock::now();
        EventLoop();
        AppLoop();
        LimitFrameRate(frameStart);
    }
}

void Engine::LimitFrameRate(std::chrono::steady_clock::time_point frameStart)
{
    int targetFps = config_.maxFps;
    if (targetFps <= 0 && config_.updateDelay > 0)
    {
        targetFps = 1000 / config_.updateDelay;
    }
    if (targetFps <= 0)
        return;

    const auto targetFrame = std::chrono::duration<double>(
        1.0 / static_cast<double>(targetFps));
    const auto deadline = frameStart +
        std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            targetFrame);

    while (std::chrono::steady_clock::now() < deadline)
    {
        const auto remaining = deadline - std::chrono::steady_clock::now();
        if (remaining > std::chrono::milliseconds(2))
        {
            std::this_thread::sleep_for(remaining - std::chrono::milliseconds(1));
        }
        else
        {
            std::this_thread::yield();
        }
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
    analyzer_->BeginFrame();
    
    {
        ScopedTimer timer(analyzer_.get(), "Time");
        time_->Update();
    }
    
    // 1. Process pending spawn/destroy from previous frame
    {
        ScopedTimer timer(analyzer_.get(), "SceneRequests");
        scene_->ProcessRequests();
    }
    
    // 2. Fixed timestep physics update
    {
        ScopedTimer timer(analyzer_.get(), "Physics");
        physics_->Update(time_->GetDeltaTime());
    }
    
    // 3. Game logic update
    {
        ScopedTimer timer(analyzer_.get(), "SceneUpdate");
        scene_->Update();
    }
    
    // 4. Render
    {
        ScopedTimer timer(analyzer_.get(), "Render");
        render_->Update();
    }

    analyzer_->EndFrame();
}

} // namespace Axion
