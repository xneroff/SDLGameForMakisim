// Enemy.cpp
#include "Enemy.h"
#include <iostream>
#include <cmath>

std::vector<Enemy*> Enemy::allEnemies;

static float distanceBetween(const SDL_FRect& a, const SDL_FRect& b) {
    float dx = (a.x + a.w / 2.0f) - (b.x + b.w / 2.0f);
    float dy = (a.y + a.h / 2.0f) - (b.y + b.h / 2.0f);
    return sqrtf(dx * dx + dy * dy);
}

Enemy::Enemy(SDL_Renderer* renderer, float x, float y, EnemyType type)
    : renderer(renderer), health(100), maxHealth(100), type(type) {
    spawnPoint = { x, y };
    rect = { x, y, 90, 90 };
    allEnemies.push_back(this);

    auto loadTexture = [&](const std::string& name, const std::string& path) {
        SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
        if (!tex) std::cerr << "Failed to load " << path << ": " << SDL_GetError() << std::endl;
        textures[name] = tex;
        };

    if (type == EnemyType::GraveRobber) {
        loadTexture("idle", "assets/NPC/2 GraveRobber/GraveRobber_idle.png");
        loadTexture("walk", "assets/NPC/2 GraveRobber/GraveRobber_walk.png");
        loadTexture("attack1", "assets/NPC/2 GraveRobber/GraveRobber_attack1.png");
        loadTexture("attack2", "assets/NPC/2 GraveRobber/GraveRobber_attack2.png");
        loadTexture("death", "assets/NPC/2 GraveRobber/GraveRobber_death.png");
        loadTexture("hurt", "assets/NPC/2 GraveRobber/GraveRobber_hurt.png");

        frameCounts["idle"] = 4;
        frameCounts["walk"] = 6;
        frameCounts["attack1"] = 6;
        frameCounts["attack2"] = 6;
        frameCounts["death"] = 6;
        frameCounts["hurt"] = 3;

        health = maxHealth = 500;
        speed = 40.0f;
        rect.w = rect.h = 130; // размер побольше

    }
    else {
        loadTexture("idle", "assets/1 Enemies/2/idle.png");
        loadTexture("walk", "assets/1 Enemies/2/walk.png");
        loadTexture("attack", "assets/1 Enemies/2/attack.png");
        loadTexture("death", "assets/1 Enemies/2/death.png");
        loadTexture("hurt", "assets/1 Enemies/2/hurt.png");

        frameCounts["idle"] = 4;
        frameCounts["walk"] = 6;
        frameCounts["attack"] = 6;
        frameCounts["death"] = 6;
        frameCounts["hurt"] = 2;
    }


    initRectSize();
    setAnimation("idle");
}

Enemy::~Enemy() {
    for (auto& pair : textures) {
        if (pair.second) SDL_DestroyTexture(pair.second);
    }
    textures.clear();
}

void Enemy::setAggroState(bool state) {
    this->state = state ? EnemyState::Aggro : EnemyState::Idle;
}

void Enemy::setAnimation(const std::string& anim) {
    if (!textures.count(anim) || !textures[anim]) {
        std::cerr << "Texture for animation '" << anim << "' not found!\n";
        return;
    }

    if (currentAnim == anim && totalFrames > 1) return;
    currentAnim = anim;

    if (frameCounts.count(anim) == 0) {
        std::cerr << "Frame count for animation '" << anim << "' not found!\n";
        return;
    }

    totalFrames = frameCounts[anim];
    SDL_Texture* tex = textures[anim];

    float w = 0, h = 0;
    SDL_GetTextureSize(tex, &w, &h);

    if (w == 0 || totalFrames == 0) {
        std::cerr << "Invalid texture size or frame count for '" << anim << "'\n";
    }

    frameWidth = w / totalFrames;
    frameHeight = h;

    currentFrame = 0;
    animationTimer = 0.0f;
}


void Enemy::takeDamage(int amount) {
    if (isDead) return;
    health -= amount;
    if (health > 0) {
        isHurt = true;
        hurtTimer = 0.0f;
        setAnimation("hurt");
    }

    else {
        health = 0;
        isDead = true;
        state = EnemyState::Dead;
        deathTimer = 0.0f;
        setAnimation("death");
    }
}

void Enemy::initRectSize() {
    rect.w = 70;
    rect.h = 70;
}

void Enemy::setCollisionRects(const std::vector<SDL_FRect>& rects) {
    collisionRects = rects;
}

SDL_FRect Enemy::getHitbox() const {
    return { rect.x, rect.y, rect.w * 1.2f, rect.h * 1.2f };
}

