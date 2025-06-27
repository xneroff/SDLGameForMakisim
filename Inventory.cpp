#include "Inventory.h"
#include <iostream>

// ───────────────────────────────
//          Constructor
// ───────────────────────────────
Inventory::Inventory(SDL_Renderer* renderer)
    : renderer(renderer),
    dragOffset{ 0.0f, 0.0f },
    draggingItem(nullptr),
    previewAnim{ nullptr, 0, 0 },
    draggingItemOriginalRect{ 0, 0, 0, 0 }
{
    // Загрузка графики
    background = IMG_LoadTexture(renderer, "assets/MoiInventory/Inventory_style_02d.png");
    SDL_SetTextureScaleMode(background, SDL_SCALEMODE_NEAREST);

    slotHighlight = IMG_LoadTexture(renderer, "assets/MoiInventory/Inventory_select.png");
    SDL_SetTextureScaleMode(slotHighlight, SDL_SCALEMODE_NEAREST);

    previewRect = { 662, 450, 104, 140 };

    // Загрузка idle-анимации персонажа в инвентаре
    SDL_Texture* idleTex = IMG_LoadTexture(renderer, "assets/1 Woodcutter/Woodcutter_idle.png");
    if (idleTex) {
        SDL_SetTextureScaleMode(idleTex, SDL_SCALEMODE_NEAREST);
        previewAnim = { idleTex, 4, 200 }; // 4 кадра, 200 мс задержка
    }

    inventoryRect = { 554.0f, 306.0f, 812.0f, 468.0f }; // основной прямоугольник интерфейса

    // Координаты всех слотов
    slots = {
        // Верхняя панель
        {586, 338, 64, 64}, {662, 338, 64, 64}, {738, 338, 64, 64},
        {814, 338, 64, 64}, {890, 338, 64, 64}, {966, 338, 64, 64},
        {1042, 338, 64, 64}, {1118, 338, 64, 64}, {1194, 338, 64, 64}, {1270, 338, 64, 64},

        // Окна экипировки и preview
        {586, 450, 64, 64}, {586, 526, 64, 64}, {662, 450, 104, 140},
        {778, 450, 64, 64}, {778, 526, 64, 64},

        // Описание предмета
        {586, 602, 256, 140},

        // Слоты предметов
        {890, 450, 64, 64}, {966, 450, 64, 64}, {1042, 450, 64, 64}, {1118, 450, 64, 64}, {1194, 450, 64, 64},
        {966, 450, 64, 64}, {1042, 450, 64, 64}, {1118, 450, 64, 64}, {1194, 450, 64, 64}, {1270, 450, 64, 64},

        {890, 526, 64, 64}, {966, 526, 64, 64}, {1042, 526, 64, 64}, {1118, 526, 64, 64}, {1194, 526, 64, 64},
        {966, 526, 64, 64}, {1042, 526, 64, 64}, {1118, 526, 64, 64}, {1194, 526, 64, 64}, {1270, 526, 64, 64},

        {890, 602, 64, 64}, {966, 602, 64, 64}, {1042, 602, 64, 64}, {1118, 602, 64, 64}, {1194, 602, 64, 64},
        {966, 602, 64, 64}, {1042, 602, 64, 64}, {1118, 602, 64, 64}, {1194, 602, 64, 64}, {1270, 602, 64, 64},

        {890, 678, 64, 64}, {966, 678, 64, 64}, {1042, 678, 64, 64}, {1118, 678, 64, 64}, {1194, 678, 64, 64},
        {966, 678, 64, 64}, {1042, 678, 64, 64}, {1118, 678, 64, 64}, {1194, 678, 64, 64}, {1270, 678, 64, 64}
    };
}

// ───────────────────────────────
//          Destructor
// ───────────────────────────────
Inventory::~Inventory() {
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(slotHighlight);
    for (auto& item : items)
        SDL_DestroyTexture(item.texture);
}

// ───────────────────────────────
//      Load Texture Utility
// ───────────────────────────────
SDL_Texture* Inventory::loadTexture(const std::string& path) {
    SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());
    if (!tex)
        std::cerr << "Не удалось загрузить: " << path << std::endl;
    else
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
    return tex;
}

// ───────────────────────────────
//         Add New Item
// ───────────────────────────────
void Inventory::addItem(const std::string& name, const std::string& path) {
    if (items.size() >= slots.size()) return;

    SDL_Texture* tex = loadTexture(path);
    if (!tex) return;

    SDL_FRect rect = slots[items.size()];
    items.push_back({ name, tex, rect });
}

// ───────────────────────────────
//            Render
// ───────────────────────────────
void Inventory::render() {
    // Фон инвентаря
    if (background)
        SDL_RenderTexture(renderer, background, nullptr, &inventoryRect);

    float mx_int, my_int;
    SDL_GetMouseState(&mx_int, &my_int);
    float mx = static_cast<float>(mx_int);
    float my = static_cast<float>(my_int);

    // Подсветка слота
    for (const auto& slot : slots) {
        if (slot.x == previewRect.x && slot.y == previewRect.y &&
            slot.w == previewRect.w && slot.h == previewRect.h)
            continue;

        if (mx > slot.x && mx < slot.x + slot.w &&
            my > slot.y && my < slot.y + slot.h) {
            if (slotHighlight) {
                SDL_FRect outlineRect = {
                    slot.x - 4, slot.y - 4,
                    slot.w + 8, slot.h + 8
                };
                SDL_RenderTexture(renderer, slotHighlight, nullptr, &outlineRect);
            }
            break;
        }
    }

    // Превью персонажа
    if (previewAnim.texture) {
        SDL_FRect src = {
            previewHandler.getCurrentFrame() * 48, 0, 48, 48
        };
        previewHandler.update(previewAnim, src, 48);

        SDL_FRect dst = {
            previewRect.x,
            previewRect.y - 30,
            previewRect.w + 80,
            previewRect.h + 30
        };
        SDL_RenderTexture(renderer, previewAnim.texture, &src, &dst);
    }

    // Предметы
    for (const auto& item : items) {
        if (&item == draggingItem) continue;
        SDL_RenderTexture(renderer, item.texture, nullptr, &item.rect);
    }

    if (draggingItem)
        SDL_RenderTexture(renderer, draggingItem->texture, nullptr, &draggingItem->rect);
}

// ───────────────────────────────
//        Input Handling
// ───────────────────────────────
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
            // Поиск ближайшего слота
            SDL_FRect* closestSlot = nullptr;
            float minDist = std::numeric_limits<float>::max();
        }
    }
}