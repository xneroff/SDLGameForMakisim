#include "Inventory.h"
#include <iostream>

Inventory::Inventory(SDL_Renderer* renderer)
    : renderer(renderer)
    , dragOffset{ 0.0f, 0.0f }  // Инициализируем смещение для drag&drop
    , draggingItem(nullptr)
    , previewAnim{ nullptr, 0, 0 }  // Инициализируем пустой аниматор
    , draggingItemOriginalRect{ 0,0,0,0 }  // Инициализируем rect
{
    background = IMG_LoadTexture(renderer, "assets/MoiInventory/Inventory_style_02d.png");
    SDL_SetTextureScaleMode(background, SDL_SCALEMODE_NEAREST);
    slotHighlight = IMG_LoadTexture(renderer, "assets/MoiInventory/Inventory_select.png");
    SDL_SetTextureScaleMode(slotHighlight, SDL_SCALEMODE_NEAREST);
    previewRect = { 662, 450, 104, 140 };

    SDL_Texture* idleTex = IMG_LoadTexture(renderer, "assets/1 Woodcutter/Woodcutter_idle.png");
    if (idleTex) {
        SDL_SetTextureScaleMode(idleTex, SDL_SCALEMODE_NEAREST);
        previewAnim = { idleTex, 4, 200 };  // 4 кадра, задержка 200 мс
    }


    // Центр инвентаря
    inventoryRect = { 554.0f, 306.0f, 812.0f, 468.0f };


    // Слоты (абсолютные позиции под фон)
    slots = {
        {586.0f, 338.0f, 64.0f, 64.0f}, {662.0f, 338.0f, 64.0f, 64.0f}, {738.0f, 338.0f, 64.0f, 64.0f},
        {814.0f, 338.0f, 64.0f, 64.0f}, {890.0f, 338.0f, 64.0f, 64.0f}, {966.0f, 338.0f, 64.0f, 64.0f},
        {1042.0f, 338.0f, 64.0f, 64.0f}, {1118.0f, 338.0f, 64.0f, 64.0f}, {1194.0f, 338.0f, 64.0f, 64.0f},
        {1270.0f, 338.0f, 64.0f, 64.0f}, // Верхняя панель инвентаря 

        {586.0f, 450.0f, 64.0f, 64.0f}, {586.0f, 526.0f, 64.0f, 64.0f}, {662.0f, 450.0f, 104.0f, 140.0f}, // Для персонажа
        {778.0f, 450.0f, 64.0f, 64.0f}, {778.0f, 526.0f, 64.0f, 64.0f},

        {586.0f, 602.0f, 256.0f, 140.0f}, // Для описания предмета

        {890.0f, 450.0f, 64.0f, 64.0f}, {966.0f, 450.0f, 64.0f, 64.0f}, {1042.0f, 450.0f, 64.0f, 64.0f}, {1118.0f, 450.0f, 64.0f, 64.0f}, {1194.0f, 450.0f, 64.0f, 64.0f},
        {966.0f, 450.0f, 64.0f, 64.0f}, {1042.0f, 450.0f, 64.0f, 64.0f}, {1118.0f, 450.0f, 64.0f, 64.0f}, {1194.0f, 450.0f, 64.0f, 64.0f}, {1270.0f, 450.0f, 64.0f, 64.0f}, // 1 строка инвентаря

        {890.0f, 526.0f, 64.0f, 64.0f}, {966.0f, 526.0f, 64.0f, 64.0f}, {1042.0f, 526.0f, 64.0f, 64.0f}, {1118.0f, 526.0f, 64.0f, 64.0f}, {1194.0f, 526.0f, 64.0f, 64.0f},
        {966.0f, 526.0f, 64.0f, 64.0f}, {1042.0f, 526.0f, 64.0f, 64.0f}, {1118.0f, 526.0f, 64.0f, 64.0f}, {1194.0f, 526.0f, 64.0f, 64.0f}, {1270.0f, 526.0f, 64.0f, 64.0f}, // 2 строка инвентаря

        {890.0f, 602.0f, 64.0f, 64.0f}, {966.0f, 602.0f, 64.0f, 64.0f}, {1042.0f, 602.0f, 64.0f, 64.0f}, {1118.0f, 602.0f, 64.0f, 64.0f}, {1194.0f, 602.0f, 64.0f, 64.0f},
        {966.0f, 602.0f, 64.0f, 64.0f}, {1042.0f, 602.0f, 64.0f, 64.0f}, {1118.0f, 602.0f, 64.0f, 64.0f}, {1194.0f, 602.0f, 64.0f, 64.0f}, {1270.0f, 602.0f, 64.0f, 64.0f}, // 3 строка инвентаря

        {890.0f, 678.0f, 64.0f, 64.0f}, {966.0f, 678.0f, 64.0f, 64.0f}, {1042.0f, 678.0f, 64.0f, 64.0f}, {1118.0f, 678.0f, 64.0f, 64.0f}, {1194.0f, 678.0f, 64.0f, 64.0f},
        {966.0f, 678.0f, 64.0f, 64.0f}, {1042.0f, 678.0f, 64.0f, 64.0f}, {1118.0f, 678.0f, 64.0f, 64.0f}, {1194.0f, 678.0f, 64.0f, 64.0f}, {1270.0f, 678.0f, 64.0f, 64.0f}  // 4 строка инвентаря
    };

}

