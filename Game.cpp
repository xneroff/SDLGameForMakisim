#include "Game.h"
#include <vector>
#include <string>
#include "FireballSkill.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "TileMap.h"
#include "Enemy.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "NPC.h"
#include "Player.h"

std::vector<NPC*> npcs;
void Game::startTeleport(const std::string& map, const std::string& spawn) {
    isTeleporting = true;
    shouldLoadNextMap = false;
    teleportTimer = 3.0f;
            
    teleportTargetMap = map;
    teleportTargetSpawn = spawn;
}

bool checkCollision(const SDL_FRect& a, const SDL_FRect& b) {
    return a.x < b.x + b.w && a.x + a.w > b.x &&
        a.y < b.y + b.h && a.y + a.h > b.y;
}

std::vector<FloatingText> floatingTexts;


void showFloatingText(const std::string& text, float x, float y) {
    floatingTexts.push_back({ text, {x, y}, SDL_GetTicks(), 2000 }); // 2 секунды
}

void renderFloatingTexts(SDL_Renderer* renderer, TTF_Font* font, Camera* camera) {
    Uint32 now = SDL_GetTicks();
    SDL_Color color = { 255, 255, 255, 255 };

    for (auto it = floatingTexts.begin(); it != floatingTexts.end();) {
        if (now - it->startTime > it->duration) {
            it = floatingTexts.erase(it);
        }
        else {
            SDL_Surface* surface = TTF_RenderText_Solid(font, it->text.c_str(), it->text.length(), color);

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);

            SDL_FRect screenPos = camera->apply({ it->position.x, it->position.y - ((now - it->startTime) / 20.0f), 0, 0 });

            float w = 0, h = 0;
            SDL_GetTextureSize(texture, &w, &h);

            SDL_FRect dst = { screenPos.x, screenPos.y, (float)w, (float)h };
            SDL_RenderTexture(renderer, texture, nullptr, &dst);

            SDL_DestroyTexture(texture);
            ++it;
        }
    }
}

Game::Game() : font(nullptr), window(nullptr), renderer(nullptr), camera(nullptr), player(nullptr), menu(nullptr)
{
    // остальные инициализации, если нужны
}


Game::~Game()
{
    // Очистка ресурсов игры

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    if (dialogFont) TTF_CloseFont(dialogFont);
    if (dialogNameFont) TTF_CloseFont(dialogNameFont);

    // Уничтожение окна и рендерера
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    TTF_Quit();
    SDL_Quit();
}




