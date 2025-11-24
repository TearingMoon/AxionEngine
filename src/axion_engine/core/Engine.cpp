#include "Engine.hpp"

// TODO: Remove window initialization from here
Engine::Engine(EngineConfig config, WindowConfig WindowConfig) : sdlManager_(), window_(WindowConfig)
{
    config_ = config;
    // Initialize SDL
    sdlManager_.InitSDL();

    // Initialize Managers
    printf("Initializing Engine Managers...\n");
    time_ = std::make_unique<TimeManager>();
    context_.time = time_.get();

    input_ = std::make_unique<InputManager>();
    context_.input = input_.get();

    scene_ = std::make_unique<SceneManager>(context_);
    context_.scene = scene_.get();

    physics_ = std::make_unique<PhysicsManager>(context_);
    context_.physics = physics_.get();

    render_ = std::make_unique<RenderManager>(context_);
    context_.render = render_.get();
}

Engine::~Engine()
{
    // Ensure SDL is properly shut down
    sdlManager_.QuitSDL(); // Although SDLManager destructor also does this, being explicit here
}

void Engine::Run()
{
    state_ = ENGINE_STATE_RUNNING;

    // Main engine loop
    while (state_ == ENGINE_STATE_RUNNING)
    {
        // Event handling
        EventLoop();

        // Engine Update Logic
        AppLoop();

        // For now, just delay to simulate work being done
        SDL_Delay(config_.updateDelay);
    }
}

void Engine::EventLoop()
{
    input_->BeginFrame();
    while (SDL_PollEvent(&event_))
    {
        // Process event for InputManager
        input_->ProcessEvent(event_);

        if (event_.type == SDL_QUIT)
        {
            printf("Quit event received. Stopping engine.\n");
            state_ = ENGINE_STATE_STOPPED;
        }
        // Handle other events as needed
    }
    input_->EndFrame();
}

void Engine::AppLoop()
{
    time_->Update();

    scene_->Update();

    // Physics update
    physics_->Update(time_->GetDeltaTime());

    // Graphics update
    render_->Update();

    scene_->ProcessRequests();
}
