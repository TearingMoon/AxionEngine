#pragma once

#include "axion_engine/Axion.hpp"
#include <string>

namespace Axion
{

class GameUIScript : public ScriptableComponent
{
public:
    UILabel *scoreText = nullptr;
    UILabel *healthText = nullptr;
    UILabel *roundText = nullptr;
    UILabel *gameOverText = nullptr;
    UIButton *restartButton = nullptr;
    
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
        (void)context;
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
            gameOverText->SetText("GAME OVER");
            gameOverText->SetColor({255, 0, 0, 255});
            gameOverText->SetVisible(true);
        }
        if (restartButton)
            restartButton->SetVisible(true);
    }
    
    void HideGameOver()
    {
        isGameOver = false;
        if (gameOverText)
        {
            gameOverText->SetText("");
            gameOverText->SetVisible(false);
        }
        if (restartButton)
            restartButton->SetVisible(false);
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