SDL_FRect Enemy::getRect() const {
    return rect;
}

bool Enemy::isMarkedForDeletion() const {
    return markedForDeletion;
}

void Enemy::setPosition(float x, float y) {
    rect.x = x;
    rect.y = y;
}

void Enemy::render(SDL_Renderer* renderer, Camera* camera) {
    SDL_Texture* tex = textures[currentAnim];
    if (!tex) return;

    SDL_FRect srcRect = { static_cast<float>(currentFrame * frameWidth), 0.0f, (float)frameWidth, (float)frameHeight };
    SDL_FRect screenRect = camera->apply(rect);
    SDL_FlipMode flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

    if (isDead)
        SDL_SetTextureAlphaMod(tex, static_cast<Uint8>(deathAlpha));

    SDL_RenderTextureRotated(renderer, tex, &srcRect, &screenRect, 0.0, nullptr, flip);

    if (isDead)
        SDL_SetTextureAlphaMod(tex, 255);

    if (isAttacking) {
        SDL_FRect attackBox = getAttackHitbox();
        SDL_FRect screenAttackBox = camera->apply(attackBox);

        // Сохраняем текущий цвет рисования
        Uint8 r, g, b, a;
        SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);  // Красный цвет с прозрачностью
        SDL_RenderRect(renderer, &screenAttackBox);

        // Восстанавливаем цвет
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
    }

    SDL_FRect screenHitbox = camera->apply(hitbox);
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 150); // голубой с прозрачностью
    SDL_RenderRect(renderer, &screenHitbox);



}

void Enemy::setGroundRects(const std::vector<SDL_FRect>& rects) {
    groundRects = rects;
}

void Enemy::setObstacleRects(const std::vector<SDL_FRect>& rects) {
    obstacleRects = rects;
}


