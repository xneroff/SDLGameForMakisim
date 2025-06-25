#include <SDL3_image/SDL_image.h>
#include <vector>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>
#include "DashSkill.h"      
#include "FireballSkill.h"
#include "Enemy.h"
#include <iostream>
#include <fstream>
#include "Game.h"
#include "TileMap.h"
#include "Player.h"

Player::Player(SDL_Renderer* renderer, TTF_Font* font, Camera* camera, Game* game)
    : renderer(renderer), font(font), camera(camera), game(game),
    inventoryOpen(false), inventory(new Inventory(renderer)),
    dashIconTexture(nullptr), animationHandler(),
    speed(10), currentHealth(100), TotalHealth(100),
    money(0), flip(SDL_FLIP_NONE),
    oldX(0), velocityY(0), gravity(1.0f), sila_prizhka(-15.0f),
    currentLoop(true), isWalk(false), isAttack(false),
    isjump(false), isRunning(false), hasDealtDamage(false),
    isSkillActive(false), lastDashTime(0), lastAttackTime(0)
{
    initAnimations();
    src = { 0, 0, 48, 48 };
    dest = { 400.0f, 300.0f, 64.0f, 64.0f };
    rect = dest;

    interface = new Interface(renderer, font, currentHealth, TotalHealth);
    skillHUD = new SkillHUD(renderer);

    inventory->addItem("Topor", "assets/MoiInventory/Topor.png");
    inventory->addItem("eda", "assets/MoiInventory/eda.png");

}

Player::~Player() {
    delete interface;
    delete skillHUD;
    for (auto s : skills) delete s;
    delete inventory;
    if (dashIconTexture) SDL_DestroyTexture(dashIconTexture);
    
}

void Player::initAnimations() {
    SDL_Texture* tex = nullptr;

    tex = IMG_LoadTexture(renderer, "assets/1 Woodcutter/Woodcutter_idle.png");
    if (tex) {
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
        animations["idle"] = { tex, 4, 200 };
    }

    tex = IMG_LoadTexture(renderer, "assets/1 Woodcutter/Woodcutter_walk.png");
    if (tex) {
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
        animations["walk"] = { tex, 6, 100 };
    }

    tex = IMG_LoadTexture(renderer, "assets/1 Woodcutter/Woodcutter_run.png");
    if (tex) {
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
        animations["run"] = { tex, 6, 100 };
    }

    tex = IMG_LoadTexture(renderer, "assets/1 Woodcutter/Woodcutter_attack2.png");
    if (tex) {
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
        animations["attack"] = { tex, 6, 150 };
    }

    tex = IMG_LoadTexture(renderer, "assets/1 Woodcutter/Woodcutter_jump.png");
    if (tex) {
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
        animations["jump"] = { tex, 6, 150 };
    }
}

void Player::otrisovka() {
    const AnimationSet& anim = animations[currentAnim];
    SDL_FRect screenDest = camera->apply(dest);
    SDL_RenderTextureRotated(renderer, anim.texture, &src, &screenDest, 0, nullptr, flip);
    interface->otrisovka();
    skillHUD->render();

    static Uint64 lastTime = SDL_GetTicks();
    Uint64 now = SDL_GetTicks();
    Uint64 deltaTimeMs = now - lastTime;
    lastTime = now;
    float deltaTime = deltaTimeMs / 1000.0f;

    for (Skill* skill : skills) {
        skill->render(renderer, camera);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 120);
    SDL_FRect screenHitbox = camera->apply(hitbox);
    SDL_RenderRect(renderer, &screenHitbox);
}


bool Player::checkCollisionForRect(const SDL_FRect& rect) const {
    for (const auto& obstacle : collisionRects) {
        if (checkCollision(rect, obstacle)) {
            return true;
        }
    }
    return false;
}

bool Player::checkCollision(const SDL_FRect& a, const SDL_FRect& b) const {
    float rightA = a.x + a.w;
    float rightB = b.x + b.w;
    float bottomA = a.y + a.h;
    float bottomB = b.y + b.h;

    bool collidingX = (a.x < rightB) && (rightA > b.x);
    bool collidingY = (a.y < bottomB) && (bottomA > b.y);

    return collidingX && collidingY;
}


void Player::setEnemies(const std::vector<Enemy*>& enemiesList) {
    enemies = enemiesList;
}

void Player::renderInventory() {
    if (inventoryOpen && inventory) {
        inventory->render();
    }
}

void Player::setAnimation(const std::string& animName, bool loop) {
    if (currentAnim != animName) {
        currentAnim = animName;
        animationHandler.reset();
        src.x = 0;
        src.y = 0;
        currentLoop = loop;
    }
}

SDL_FRect Player::getRect() const {
    return rect;
}

