#pragma once
#include <SDL3/SDL.h>
#include <json.hpp>
#include <vector>
#include <string>
#include "Camera.h"
#include "Chest.h"
struct Tileset {
    int firstgid;
    int columns;
    int tileWidth, tileHeight;
    SDL_Texture* texture;
};

struct MapLayer {
    std::string name;
    std::vector<int> data;
};

class TileMap { 
public:
    TileMap(SDL_Renderer* renderer);
    ~TileMap();

    bool loadFromFile(const std::string& path);
    void renderLayer(SDL_Renderer* renderer, Camera* camera, const std::string& name);
    SDL_FPoint getSpawnPoint() const;
    const std::vector<SDL_FRect>& getCollisionRects() const;

    int getMapWidth() const { return mapWidth; }
    int getMapHeight() const { return mapHeight; }
    const std::vector<Chest>& getChests() const { return chests; }
    std::vector<Chest>& getChestsMutable() { return chests; } // если нужен доступ для изменений

    int getTileWidth() const { return tileWidth; }
    int getTileHeight() const { return tileHeight; }

private:

    SDL_FPoint spawnPoint{ 0, 0 };
    std::vector<Chest> chests;
    SDL_Renderer* renderer;
    std::vector<Tileset> tilesets;
    std::vector<MapLayer> layers;
    std::vector<SDL_FRect> collisionRects;

    int tileWidth = 0, tileHeight = 0, mapWidth = 200, mapHeight = 50;

    void loadTilesets(const std::string& folder, const nlohmann::json& tilesetsJson);
    void loadCollisions(const nlohmann::json& layersJson);
};