void Enemy::update(float deltaTime, Player* player)
{

    float shrinkAmount = 35.0f;  // на сколько пикселей хотим сузить хитбокс по ширине
    hitbox.x = rect.x + shrinkAmount / 2.0f;  // сдвигаем внутрь на половину shrinkAmount
    hitbox.y = rect.y + 12;   // если хочешь, оставь подъем по Y как есть
    hitbox.w = rect.w - shrinkAmount;  // уменьшаем ширину
    hitbox.h = rect.h - 1;   // оставляем высоту как есть


    bool wasOnGround = isOnGround;

    // ❶ Всегда двигаем кадры раньше любых return
    animationTimer += deltaTime;
    if (animationTimer >= 0.15f) {
        animationTimer = 0.0f;
        currentFrame = (currentFrame + 1) % totalFrames;
    }

    // --- смерть -----------------------------------------------------------
    if (state == EnemyState::Dead) {
        deathTimer += deltaTime;
        deathAlpha -= (255.0f / deathDuration) * deltaTime;
        if (deathAlpha < 0.0f) deathAlpha = 0.0f;
        if (deathTimer >= deathDuration) markedForDeletion = true;
        return;
    }

    // --- получение урона --------------------------------------------------
    if (isHurt) {
        hurtTimer += deltaTime;
        if (hurtTimer >= hurtDuration) {
            isHurt = false;
            setAnimation("idle");
        }
        return;
    }

    // --- атака ------------------------------------------------------------
    if (isAttacking) {
        // Всегда смотрим на игрока, даже во время атаки
        float dx = player->getRect().x - rect.x;
        facingRight = dx >= 0;

        attackAnimTimer += deltaTime;
        if (!hasDealtDamageInThisAttack && currentFrame == 3) {
            SDL_FRect attackBox = getAttackHitbox();
            SDL_FRect playerBox = player->getHitbox();

            if (SDL_HasRectIntersectionFloat(&attackBox, &playerBox)) {
                player->takeDamage(12 + rand() % 4);
                hasDealtDamageInThisAttack = true;
            }
        }

        if (attackAnimTimer >= attackAnimDuration) {
            isAttacking = false;
            setAnimation("idle");
        }
        return;
    }


    int damage = 12 + rand() % 4;
    float aggroRadius = 150.0f;

    if (type == EnemyType::GraveRobber) {
        damage = 88 + rand() % 2;
        aggroRadius = 300.0f;
    }




    timeSinceLastAttack += deltaTime;

    float dx = player->getRect().x - rect.x;
    float dy = player->getRect().y - rect.y;
    float distance = sqrtf(dx * dx + dy * dy);

    float enemyCenterX = rect.x + rect.w / 2.0f;
    float playerCenterX = player->getRect().x + player->getRect().w / 2.0f;
    float distanceX = fabsf(enemyCenterX - playerCenterX);

    float desiredDistance = 25.0f; // минимальная дистанция до игрока



    if (state == EnemyState::Aggro) {
        for (Enemy* other : Enemy::allEnemies) {
            if (other != this && !other->isDeadNow()) {
                if (distanceBetween(this->rect, other->getRect()) < 150.0f)
                    other->state = EnemyState::Aggro;
            }
        }

        if (distance < 50.0f && timeSinceLastAttack >= attackCooldown) {
            if (type == EnemyType::GraveRobber) {
                // можно чередовать атаки: attack1 и attack2
                static bool useAltAttack = false;
                setAnimation(useAltAttack ? "attack1" : "attack2");
                useAltAttack = !useAltAttack;

            }
            else {
                setAnimation("attack");
            }

            isAttacking = true;
            attackAnimTimer = 0.0f;
            hasDealtDamageInThisAttack = false;
            timeSinceLastAttack = 0.0f;
            attackHitbox = player->getHitbox();
        }

        else if (distanceX > desiredDistance) {
            setAnimation("walk");
            float moveX = 0.0f;
            if (distance != 0.0f) {
                moveX = (dx / distance) * speed * deltaTime;
            }


            // Проверка препятствий
            SDL_FRect nextRect = rect;
            nextRect.x += moveX;

            bool obstacleAhead = false;
            SDL_FRect obstacleRect;

            for (const auto& wall : obstacleRects) {  // используй obstacleRects вместо collisionRects
                if (SDL_HasRectIntersectionFloat(&nextRect, &wall)) {
                    obstacleAhead = true;
                    obstacleRect = wall;
                    break;
                }
            }




            if (obstacleAhead) {
                float heightDiff = (obstacleRect.y - rect.y);


                if (heightDiff > -maxJumpHeight && heightDiff < 0) {
                    if (isOnGround) {
                        velocityY = jumpVelocity;
                        isOnGround = false;
                    }
                }
                else {
                    moveX = 0;
                    setAnimation("idle");
                }


            }

            rect.x += moveX;
            facingRight = dx >= 0;
        }
        else {
            setAnimation("idle");
        }

        if (distance > aggroRadius * 1.5f)
            state = EnemyState::Returning;
    }


    else if (state == EnemyState::Idle) {
        if (distance < aggroRadius) {
            suspicionTimer += deltaTime;
            if (suspicionTimer > suspicionThreshold) {
                state = EnemyState::Aggro;
            }
            else {
                state = EnemyState::Suspicious;
            }
        }
        else {
            suspicionTimer = 0; // сброс, если игрок далеко
        }
    }

    else if (state == EnemyState::Suspicious) {
        if (distance < aggroRadius) {
            suspicionTimer += deltaTime;
            if (suspicionTimer >= suspicionThreshold) state = EnemyState::Aggro;
        }
        else {
            suspicionTimer = 0;
            state = EnemyState::Idle;
        }
    }

    else if (state == EnemyState::Returning) {
        float back = spawnPoint.x - rect.x;

        if (fabsf(back) > 5.0f) {
            setAnimation("walk");
            rect.x += (back / fabsf(back)) * speed * deltaTime;
            facingRight = back >= 0;
        }
        else {
            rect.x = spawnPoint.x;
            state = EnemyState::Idle;
            setAnimation("idle");
            suspicionTimer = 0.0f; // сбрасываем таймер подозрения
        }
    }


    velocityY += gravity;
    rect.y += velocityY;
    isOnGround = false;

    // создаём прямоугольник для ног (меньше и выше)
    SDL_FRect feetRect = rect;
    feetRect.y += rect.h - 8;  // только нижние 8 пикселей
    feetRect.h = 8;

    for (const auto& wall : collisionRects) {
        SDL_FRect inter;
        if (SDL_GetRectIntersectionFloat(&hitbox, &wall, &inter)) {
            if (velocityY > 0) {
                rect.y = wall.y - rect.h;
                velocityY = 0;
                isOnGround = true;
            }
            else if (velocityY < 0) {
                rect.y = wall.y + wall.h;
                velocityY = 0;
            }
            break;
        }
    }


}

SDL_FRect Enemy::getAttackHitbox() const {
    float width = rect.w * 0.8f;    // ширина хитбокса
    float height = rect.h * 0.8f;   // высота хитбокса

    float offsetX = rect.w * 0.6f;  // сдвиг хитбокса внутрь врага по X (30% ширины тела)

    float x = facingRight ? rect.x + rect.w - offsetX : rect.x - width + offsetX;
    float y = rect.y + rect.h * 0.2f; // чуть ниже верхней границы врага

    return SDL_FRect{ x, y, width, height };
}