Inventory::~Inventory() {
    SDL_DestroyTexture(background);
    for (auto& item : items)
        SDL_DestroyTexture(item.texture);
    SDL_DestroyTexture(slotHighlight);

}

SDL_Texture* Inventory::loadTexture(const std::string& path) {
    SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());
    if (!tex) std::cerr << " Не удалось загрузить: " << path << std::endl;
    else SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
    return tex;
}

void Inventory::addItem(const std::string& name, const std::string& path) {
    if (items.size() >= slots.size()) return;

    SDL_Texture* tex = loadTexture(path);
    if (!tex) return;

    SDL_FRect rect = slots[items.size()];
    items.push_back({ name, tex, rect });
}

void Inventory::render() {
    if (background) {
        SDL_RenderTexture(renderer, background, nullptr, &inventoryRect);
    }

    float mx_int, my_int;
    SDL_GetMouseState(&mx_int, &my_int);
    float mx = static_cast<float>(mx_int);
    float my = static_cast<float>(my_int);




    // Отрисовать выделение, если курсор над каким-то слотом
    for (const auto& slot : slots) {
        // Пропускаем прямоугольник персонажа
        if (slot.x == previewRect.x && slot.y == previewRect.y &&
            slot.w == previewRect.w && slot.h == previewRect.h) {
            continue;
        }

        if (mx > slot.x && mx < slot.x + slot.w &&
            my > slot.y && my < slot.y + slot.h) {
            if (slotHighlight) {
                SDL_FRect outlineRect = {
                    slot.x - 4,
                    slot.y - 4,
                    slot.w + 8,
                    slot.h + 8
                };
                SDL_RenderTexture(renderer, slotHighlight, nullptr, &outlineRect);
            }
            break;
        }
    }


    // 🎮 Отображение idle-анимации персонажа в превью-окне
    if (previewAnim.texture) {
        SDL_FRect src;
        src.x = previewHandler.getCurrentFrame() * 48;  // кадры идут по ширине
        src.y = 0;
        src.w = 48;
        src.h = 48;

        previewHandler.update(previewAnim, src, 48);

        SDL_FRect dst;
        dst.x = previewRect.x;
        dst.y = previewRect.y - 30;
        dst.w = previewRect.w + 80;
        dst.h = previewRect.h + 30;

        SDL_RenderTexture(renderer, previewAnim.texture, &src, &dst);
    }



    // Предметы
    for (const auto& item : items) {
        if (&item == draggingItem) continue;
        SDL_RenderTexture(renderer, item.texture, nullptr, &item.rect);
    }

    if (draggingItem) {
        SDL_RenderTexture(renderer, draggingItem->texture, nullptr, &draggingItem->rect);
    }
}

void Inventory::handleEvent(SDL_Event* event) {
    float mx_int, my_int;
    SDL_GetMouseState(&mx_int, &my_int);
    float mx = static_cast<float>(mx_int);
    float my = static_cast<float>(my_int);
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_LEFT) {
        if (!draggingItem) {
            // Попытка взять предмет
            for (auto& item : items) {
                if (mx > item.rect.x && mx < item.rect.x + item.rect.w &&
                    my > item.rect.y && my < item.rect.y + item.rect.h) {
                    draggingItem = &item;
                    draggingItemOriginalRect = item.rect;
                    break;
                }
            }
        }
        else {
            // Пытаемся положить предмет в слот
            SDL_FRect* closestSlot = nullptr;
            float minDist = std::numeric_limits<float>::max();

            for (auto& slot : slots) {
                float dx = mx - (slot.x + slot.w / 2.0f);
                float dy = my - (slot.y + slot.h / 2.0f);
                float dist = dx * dx + dy * dy;

                if (dist < minDist) {
                    minDist = dist;
                    closestSlot = &slot;
                }
            }

            // Проверка: занят ли слот
            bool slotOccupied = false;
            if (closestSlot) {  // ОБЯЗАТЕЛЬНАЯ ПРОВЕРКА!
                for (const auto& item : items) {
                    if (&item != draggingItem &&
                        item.rect.x == closestSlot->x &&
                        item.rect.y == closestSlot->y) {
                        slotOccupied = true;
                        break;
                    }
                }
            }

            if (closestSlot != nullptr) {
                // Проверяем, чтобы не положить предмет в previewRect и чтобы слот не был занят
                if (!(closestSlot->x == previewRect.x &&
                    closestSlot->y == previewRect.y &&
                    closestSlot->w == previewRect.w &&
                    closestSlot->h == previewRect.h)
                    && !slotOccupied)
                {
                    draggingItem->rect.x = closestSlot->x;
                    draggingItem->rect.y = closestSlot->y;
                }
                else {
                    // Если слот занят или это previewRect, возвращаем предмет обратно
                    draggingItem->rect = draggingItemOriginalRect;
                }
            }
            else {
                draggingItem->rect = draggingItemOriginalRect;
            }

            draggingItem = nullptr; // отпускаем предмет
        }
    }

    if (draggingItem) {
        // предмет следует за курсором
        draggingItem->rect.x = mx - draggingItem->rect.w / 2.0f;
        draggingItem->rect.y = my - draggingItem->rect.h / 2.0f;
    }
}