SDL_AppResult Game::SDL_AppInit()
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_CreateWindowAndRenderer("SDL3 Game", 1920, 1080, SDL_WINDOW_RESIZABLE, &window, &renderer);
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    font = TTF_OpenFont("assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 18);
    dialogFont = TTF_OpenFont("assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 16);
    dialogNameFont = TTF_OpenFont("assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 18);
  
    dialogBoxTexture = IMG_LoadTexture(renderer, "assets/NPC/dialogue2.png");
    SDL_SetTextureScaleMode(dialogBoxTexture, SDL_SCALEMODE_NEAREST);
    camera = new Camera(1920.0f, 1080.0f);


    player = new Player(renderer, font, camera, this);

    SDL_FRect playerRect = player->getRect();

    tileMap = new TileMap(renderer);
    tileMap->loadFromFile("assets/map/MEGATEST.json");


    float groundY = tileMap->getSpawnPoint().y;

    // Создаем врагов
 // Создаем врагов
    enemies.push_back(new Enemy(renderer, 800.0f, 250.0f, EnemyType::Default));
    enemies.push_back(new Enemy(renderer, 850.0f, 250.0f, EnemyType::Default));
  


    // Game::SDL_AppInit()

    //enemies.push_back(new Enemy(renderer, 800.0f, 250.0f, EnemyType::Boar));
    //enemies.push_back(new Enemy(renderer, 800.0f, 250.0f, EnemyType::Fox));
    //enemies.push_back(new Enemy(renderer, 1200.0f, 230.0f, EnemyType::Bird));

    float playerY = player->getDest().y;
    SDL_FPoint npc1Pos = tileMap->getNPCSpawn("NPCSpawn1");


    SDL_FPoint pos1 = tileMap->getNPCSpawn("NPCSpawn1");
    std::vector<std::string> phrases1 = {
        "Hello, Olver!"
        "U need be safe in this place",
        "ff"
    };
    npcs.push_back(new NPC(renderer, pos1.x, pos1.y - 64, "Старейшина", phrases1));

    SDL_FPoint pos2 = tileMap->getNPCSpawn("NPCSpawn2");
    std::vector<std::string> phrases2 = {
        "Здесь обитает древнее зло.",
        "Ты храбр, раз пришёл сюда.",
        "Возьми это — тебе пригодится."
    };
    npcs.push_back(new NPC(renderer, pos2.x, pos2.y - 64, "Рыцарь", phrases2));

    startMenu = new StartMenu(renderer, font, window);
    
    for (Enemy* enemy : enemies) {
        enemy->setCollisionRects(tileMap->getCollisionRects());
    }

    // Передаем врагов игроку
    player->setEnemies(enemies);
    player->setCollisions(tileMap->getCollisionRects());
    player->setPosition(tileMap->getSpawnPoint().x, tileMap->getSpawnPoint().y);
    SDL_FPoint spawn = tileMap->getSpawnPoint();
    float playerHeight = 64.0f;  // замени на реальную высоту спрайта игрока
    spawn.y -= playerHeight;     // сдвигаем точку спавна вниз, чтобы игрок стоял на земле
    player->setPosition(spawn.x, spawn.y);


    menu = new MainMenu(renderer, font, window);  // 🔧 добавлено

    // Проверяем агр-зону и устанавливаем состояние для всех врагов
    for (Enemy* enemy : enemies) {
        double distance = sqrt(pow(playerRect.x - enemy->getRect().x, 2) +
            pow(playerRect.y - enemy->getRect().y, 2));

        enemy->setAggroState(distance < enemy->getAggroRadius());

    }
    return SDL_AppResult();
}



SDL_AppResult Game::SDL_AppEvent(SDL_Event* event)
{
    if (showStartMenu) {
        bool startGame = false, showSettings = false, quit = false;
        startMenu->handleEvent(*event, startGame, showSettings, quit);
        if (startGame) {
            showStartMenu = false;
            // Запускаем игру
        }
        if (quit) {
            // Завершаем приложение
        }
    }

    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_ESCAPE)
    {
        if (showMenu) {
            if (!menu || !menu->isInSettings()) {
                showMenu = false;
            }
        }
        else {
            showMenu = true;
        }
    }

    if (showMenu) {
        bool resume = false;
        if (menu)
            menu->handleEvent(*event, resume, quit);
        if (resume) showMenu = false;
    }
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_F) {
        if (!teleportConfirmDialogOpen && lastPortalInRange) {
            pendingTeleport = *lastPortalInRange;
            teleportConfirmDialogOpen = true;
        }

    }

    if (teleportConfirmDialogOpen) {
    if (event->type == SDL_EVENT_KEY_DOWN) {
        if (event->key.key == SDLK_Y) {
            startTeleport(pendingTeleport.targetMap, pendingTeleport.targetSpawn);
            teleportConfirmDialogOpen = false;
        }
        else if (event->key.key == SDLK_N) {
            teleportConfirmDialogOpen = false; // Просто закроем окно, не телепортируемся
        }
    }
    return SDL_APP_CONTINUE;
}


    else {
        player->obrabotkaklavish(event);
    }

    return quit ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
}



