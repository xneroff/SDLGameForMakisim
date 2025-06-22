//#include "SunBeamSkill.h"
//
//SunBeamSkill::SunBeamSkill() {}
//
//void SunBeamSkill::activate(Player* player) {
//    Uint64 now = SDL_GetTicks();
//    if (active || (now - lastActivationTime < static_cast<Uint64>(cooldown)))
//        return;
//
//    active = true;
//    beamHeight = 0;
//
//    SDL_FRect playerRect = player->gedDest();
//    maxBeamHeight = playerRect.y + playerRect.h;
//
//    float direction = static_cast<float>(player->getDirection());
//    float x = playerRect.x + playerRect.w / 2.0f + direction * (playerRect.w / 2.0f + 100);
//    beamX = x - beamWidth / 2.0f;
//
//    beamRect = { beamX, 0, beamWidth, beamHeight };
//
//    lastUpdate = now;
//    lastActivationTime = now;
//}
//
//void SunBeamSkill::update(Player* player, float deltaTime) {
//    if (!active) return;
//
//    float speed = 900000.0f; // пикселей в секунду — увеличил скорость
//    float seconds = deltaTime * 0.001f; // перевод из миллисекунд в секунды
//
//    beamHeight += speed * seconds;
//
//    beamRect.x = beamX;
//    beamRect.y = 0;
//    beamRect.w = beamWidth;
//    beamRect.h = beamHeight;
//
//    if (beamHeight >= maxBeamHeight) {
//        beamHeight = maxBeamHeight;
//        active = false;
//    }
//}
//
//
//void SunBeamSkill::render(SDL_Renderer* renderer, Camera* camera) {
//    if (!active) return;
//
//    SDL_FRect screenRect = camera->apply(beamRect);
//    SDL_SetRenderDrawColor(renderer, beamColor.r, beamColor.g, beamColor.b, beamColor.a);
//    SDL_RenderFillRect(renderer, &screenRect);
//}