void Player::addMoney(int addedMoney) {
    money += addedMoney;
}

void Player::updateHitbox() {
    hitbox.w = dest.w - 50;
    hitbox.h = dest.h - 25;
    hitbox.y = dest.y + 24;

    hitbox.x = (flip == SDL_FLIP_HORIZONTAL) ? dest.x + 35 : dest.x + 15;
}

void Player::defineLook(const bool* keys) {
    if (isAttack) return;

    SDL_FlipMode prevFlip = flip;

    if (keys[SDL_SCANCODE_A]) {
        flip = SDL_FLIP_HORIZONTAL;
    }
    else if (keys[SDL_SCANCODE_D]) {
        flip = SDL_FLIP_NONE;
    }

    if (prevFlip != flip) {
        dest.x += (flip == SDL_FLIP_HORIZONTAL) ? -20 : 20;
    }
}



void Player::attackHandler() {
    if (!isAttack) return;

    int currentFrame = animationHandler.getCurrentFrame();


    if (currentFrame >= 4 && !hasDealtDamage) {
        SDL_FRect atkBox = getAttackHitbox();
        for (Enemy* enemy : enemies) {
            if (!enemy) continue;
            SDL_FRect enemyBox = enemy->getHitbox();
            if (checkCollision(atkBox, enemyBox)) {
                std::cout << "HIT DETECTED" << std::endl;
                enemy->takeDamage(10);
                hasDealtDamage = true;
                break;
            }
        }
    }

    if (animationHandler.isFinished()) {
        isAttack = false;
        hasDealtDamage = false; 
        currentAnim = "idle";
        animationHandler.reset();
    }
}

void Player::updateInventory() {
    if (inventoryOpen && inventory) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            inventory->handleEvent(&e);
        }
    }
}



void Player::moveHandler(const bool* keys) {
    isWalk = false;
    isRunning = false;
    defineLook(keys);

    int actualSpeed = keys[SDL_SCANCODE_LSHIFT] ? speed * 2 : speed;
    isRunning = keys[SDL_SCANCODE_LSHIFT];

    bool moveLeft = keys[SDL_SCANCODE_A];
    bool moveRight = keys[SDL_SCANCODE_D];

    float oldX = dest.x;
    float oldY = dest.y;

    // --- Горизонтальное движение
    if (moveLeft && !moveRight) {
        dest.x -= actualSpeed;
        flip = SDL_FLIP_HORIZONTAL;
        isWalk = true;
    }
    else if (moveRight && !moveLeft) {
        dest.x += actualSpeed;
        flip = SDL_FLIP_NONE;
        isWalk = true;
    }

    updateHitbox();
    // Проверка столкновений по X
    for (const auto& rect : collisionRects) {
        if (SDL_HasRectIntersectionFloat(&hitbox, &rect)) {
            dest.x = oldX; // Откат назад
            updateHitbox();
            break;
        }
    }

    // --- Прыжок
    if (keys[SDL_SCANCODE_SPACE] && isOnGround) {
        velocityY = sila_prizhka;
        isjump = true;
    }

    // --- Гравитация
    velocityY += gravity;
    dest.y += velocityY;
    updateHitbox();

    isOnGround = false;

    // Проверка столкновений по Y
    for (const auto& rect : collisionRects) {
        if (SDL_HasRectIntersectionFloat(&hitbox, &rect)) {
            if (velocityY > 0 && hitbox.y + hitbox.h - velocityY <= rect.y) {
                dest.y = rect.y - dest.h;
                isOnGround = true;
                isjump = false;
                velocityY = 0;
            }
            else if (velocityY < 0 && hitbox.y - velocityY >= rect.y + rect.h) {
                dest.y = rect.y + rect.h;
                velocityY = 0;
            }
            updateHitbox();
            break;
        }
    }

    // --- Анимации
    if (!isAttack) {
        std::string prevAnim = currentAnim;

        if (isjump) {
            currentAnim = "jump";
        }
        else if (isWalk) {
            currentAnim = isRunning ? "run" : "walk";
        }
        else {
            currentAnim = "idle";
        }

        // Только если анимация изменилась — сбрасываем и обнуляем src
        if (prevAnim != currentAnim) {
            animationHandler.reset();
            src.x = 0;
            src.y = 0;
        }

        animationHandler.update(animations[currentAnim], src, 48);
    }
}