SDL_AppResult Game::SDL_AppIterate()
{
    if (showStartMenu) {
        startMenu->render();
        return SDL_APP_CONTINUE;
    }

    static Uint64 lastTime = SDL_GetTicks();
    Uint64 currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    tileMap->renderLayer(renderer, camera, "Tile Layer 1");
    tileMap->renderLayer(renderer, camera, "Tile Layer 2");
    tileMap->renderLayer(renderer, camera, "Tile Layer 3");
    tileMap->renderLayer(renderer, camera, "Tile Layer 4");
    tileMap->renderLayer(renderer, camera, "Tile Layer 5");
    tileMap->renderLayer(renderer, camera, "Tile Layer 6");
    tileMap->renderLayer(renderer, camera, "Tile Layer 7");
    tileMap->renderCollisions(renderer, camera);
    
    // Проверка попадания в ловушку
    SDL_FRect playerRect = player->getDest();
    for (const auto& trap : tileMap->getTraps()) {
        if (SDL_HasRectIntersectionFloat(&playerRect, &trap)) {
             player->takeDamage(player->getHealth()); // или player->die(), если есть
            break;
        }
    }


    if (player->isDead()) {
        if (!gameOver) {
            gameOver = true;
            deathTime = SDL_GetTicks();
        }

        Uint32 elapsed = SDL_GetTicks() - deathTime;

        SDL_Color red = { 255, 0, 0, 255 };
        SDL_Surface* surface = TTF_RenderText_Blended(font, "GAME OVER", strlen("GAME OVER"), red);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);

            float w, h;
            SDL_GetTextureSize(texture, &w, &h);
            SDL_FRect dst = { 1920 / 2.0f - w / 2.0f, 1080 / 2.0f - h / 2.0f, w, h };
            SDL_RenderTexture(renderer, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }

        SDL_RenderPresent(renderer);

        if (elapsed >= restartDelay) {
            // Перезапускаем игру:
            SDL_AppQuit(SDL_APP_SUCCESS);  // очистим старое состояние                // пересоздаём объект игры
            SDL_AppInit();                 // запускаем заново
        }

        SDL_Delay(16);
        return SDL_APP_CONTINUE;
    }


    if (showMenu) {
        menu->render();
    }
    else {
        const bool* keys = SDL_GetKeyboardState(nullptr);
        camera->update(player->getDest(), tileMap->getMapWidth(), tileMap->getMapHeight(), tileMap->getTileWidth(), tileMap->getTileHeight());
        const float minZoom = 1.5f;
        const float maxZoom = 3.5f;

        if (keys[SDL_SCANCODE_EQUALS]) camera->zoom += 0.01f;
        if (keys[SDL_SCANCODE_MINUS]) camera->zoom -= 0.01f;

        if (camera->zoom < minZoom) camera->zoom = minZoom;
        if (camera->zoom > maxZoom) camera->zoom = maxZoom;


        player->otrisovka();
        player->obnovleniepersa();

        for (Skill* skill : player->getSkills()) {
            FireballSkill* fireball = dynamic_cast<FireballSkill*>(skill);
            if (fireball && fireball->isActive()) {
                for (Enemy* enemy : enemies) {
                    if (checkCollision(fireball->getRect(), enemy->getRect())) {
                        enemy->takeDamage(10);
                        fireball->deactivate();
                        break;
                    }
                }
            }
        }

        static bool ePreviouslyPressed = false;
        bool ePressed = keys[SDL_SCANCODE_E];

        if (ePressed && !ePreviouslyPressed) {
            for (auto& chest : tileMap->getChestsMutable()) {
                SDL_FRect playerRect = player->getDest();
                if (!chest.opened && SDL_HasRectIntersectionFloat(&playerRect, &chest.rect)) {
                    chest.opened = true;
                    std::string message = "+ " + std::to_string(chest.amount) + " x " + chest.item;
                    showFloatingText(message, chest.rect.x, chest.rect.y);
                }
            }

            for (auto& npc : npcs) {
                if (npc->isNearPlayer(player->getDest())) {
                    if (!npc->showDialog) {
                        npc->showDialog = true;
                        npc->currentPhrase = 0;
                    }
                    else {
                        npc->currentPhrase++;
                        if (npc->currentPhrase >= npc->dialogPhrases.size())
                            npc->showDialog = false;
                    }
                    break;
                }
            }
        }

        ePreviouslyPressed = ePressed;

        for (auto npc : npcs) {
            if (npc->showDialog && !npc->isNearPlayer(player->getDest()))
                npc->showDialog = false;

            npc->update(deltaTime);
            npc->render(renderer, camera);

            if (npc->showDialog) {
                std::string phrase = npc->dialogPhrases[npc->currentPhrase];
                std::string name = npc->getName();  // Предполагается, что у NPC есть метод getName()

                // Размер окна и позиция (центр внизу экрана)
                float dialogW = 900;
                float dialogH = 200;
                float centerX = (1920.0f - dialogW) / 2.0f;
                float bottomY = 1080.0f - dialogH - 35;

                SDL_FRect dialogRect = { centerX, bottomY, dialogW, dialogH };

                // Отрисовка текстуры окна
                SDL_RenderTexture(renderer, dialogBoxTexture, nullptr, &dialogRect);

                // 🏷️ Имя NPC
           
                SDL_Color nameColor = { 255, 223, 100, 255 };
                SDL_Surface* nameSurf = TTF_RenderText_Blended(dialogNameFont, name.c_str(), name.length(), nameColor);
                if (nameSurf) {
                    SDL_Texture* nameTex = SDL_CreateTextureFromSurface(renderer, nameSurf);
                    SDL_FRect nameRect = {
                        dialogRect.x + 165,
                        dialogRect.y + 25,
                        (float)nameSurf->w,
                        (float)nameSurf->h
                    };
                    SDL_DestroySurface(nameSurf);
                    SDL_RenderTexture(renderer, nameTex, nullptr, &nameRect);
                    SDL_DestroyTexture(nameTex);
                }

                // 💬 Текст диалога
                SDL_Color white = { 255, 255, 255, 255 };
          
                SDL_Surface* textSurf = TTF_RenderText_Blended(dialogFont, phrase.c_str(), phrase.length(), white);

                if (textSurf) {
                    SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
                    SDL_FRect textRect = {
                        dialogRect.x + 60,
                        dialogRect.y + 70,  // текст ниже имени
                        (float)textSurf->w,
                        (float)textSurf->h
                    };
                    SDL_DestroySurface(textSurf);
                    SDL_RenderTexture(renderer, textTex, nullptr, &textRect);
                    SDL_DestroyTexture(textTex);
                }
            }


        }

        if (player->getIsAttack() && player->getCurrentAttackFrame() == 5) {
            SDL_FRect attackBox = player->getAttackHitbox();
            for (Enemy* enemy : enemies) {
                if (checkCollision(attackBox, enemy->getRect())) {
                    enemy->takeDamage(10);
                }
            }
        }

        for (auto it = enemies.begin(); it != enemies.end();) {
            Enemy* enemy = *it;
            enemy->update(deltaTime, player);
            enemy->render(renderer, camera);

            if (enemy->isMarkedForDeletion()) {
                delete enemy;
                it = enemies.erase(it);
            }
            else {
                ++it;
            }
        }
        for (const auto& label : tileMap->getLabels()) {
            SDL_FRect screenBox = camera->apply(label.rect);
            SDL_Color color = { 255, 255, 255, 255 };
            SDL_Surface* surface = TTF_RenderText_Solid(font, label.text.c_str(),0, color);


            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_DestroySurface(surface);

                float w = 0, h = 0;
                SDL_GetTextureSize(texture, &w, &h);

                SDL_FRect dst = {
                    screenBox.x + (screenBox.w - w) / 2.0f,
                    screenBox.y + (screenBox.h - h) / 2.0f,
                    (float)w, (float)h
                };

                SDL_RenderTexture(renderer, texture, nullptr, &dst);
                SDL_DestroyTexture(texture);
            }
        }
        if (isTeleporting) {
            teleportTimer -= deltaTime;

            if (!shouldLoadNextMap && teleportTimer <= 2.95f) {
                shouldLoadNextMap = true;
            }

            if (shouldLoadNextMap) {
                shouldLoadNextMap = false;
                delete tileMap;
                tileMap = new TileMap(renderer);
                tileMap->loadFromFile("assets/map/" + teleportTargetMap);

                player->setCollisions(tileMap->getCollisionRects());
                SDL_FPoint spawn = tileMap->getGenericSpawnPointByName(teleportTargetSpawn);
                spawn.y -= 64;
                player->setPosition(spawn.x, spawn.y);

                isTeleporting = false;
            }
        }


        player->renderInventory();
        renderFloatingTexts(renderer, font, camera);
    }
    if (teleportConfirmDialogOpen) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
        SDL_RenderFillRect(renderer, nullptr);


        const char* msg = "Перейти в другую локацию?\n[Y] — Да    [N] — Нет";
        SDL_Color white = { 255, 255, 255, 255 };   

        SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(font, msg, strlen(msg), white, 600);


        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            float w = 0, h = 0;
            SDL_GetTextureSize(tex, &w, &h);
            SDL_FRect dst = { 960 - w / 2.0f, 540 - h / 2.0f, w, h };
            SDL_RenderTexture(renderer, tex, nullptr, &dst);
            SDL_DestroySurface(surf);
            SDL_DestroyTexture(tex);
        }
    }

    if (isTeleporting) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, nullptr);

        SDL_Color white = { 255, 255, 255, 255 };
        SDL_Surface* surf = TTF_RenderText_Solid(font, "Loading...", strlen("Loading..."), white);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            float w = 0, h = 0;
            SDL_GetTextureSize(tex, &w, &h);
            SDL_FRect dst = { 960 - w / 2.0f, 540 - h / 2.0f, w, h };
            SDL_RenderTexture(renderer, tex, nullptr, &dst);
            
            SDL_DestroySurface(surf);
            SDL_DestroyTexture(tex);

        }
    }   

    SDL_RenderPresent(renderer);
    SDL_Delay(16);
    return quit ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
}

void Game::SDL_AppQuit(SDL_AppResult result)
{
    // Всё удаляется в деструкторе
    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();

}