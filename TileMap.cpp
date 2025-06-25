#include "TileMap.h"
#include <fstream>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include "Camera.h"
#include "Chest.h"

using json = nlohmann::json;

TileMap::TileMap(SDL_Renderer* rend) : renderer(rend) {}
TileMap::~TileMap() { for (auto& ts : tilesets) SDL_DestroyTexture(ts.texture); }

bool TileMap::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Ошибка загрузки карты: " << path << std::endl;
        return false;
    }
    std::cout << "Файл карты загружен успешно: " << path << std::endl;

    json j;
    file >> j;
    mapJson = j;

    // Checking main map parameters
    std::cout << "Loading map: " << path << std::endl;
    std::cout << "Tile size: " << j["tilewidth"] << "x" << j["tileheight"] << std::endl;
    std::cout << "Map size: " << j["width"] << "x" << j["height"] << std::endl;

    tileWidth = j["tilewidth"];
    tileHeight = j["tileheight"];
    mapWidth = j["width"];
    mapHeight = j["height"];

    for (const auto& layer : j["layers"]) {
        if (layer["type"] == "tilelayer") {
            std::cout << "Loaded tile layer: " << layer["name"] << std::endl;
            layers.push_back({ layer["name"], layer["data"].get<std::vector<int>>() });
        }
    }

    loadCollisions(j["layers"]);
    loadPortals(j);

    std::string folder = path.substr(0, path.find_last_of("/\\") + 1);
    loadTilesets(folder, j["tilesets"]);
    std::cout
        << "Tile size: " << tileWidth << "x" << tileHeight << "\n"
        << "Map size:  " << mapWidth << "x" << mapHeight
        << " tiles → " << (mapWidth * tileWidth) << "×" << (mapHeight * tileHeight) << " px\n";
    for (auto& r : collisionRects) {
        std::cout << "Collision: x=" << r.x << "…" << r.x + r.w
            << " y=" << r.y << "…" << r.y + r.h << "\n";
    }

    std::cout << "Map loaded successfully!\n";
    return true;
}


void TileMap::renderLayer(SDL_Renderer* renderer, Camera* camera, const std::string& name) {
    for (const auto& layer : layers) {
        if (layer.name != name) continue;

        // Получаем видимую область камеры в тайлах
        SDL_FRect view = camera->getView();
        float camX = view.x;
        float camY = view.y;
        float camW = view.w;
        float camH = view.h;


        int startX = std::max(0, int(camX / tileWidth));
        int startY = std::max(0, int(camY / tileHeight));
        int endX = std::min(mapWidth, int((camX + camW) / tileWidth) + 2); // +2 для плавности по краям
        int endY = std::min(mapHeight, int((camY + camH) / tileHeight) + 2);

        for (int y = startY; y < endY; ++y) {
            for (int x = startX; x < endX; ++x) {
                int tileID = layer.data[y * mapWidth + x];
                if (tileID == 0) continue;

                // Оптимизированный поиск tileset (проходим в обратном порядке)
                const Tileset* ts = nullptr;
                for (auto it = tilesets.rbegin(); it != tilesets.rend(); ++it) {
                    if (tileID >= it->firstgid) {
                        ts = &(*it);
                        break;
                    }
                }
                if (!ts || !ts->texture) continue;

                SDL_FRect src = {
                    float((tileID - ts->firstgid) % ts->columns * tileWidth),
                    float((tileID - ts->firstgid) / ts->columns * tileHeight),
                    float(tileWidth),
                    float(tileHeight)
                };

                SDL_FRect worldDest = {
                    float(x * tileWidth),
                    float(y * tileHeight),
                    float(tileWidth),
                    float(tileHeight)
                };

                SDL_FRect screenDest = camera->apply(worldDest);

                SDL_RenderTexture(renderer, ts->texture, &src, &screenDest);
            }
        }
    }
}




