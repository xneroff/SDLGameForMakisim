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
struct MapLabel {
    std::string text;
    SDL_FRect rect;

};
struct MapLayer {
    std::string name;
    std::vector<int> data;
};

struct Portal {
    SDL_FRect rect;
    std::string targetMap;
    std::string targetSpawn;
};



class TileMap { 
public:
    TileMap(SDL_Renderer* renderer);
    ~TileMap();
    const std::vector<MapLabel>& getLabels() const { return labels; }
    bool loadFromFile(const std::string& path);
    void renderLayer(SDL_Renderer* renderer, Camera* camera, const std::string& name);
    std::vector<SDL_FPoint> getNPCSpawnPoints() const;
    SDL_FPoint getNPCSpawn(const std::string& name) const;
    void renderCollisions(SDL_Renderer* renderer, Camera* camera);
    SDL_FPoint getGenericSpawnPointByName(const std::string& name) const;

    SDL_FPoint getSpawnPoint() const;
    const std::vector<SDL_FRect>& getCollisionRects() const;

    int getMapWidth() const { return mapWidth; }
    int getMapHeight() const { return mapHeight; }
    const std::vector<Chest>& getChests() const { return chests; }
    std::vector<Chest>& getChestsMutable() { return chests; } // если нужен доступ для изменений
    const std::vector<Portal>& getPortals() const { return portals; }

    int getTileWidth() const { return tileWidth; }
    int getTileHeight() const { return tileHeight; }
    const std::vector<SDL_FRect>& getTraps() const { return traps; }

private:
    std::vector<MapLabel> labels;
    std::vector<const Tileset*> tileIDToTileset;
    std::vector<Portal> portals;
    SDL_FPoint spawnPoint{ 0, 0 };
    std::map<std::string, SDL_FPoint> npcSpawnPoints;
    void loadPortals(const nlohmann::json& mapJson);
    std::unordered_map<int, const Tileset*> tileToTileset;

    std::vector<Chest> chests;
    SDL_Renderer* renderer;
    std::vector<Tileset> tilesets;
    std::vector<MapLayer> layers;
    std::vector<SDL_FRect> collisionRects;
    std::vector<SDL_FRect> traps;
    nlohmann::json mapJson;

    int tileWidth = 0, tileHeight = 0, mapWidth = 350, mapHeight = 100;

    void loadTilesets(const std::string& folder, const nlohmann::json& tilesetsJson);
    void loadCollisions(const nlohmann::json& layersJson);
};