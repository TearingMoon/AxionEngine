#pragma once

#include "axion_engine/Axion.hpp"
#include <string>

namespace Axion
{

class GameUIScript : public ScriptableComponent
{
public:
    TextRenderComponent *scoreText = nullptr;
    TextRenderComponent *healthText = nullptr;
    TextRenderComponent *roundText = nullptr;
    TextRenderComponent *gameOverText = nullptr;
    
    int score = 0;
    int health = 100;
    int currentRound = 1;
    bool isGameOver = false;
    
    void Start(EngineContext &context) override
    {
        UpdateUI();
    }
    
    void Update(EngineContext &context) override
    {
        auto *owner = GetOwner();
        if (!owner || owner->IsDestroyed()) return;
        
        // Follow camera position for UI
        auto *scene = context.sceneProvider->GetCurrentScene();
        if (!scene) return;
        
        auto *camera = scene->GetCurrentCamera();
        if (!camera) return;
        
        auto *camOwner = camera->GetOwner();
        if (!camOwner || camOwner->IsDestroyed()) return;
        
        auto *camTr = camOwner->GetTransform();
        if (!camTr) return;
        
        glm::vec3 camPos = camTr->GetPosition();
        
        // Position UI elements relative to camera (top-left corner)
        if (scoreText && scoreText->GetOwner())
        {
            scoreText->GetOwner()->GetTransform()->SetPosition({camPos.x - 350.0f, camPos.y + 350.0f, 0.9f});
        }
        if (healthText && healthText->GetOwner())
        {
            healthText->GetOwner()->GetTransform()->SetPosition({camPos.x - 350.0f, camPos.y + 310.0f, 0.9f});
        }
        if (roundText && roundText->GetOwner())
        {
            roundText->GetOwner()->GetTransform()->SetPosition({camPos.x + 350.0f, camPos.y + 350.0f, 0.9f});
        }
        if (gameOverText && gameOverText->GetOwner())
        {
            gameOverText->GetOwner()->GetTransform()->SetPosition({camPos.x, camPos.y, 0.95f});
        }
    }
    
    void AddScore(int points)
    {
        score += points;
        UpdateUI();
    }
    
    void SetHealth(int newHealth)
    {
        health = newHealth;
        UpdateUI();
    }
    
    void SetRound(int round)
    {
        currentRound = round;
        UpdateUI();
    }
    
    void ShowGameOver()
    {
        isGameOver = true;
        if (gameOverText)
        {
            gameOverText->SetText("GAME OVER - Press R to Restart");
            gameOverText->SetColor({255, 0, 0, 255});
        }
    }
    
    void HideGameOver()
    {
        isGameOver = false;
        if (gameOverText)
        {
            gameOverText->SetText("");
        }
    }
    
private:
    void UpdateUI()
    {
        if (scoreText)
        {
            scoreText->SetText("Score: " + std::to_string(score));
        }
        if (healthText)
        {
            healthText->SetText("Health: " + std::to_string(health));
        }
        if (roundText)
        {
            roundText->SetText("Round: " + std::to_string(currentRound));
        }
    }
};

} // namespace Axion
