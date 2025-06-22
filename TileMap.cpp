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
    std::string folder = path.substr(0, path.find_last_of("/\\") + 1);
    loadTilesets(folder, j["tilesets"]);

    std::cout << "Map loaded successfully!\n";
    return true;
}


void TileMap::renderLayer(SDL_Renderer* renderer, Camera* camera, const std::string& name) {
    for (const auto& layer : layers) {
        if (layer.name != name) continue;
        for (int y = 0; y < mapHeight; ++y) {
            for (int x = 0; x < mapWidth; ++x) {
                int tileID = layer.data[y * mapWidth + x];
                if (tileID == 0) continue;

                const Tileset* ts = nullptr;
                for (const auto& tileset : tilesets)
                    if (tileID >= tileset.firstgid) ts = &tileset;

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
        }
    }

    std::cout << "✅ Total collisions loaded: " << collisionRects.size() << std::endl;
    std::cout << "✅ Total chests loaded: " << chests.size() << std::endl;
}




const std::vector<SDL_FRect>& TileMap::getCollisionRects() const { return collisionRects; }

SDL_FPoint TileMap::getSpawnPoint() const {
    return spawnPoint;
}