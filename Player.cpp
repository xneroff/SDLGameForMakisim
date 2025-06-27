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

Player::Player(SDL_Renderer * renderer, TTF_Font * font, Camera * camera, Game * game)
    : renderer(renderer), font(font), camera(camera), game(game),
    inventoryOpen(false), inventory(new Inventory(renderer)),
    dashIconTexture(nullptr), animationHandler(),
    speed(10), currentHealth(100), TotalHealth(100),
    money(0), flip(SDL_FLIP_NONE),
    oldX(0), velocityY(0), gravity(1.0f), sila_prizhka(-15.0f),
    currentLoop(true), isWalk(false), isAttack(false),
    isjump(false), isRunning(false), hasDealtDamage(false),
    isSkillActive(false), lastDashTime(0), lastAttackTime(0) {

    initAnimations();
    src = { 0, 0, 48, 48 };
    dest = { 400.0f, 300.0f, 64.0f, 64.0f };
    rect = dest;

    interface = new Interface(renderer, font, currentHealth, TotalHealth);
    skillHUD = new SkillHUD(renderer);

    inventory->addItem("Topor", "assets/MoiInventory/Topor.png");
    inventory->addItem("eda", "assets/MoiInventory/eda.png");

    skills.push_back(new DashSkill());
    skills.push_back(new FireballSkill());

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
    const std::vector<std::tuple<std::string, std::string, int>> anims = {
        {"idle",   "Woodcutter_idle.png",   4},
        {"walk",   "Woodcutter_walk.png",   6},
        {"run",    "Woodcutter_run.png",    6},
        {"attack", "Woodcutter_attack2.png",6},
        {"jump",   "Woodcutter_jump.png",   6}
    };

    for (const auto& anim : anims) {
        std::string name = std::get<0>(anim);
        std::string file = std::get<1>(anim);
        int frames = std::get<2>(anim);

        SDL_Texture* tex = IMG_LoadTexture(renderer, ("assets/1 Woodcutter/" + file).c_str());
        if (tex) {
            SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
            animations[name] = { tex, frames, 150 };
        }
    }

}

void Player::otrisovka() {
    const AnimationSet& anim = animations[currentAnim];

    SDL_FRect screenDest = camera->apply(dest);  // ← исправлено
    SDL_RenderTextureRotated(renderer, anim.texture, &src, &screenDest, 0, nullptr, flip);

    interface->otrisovka();
    skillHUD->render();

    // Обновление времени
    static Uint64 lastTime = SDL_GetTicks();
    Uint64 now = SDL_GetTicks();
    float deltaTime = (now - lastTime) / 1000.0f;
    lastTime = now;

    // Рендер всех навыков
    for (Skill* skill : skills) {
        skill->render(renderer, camera);
    }

    // Отрисовка хитбокса
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 120);
    SDL_FRect screenHitbox = camera->apply(hitbox);  // ← исправлено
    SDL_RenderRect(renderer, &screenHitbox);


}


bool Player::checkCollisionForRect(const SDL_FRect& rect) const {
    for (const auto& r : collisionRects)
        if (checkCollision(rect, r)) return true;
    return false;
}

bool Player::checkCollision(const SDL_FRect& a, const SDL_FRect& b) const {
    return (a.x < b.x + b.w && a.x + a.w > b.x &&
        a.y < b.y + b.h && a.y + a.h > b.y);
}

void Player::setEnemies(const std::vector<Enemy*>& e) { enemies = e; }

void Player::renderInventory() {
    if (inventoryOpen) inventory->render();
}

void Player::setAnimation(const std::string& name, bool loop) {
    if (currentAnim != name) {
        currentAnim = name; currentLoop = loop;
        animationHandler.reset(); src.x = src.y = 0;
    }
}

SDL_FRect Player::getRect() const { return rect; }
void Player::addMoney(int m) { money += m; }

void Player::updateHitbox() {
    hitbox = { (flip == SDL_FLIP_HORIZONTAL ? dest.x + 35 : dest.x + 15), dest.y + 24, dest.w - 50, dest.h - 25 };
}

void Player::updateFlip(const bool* keys) {
    SDL_FlipMode prevFlip = flip;
    if (keys[SDL_SCANCODE_A]) flip = SDL_FLIP_HORIZONTAL;
    else if (keys[SDL_SCANCODE_D]) flip = SDL_FLIP_NONE;
    if (prevFlip != flip) {
        dest.x += (flip == SDL_FLIP_HORIZONTAL) ? -20 : 20;
    }
}


SDL_FRect Player::getHitbox() const { return hitbox; }

void Player::attackHandler() {
    if (!isAttack) return;
    if (animationHandler.getCurrentFrame() >= 4 && !hasDealtDamage) {
        SDL_FRect atkBox = getAttackHitbox();
        for (Enemy* e : enemies) {
            if (e && checkCollision(atkBox, e->getHitbox())) {
                int damage = rand() % 8 + 10; // от 10 до 17
                e->takeDamage(damage);

                hasDealtDamage = true;
                break;
            }
        }
    }
    if (animationHandler.isFinished()) {
        isAttack = false; hasDealtDamage = false;
        setAnimation("idle", true);
    }
}

void Player::updateInventory() {
    if (!inventoryOpen) return;
    SDL_Event e;
    while (SDL_PollEvent(&e)) inventory->handleEvent(&e);
}

// остальные функции не изменены, по запросу продолжу...




void Player::moveHandler(const bool* keys) {
    isWalk = false;
    isRunning = false;
    updateFlip(keys);


    if (isAttack) {
        std::string prevAnim = currentAnim;
        currentAnim = "attack";

        if (prevAnim != currentAnim) {
            animationHandler.reset();
            src.x = src.y = 0;
        }

        animationHandler.update(animations[currentAnim], src, 48, false);
        // УДАЛИ вот эту строку ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
        // return;
    }



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
    // --- Анимации
    std::string prevAnim = currentAnim;

    if (isAttack) {
        currentAnim = "attack";
    }
    else if (isjump) {
        currentAnim = "jump";
    }
    else if (isWalk) {
        currentAnim = isRunning ? "run" : "walk";
    }
    else {
        currentAnim = "idle";
    }

    if (prevAnim != currentAnim) {
        animationHandler.reset();
        src.x = src.y = 0;
    }

    bool loopAnim = (currentAnim != "attack");
    animationHandler.update(animations[currentAnim], src, 48, loopAnim);


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
    float attackWidth = 10.0f;  // Расширяем ещё больше (с 90 → 110)
    float attackHeight = dest.h * 0.8f; // Делаем выше (с 1.1 → 1.2)
    float offset = 30.0f;
    float attackX;

    if (flip == SDL_FLIP_NONE) {
        attackX = dest.x + dest.w - offset;
    }
    else {
        attackX = dest.x - attackWidth + offset;
    }

    float attackY = dest.y + (dest.h - attackHeight) / 2.0f; // Центрируем по вертикали

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