void TileMap::loadTilesets(const std::string& folder, const json& tilesetsJson) {
    for (const auto& entry : tilesetsJson) {
        std::string tsxPath = folder + entry["source"].get<std::string>();
        std::ifstream tsx(tsxPath);

        if (!tsx.is_open()) {
            std::cerr << "Ошибка загрузки tileset: " << tsxPath << std::endl;
            continue;
        }

        std::string xml((std::istreambuf_iterator<char>(tsx)), {});
        std::size_t imgPos = xml.find("image source=\"");
        std::size_t colPos = xml.find("columns=\"");

        if (imgPos == std::string::npos || colPos == std::string::npos) {
            std::cerr << "Ошибка в `tsx`: нет `image source` или `columns`." << std::endl;
            continue;
        }

        std::string imgPath = xml.substr(imgPos + 14);
        imgPath = imgPath.substr(0, imgPath.find("\""));

        int columns = std::stoi(xml.substr(colPos + 9, xml.find("\"", colPos + 9) - (colPos + 9)));

        std::string fullImgPath = folder + imgPath;
        std::cout << "Загрузка изображения тайлсета: " << fullImgPath << std::endl;

        Tileset ts = {
            entry["firstgid"],
            columns,
            32, 32, // tileWidth, tileHeight
            IMG_LoadTexture(renderer, fullImgPath.c_str())
        };
        SDL_SetTextureScaleMode(ts.texture, SDL_SCALEMODE_NEAREST); // DIMAN KRASAVA +REPCHIK

        if (!ts.texture) {
            std::cerr << "Ошибка загрузки текстуры: " << fullImgPath << std::endl;
            continue;
        }

        tilesets.push_back(ts);

    }
}



void TileMap::loadCollisions(const json& layersJson) {
    std::cout << "Loading collisions and chests...\n";

    for (const auto& layer : layersJson) {
        std::string layerName = layer["name"];
        std::string layerType = layer["type"];

        if (layerType == "objectgroup") {
            if (layerName == "Collisions") {
                for (const auto& obj : layer["objects"]) {
           
                    if (obj.contains("name") && obj["name"] == "Spawn") {
                        spawnPoint.x = obj["x"].get<float>();
                        spawnPoint.y = obj["y"].get<float>();

                        std::cout << "Player spawn point: (" << spawnPoint.x << ", " << spawnPoint.y << ")\n";
                    }

                    else {
                        SDL_FRect rect = { obj["x"], obj["y"], obj["width"], obj["height"] };
                        collisionRects.push_back(rect);
                        std::cout << "Added collision box: (" << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << ")\n";
                    }
                }
            }

            else if (layerName == "NPCSpawns") {
                for (const auto& obj : layer["objects"]) {
                    if (obj.contains("name")) {
                        std::string name = obj["name"];
                        SDL_FPoint pos = {
                            obj["x"].get<float>(),
                            obj["y"].get<float>()
                        };
                        npcSpawnPoints[name] = pos;
                        std::cout << "📍 NPC spawn point '" << name << "' at: (" << pos.x << ", " << pos.y << ")\n";
                    }
                }
            }

            // 🔥 Добавляем загрузку сундуков из слоя "Chests"
            else if (layerName == "Chests") {
                for (const auto& obj : layer["objects"]) {
                    Chest chest;
                    chest.rect = {
                        static_cast<float>(obj["x"]),
                        static_cast<float>(obj["y"]),
                        static_cast<float>(obj["width"]),
                        static_cast<float>(obj["height"])
                    };
                    chest.name = obj["name"];

                    if (obj.contains("properties")) {
                        for (const auto& prop : obj["properties"]) {
                            std::string pname = prop["name"];
                            if (pname == "Item") {
                                chest.item = prop["value"];
                            }
                            else if (pname == "amount") {
                                chest.amount = prop["value"];
                            }
                            else if (pname == "Opened" || pname == "opened") {
                                chest.opened = prop["value"];
                            }
                        }
                    }

                    chests.push_back(chest);
                    std::cout << "Loaded chest: " << chest.name << " with item: " << chest.item << " x" << chest.amount << std::endl;
                }
            }
            else if (layerName == "Nadpisi") {
                for (const auto& obj : layer["objects"]) {
                    if (obj.contains("text")) {
                        MapLabel label;
                        label.rect = {
                            static_cast<float>(obj["x"]),
                            static_cast<float>(obj["y"]),
                            static_cast<float>(obj["width"]),
                            static_cast<float>(obj["height"])
                        };
                        label.text = obj["text"]["text"];
                        labels.push_back(label);
                        std::cout << "Loaded map label: " << label.text << std::endl;
                    }
                }
            }
            else if (layerName == "Traps") {
                for (const auto& obj : layer["objects"]) {
                    SDL_FRect trap = {
                        obj["x"].get<float>(),
                        obj["y"].get<float>(),
                        obj["width"].get<float>(),
                        obj["height"].get<float>()
                    };
                    traps.push_back(trap);
                  // ← добавляем как твердые объекты
                    std::cout << "Trap collision added at: (" << trap.x << ", " << trap.y << ")\n";
                }
            }
        }
    }

    std::cout << "✅ Total collisions loaded: " << collisionRects.size() << std::endl;
    std::cout << "✅ Total chests loaded: " << chests.size() << std::endl;
}




