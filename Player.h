#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <map>
#include <string>
#include <vector>
#include "Structs.h"    
#include "Animation.h"
#include "Interface.h"
#include "Camera.h"
#include "Skill.h"
#include "SkillHUD.h"
#include "Inventory.h"

class Enemy;

class Player {
public:
    Player(SDL_Renderer* renderer, TTF_Font* font, Camera* camera);
    ~Player();

    void setEnemies(const std::vector<Enemy*>& enemiesList);
    void obrabotkaklavish(SDL_Event* event);
    void obnovleniepersa();
    void otrisovka();
    bool checkCollisionForRect(const SDL_FRect& rect) const;
    void setPosition(float x, float y);
    void setCollisions(const std::vector<SDL_FRect>& rects);

    SDL_FRect getDest() const { return dest; }

    void addMoney(int addedMoney);

    bool getIsSkillActive() const { return isSkillActive; }
    bool isFlipped() const { return flip == SDL_FLIP_HORIZONTAL; }
    bool checkCollision(const SDL_FRect& a, const SDL_FRect& b) const;


    void setSkillActive(bool active);
    void setLastDashTime(Uint64 t);
    Uint64 getLastDashTime() const;

    int getDirection() const { return isFlipped() ? -1 : 1; }
    int getCurrentAttackFrame() const { return animationHandler.getCurrentFrame(); }

    void updateInventory();
    void renderInventory();

    const std::vector<Skill*>& getSkills() const {
        return skills;
    }

    bool getIsAttack() const;
    SDL_FRect getAttackHitbox() const;

    void setAnimation(const std::string& animName, bool loop);
    SDL_FRect getRect() const;

    void takeDamage(int amount);

    bool isDead() const { return currentHealth <= 0; }
    int getHealth() const { return currentHealth; }
    void setDest(const SDL_FRect& d);
    void setMapWidth(float width) { mapWidth = width; }

    bool isInventoryOpen() const { return inventoryOpen; }
    Inventory* getInventory() const { return inventory; }

private:

    // В private секции
    Uint64 lastDamageTime = 0;
    Uint64 lastHealTick = 0;
    bool canRegen = false;


    bool inventoryTogglePressed = false;
    bool inventoryOpen = false;
    Inventory* inventory;

    std::map<std::string, AnimationSet> animations;
    std::string currentAnim = "idle";
    std::vector<Enemy*> enemies;
    std::vector<Skill*> skills;
    std::vector<SDL_FRect> collisionRects;

    void updateHitbox();
    void defineLook(const bool* keys);
    void attackHandler();
    void moveHandler(const bool* keys);
    void initAnimations();

    SDL_Renderer* renderer;
    TTF_Font* font;
    Interface* interface;
    Camera* camera;

    float mapWidth = 50000.0f;

    SkillHUD* skillHUD;

    SDL_Texture* dashIconTexture = nullptr;

    Animation animationHandler;

    SDL_FRect rect;
    SDL_FRect src;
    SDL_FRect dest;
    SDL_FlipMode flip;

    int previousFrame = -1;
    int speed;
    int currentHealth;
    int TotalHealth;
    int money = 0;

    float oldX = 0.0f;
    bool isOnGround = false;
    float velocityY = 0.0f;
    float gravity = 1.0f;
    float sila_prizhka = -10.0f;

    bool currentLoop = true;
    bool isWalk = false;
    bool isAttack = false;
    bool isjump = false;
    bool isRunning = false;
    bool damageDone = false;
    bool hasDealtDamage = false;
    SDL_FRect hitbox;

    bool isSkillActive = false;

    Uint64 lastDashTime = 0;
    const Uint64 dashCooldown = 2000;

    Uint32 lastAttackTime = 0;
    const Uint32 attackCooldown = 300;
};
