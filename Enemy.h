#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <unordered_map>
#include <vector>
#include <string>
#include "Camera.h"
#include "Player.h"

enum class EnemyType {
    Default,
    GraveRobber
};

class Enemy {
public:
    Enemy(SDL_Renderer* renderer, float x, float y, EnemyType type);
    ~Enemy();

    void takeDamage(int amount);
    void render(SDL_Renderer* renderer, Camera* camera);
    void update(float deltaTime, Player* player);

    void initRectSize();
    void setCollisionRects(const std::vector<SDL_FRect>& rects);
    SDL_FRect getRect() const;
    SDL_FRect getHitbox() const;
    bool isMarkedForDeletion() const;
    float getAggroRadius() const { return aggroRadius; }
    void setPosition(float x, float y);
    void setAggroState(bool state);
    bool isDeadNow() const { return isDead; }
    SDL_FRect getAttackHitbox() const;
    // Enemy.h (добавляем публичные методы)
    void setGroundRects(const std::vector<SDL_FRect>& rects);
    void setObstacleRects(const std::vector<SDL_FRect>& rects);
    bool isDead = false;

private:
    SDL_FRect hitbox;
    std::vector<SDL_FRect> groundRects;    // для земли (стопы)
    std::vector<SDL_FRect> obstacleRects;  // д

    const float maxJumpHeight = 30.0f; // например, враг может перепрыгнуть препятствие высотой до 30 пикселей
    const float jumpVelocity = -10.0f; // скорость прыжка (отрицательная, чтобы подниматься вверх)

    void setAnimation(const std::string& anim);
    float attackRange = 50.0f;  // Минимальное расстояние для атаки

    bool markedForDeletion = false;

    enum class EnemyState { Idle, Suspicious, Aggro, Returning, Dead };
    EnemyState state = EnemyState::Idle;

    EnemyType type = EnemyType::Default;

    SDL_Renderer* renderer;
    SDL_FRect rect;
    SDL_FPoint spawnPoint;

    std::vector<SDL_FRect> collisionRects;

    std::unordered_map<std::string, SDL_Texture*> textures;
    std::unordered_map<std::string, int> frameCounts = {
        {"idle", 4}, {"walk", 6}, {"attack", 6}, {"hurt", 2}, {"death", 6}
    };

    int currentFrame = 0;
    int totalFrames = 1;
    float animationTimer = 0.0f;
    int frameWidth = 64;
    int frameHeight = 64;

    float speed = 70.0f;
    float aggroRadius = 300.0f;
    float suspicionTimer = 0.0f;
    float suspicionThreshold = 0.5f;

    bool facingRight = true;

    std::string currentAnim = "idle";

    int health;
    int maxHealth;
    SDL_FRect attackHitbox; // фиксируется при начале атаки

    float attackCooldown = 1.0f;
    float timeSinceLastAttack = 0.0f;

    float velocityY = 0.0f;
    float gravity = 1.0f;
    bool isOnGround = false;

    float deathTimer = 0.0f;
    float deathDuration = 1.5f;
    bool hasDealtDamageInThisAttack = false;

    bool isHurt = false;
    float hurtTimer = 0.0f;
    float hurtDuration = 0.5f;

    bool isAttacking = false;
    float attackAnimTimer = 0.0f;
    float attackAnimDuration = 0.6f;

    static std::vector<Enemy*> allEnemies;

    float deathAlpha = 255.0f;
};