const std::vector<SDL_FRect>& TileMap::getCollisionRects() const { return collisionRects; }

SDL_FPoint TileMap::getSpawnPoint() const {
    return spawnPoint;
}

std::vector<SDL_FPoint> TileMap::getNPCSpawnPoints() const {
    std::vector<SDL_FPoint> spawnPoints;

    for (const auto& layer : mapJson["layers"]) {
        if (layer["type"] == "objectgroup" && layer["name"] == "NPCSpawns") {
            for (const auto& obj : layer["objects"]) {
                if (obj.contains("point") && obj["point"].get<bool>()) {
                    spawnPoints.push_back({ obj["x"], obj["y"] });
                }
            }
        }
    }

    return spawnPoints;
}
SDL_FPoint TileMap::getNPCSpawn(const std::string& name) const {
    auto it = npcSpawnPoints.find(name);
    if (it != npcSpawnPoints.end()) return it->second;
    return { 0, 0 }; // по умолчанию
}

void TileMap::renderCollisions(SDL_Renderer* renderer, Camera* camera) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Красные — обычные коллизии
    //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
    //for (const auto& r : collisionRects) {
    //    SDL_FRect view = camera->apply(r); // с учётом зума
    //    SDL_RenderFillRect(renderer, &view);

    //    SDL_RenderFillRect(renderer, &view);
    //}

    // Жёлтые — ловушки, если они отдельно есть
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 100);
    for (const auto& r : traps) {
        SDL_FRect view = {
            r.x - camera->getView().x,
            r.y - camera->getView().y,
            r.w, r.h
        };
        SDL_RenderFillRect(renderer, &view);
    }
}
void TileMap::loadPortals(const json& j) {
    for (const auto& layer : j["layers"]) {
        if (layer["type"] == "objectgroup" && layer["name"] == "Collisions") {
            for (const auto& o : layer["objects"]) {
                if (!o.contains("name")) continue;
                std::string name = o["name"];
                if (name.rfind("To", 0) != 0) continue;

                Portal p;
                p.rect = {      
                    o["x"].get<float>(),
                    o["y"].get<float>(),
                    o["width"].get<float>(),
                    o["height"].get<float>()
                };

                if (o.contains("properties")) {
                    for (const auto& prop : o["properties"]) {
                        if (prop["name"] == "targetMap") p.targetMap = prop["value"];
                        if (prop["name"] == "targetSpawn") p.targetSpawn = prop["value"];
                    }
                }

                portals.push_back(p);
            }
        }
    }
}
SDL_FPoint TileMap::getGenericSpawnPointByName(const std::string& name) const {
    for (const auto& layer : mapJson["layers"]) {
        if (layer["type"] != "objectgroup") continue;

        for (const auto& obj : layer["objects"]) {
            if (obj.contains("name") && obj["name"] == name) {
                return { obj["x"].get<float>(), obj["y"].get<float>() };
            }
        }
    }
    std::cout << "⚠️ Spawn point '" << name << "' not found!\n";
    return { 0, 0 };
}