void Player::setPosition(float x, float y) {
    dest.x = x;
    dest.y = y - dest.h; // Смещаем вниз, чтобы не оказаться ВНУТРИ блока
    updateHitbox();
}


        void Player::setCollisions(const std::vector<SDL_FRect>& rects) {
            collisionRects = rects;
        }


        // Возвращает true, если игрок сейчас атакует
        bool Player::getIsAttack() const {
            return isAttack;
        }

        SDL_FRect Player::getAttackHitbox() const {
            float attackWidth = 110.0f;  // Расширяем ещё больше (с 90 → 110)
            float attackHeight = dest.h * 1.2f; // Делаем выше (с 1.1 → 1.2)
            float attackX;

            if (flip == SDL_FLIP_NONE) {
                attackX = dest.x + dest.w;
            }
            else {
                attackX = dest.x - attackWidth;
            }

            float attackY = dest.y;

            return SDL_FRect{ attackX, attackY, attackWidth, attackHeight };
        }


            void Player::obnovleniepersa() {
                static Uint64 lastTime = SDL_GetTicks();

                Uint64 now = SDL_GetTicks();
                Uint64 deltaTimeMs = now - lastTime;
                lastTime = now;

                float deltaTime = deltaTimeMs / 1000.0f;

                const bool* keys = SDL_GetKeyboardState(nullptr);

                // Всегда вызываем движение
                moveHandler(keys);

                interface->obnovlenieHUD(deltaTime);


                const AnimationSet& currentAnimation = animations[currentAnim];

                // loop=false для анимации атаки, чтобы она дошла до конца
                bool loopAnim = (currentAnim != "attack");

                animationHandler.update(currentAnimation, src, 48 /* ширина кадра для ваших спрайтов */, loopAnim);


                attackHandler();

                for (Skill* skill : skills) {
                    skill->update(this, deltaTime); // FireballSkill использует checkCollisionForRect(player)
                }
                rect = dest;  // чтобы getRect() возвращал актуальные координаты

                Uint64 nowMs = SDL_GetTicks();

                // Если прошло 7 секунд без урона — разрешить регенерацию
                if (!canRegen && nowMs - lastDamageTime >= 7000) {
                    canRegen = true;
                    lastHealTick = nowMs; // сбрасываем таймер лечения
                }

                // Если регенерация включена и прошло 5 секунд с прошлого восстановления
                if (canRegen && nowMs - lastHealTick >= 5000) {
                    if (currentHealth < TotalHealth) {
                        currentHealth += 5;
                        if (currentHealth > TotalHealth) currentHealth = TotalHealth;
                        interface->setHealth(currentHealth);
                    }
                    lastHealTick = nowMs; // обновляем таймер   
                }
                // сохраняем последний портал, в который зашёл игрок
                game->lastPortalInRange = nullptr;
                for (const Portal& p : game->getTileMap()->getPortals()) {
                    SDL_FRect playerBox = getRect();
                    if (SDL_HasRectIntersectionFloat(&playerBox, &p.rect)) {
                        game->lastPortalInRange = &p;
                        break;
                    }
                }

            }

        void Player::takeDamage(int amount) {
            if (currentHealth <= 0) return;
            currentHealth -= amount;
            interface->setHealth(currentHealth);
            if (currentHealth < 0) currentHealth = 0;

            lastDamageTime = SDL_GetTicks(); // <<< Запоминаем время урона
            canRegen = false;                // <<< Останавливаем реген
        }


        void Player::setDest(const SDL_FRect& d)
        {
            dest = d;
            updateHitbox();
        }




        void Player::setSkillActive(bool active) {
            isSkillActive = active;
        }

        void Player::setLastDashTime(Uint64 t) {
            lastDashTime = t;
        }

        Uint64 Player::getLastDashTime() const {
            return lastDashTime;
        }

        void Player::obrabotkaklavish(SDL_Event* event) {
            // --- ЛКМ для атаки (если инвентарь закрыт)
            if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_LEFT) {
                if (!inventoryOpen && !isAttack) {
                    currentAnim = "attack";
                    animationHandler.reset();
                    isAttack = true;
                    hasDealtDamage = false; // <<< ВАЖНО!
                }
            }

            // --- Не сбрасывай isAttack на MouseButtonUp!
            // Атака завершится в attackHandler() когда animationHandler.isFinished()

            // --- Инвентарь (клавиша I)
            if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_I) {
                if (!inventoryTogglePressed) {
                    inventoryOpen = !inventoryOpen;
                    inventoryTogglePressed = true;
                    std::cout << " Inventory status: " << (inventoryOpen ? "OPEN" : "CLOSED") << std::endl;
                }
            }

            if (event->type == SDL_EVENT_KEY_UP && event->key.key == SDLK_I) {
                inventoryTogglePressed = false;
            }

            // --- Навыки Q / E
            if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_Q) {
                if (!skills.empty()) {
                    skills[0]->activate(this);  // DashSkill
                }
            }
            if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_1) {
                if (skills.size() > 1) {
                    skills[1]->activate(this);  // FireballSkill
                }
            }

            // --- События для инвентаря
            if (inventoryOpen) {
                inventory->handleEvent(event);
            }
        }


