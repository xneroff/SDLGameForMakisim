//#pragma once
//#include "Skill.h"
//#include "Player.h"
//#include "Camera.h"
//
//class SunBeamSkill : public Skill {
//public:
//    SunBeamSkill();
//
//    void activate(Player* player) override;
//    void update(Player* player, float deltaTime) override;
//    void render(SDL_Renderer* renderer, Camera* camera) override;
//
//private:
//    bool active = false;
//    float beamHeight = 0.0f;
//    float maxBeamHeight = 0.0f;
//    float beamWidth = 30.0f;
//    float beamX = 0.0f;
//
//    Uint64 lastUpdate = 0;
//    SDL_FRect beamRect = { 0.0f, 0.0f, 0.0f, 0.0f };
//    SDL_Color beamColor = { 255, 165, 0, 220 };
//
//    float cooldown = 3000.0f;
//    Uint64 lastActivationTime = 0;
//};
