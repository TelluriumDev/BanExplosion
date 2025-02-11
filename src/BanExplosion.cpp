#include "BanExplosion.h"
#include "Global.h"
#include <ila/event/minecraft/world/ExplosionEvent.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/memory/Hook.h>
#include <ll/api/service/Bedrock.h>
#include <mc/common/ActorUniqueID.h>
#include <mc/world/actor/Actor.h>
#include <mc/world/actor/player/Player.h>
#include <mc/world/level/BlockPos.h>
#include <mc/world/level/Level.h>
#include <mc/world/level/block/BedBlock.h>
#include <mc/world/level/block/RespawnAnchorBlock.h>
#include <mc/world/level/dimension/VanillaDimensions.h>


namespace BanExplosion {

void ListenExplosion() {
    ExplosionBeforeEvent = ll::event::EventBus::getInstance().emplaceListener<ila::mc::world::ExplosionBeforeEvent>(
        [](ila::mc::ExplosionBeforeEvent& ev) {
            // Powered By 啥也不会的子沐呀
            auto&       Explosion = ev.getExplosion();
            const auto* Actor     = ll::service::getLevel()->fetchEntity(Explosion.mSourceID, false);
            if (Actor == nullptr) return;
            auto& TypeName = Actor->getTypeName();
            auto& [allowExplosion, allowDestroy, allowFire] =
                config.explosionSetting.contains(TypeName) ? config.explosionSetting[TypeName] : config.defaultSetting;
            if (!allowExplosion) return ev.cancel();
            if (!allowDestroy) Explosion.setBreaking(false);
            if (!allowFire) Explosion.setFire(false);
        }

    );
}

LL_TYPE_INSTANCE_HOOK(
    BedBlockUseHook,
    HookPriority::Normal,
    BedBlock,
    &BedBlock::$use,
    bool,
    Player&         player,
    BlockPos const& pos,
    uchar           face
) {
    // 床会在非主世界的地方自爆
    // 感觉多维度在这里会爆炸，先这样写吧
    if (player.getDimensionId() != VanillaDimensions::Overworld()) {
        return false;
    }
    return origin(player, pos, face);
}

LL_TYPE_INSTANCE_HOOK(
    RespawnAnchorBlockUseHook,
    HookPriority::Normal,
    RespawnAnchorBlock,
    &RespawnAnchorBlock::$use,
    bool,
    Player&         player,
    BlockPos const& pos,
    uchar           face
) {
    // 重生锚在非地狱的任何地方都会爆炸
    if (player.getDimensionId() != VanillaDimensions::Nether()) {
        return false;
    }
    return origin(player, pos, face);
}

void SetupHooks(const bool BedBlockHook, const bool RespawnAnchorBlockUseHook) {
    logger->info("Setting up Hooks...");
    if (BedBlockHook) BanExplosion::BedBlockUseHook::hook();
    if (RespawnAnchorBlockUseHook) BanExplosion::RespawnAnchorBlockUseHook::hook();
}

void UnHook(const bool BedBlockHook, const bool RespawnAnchorBlockUseHook) {
    logger->info("UnHooking...");
    if (BedBlockHook) BanExplosion::BedBlockUseHook::unhook();
    if (RespawnAnchorBlockUseHook) BanExplosion::RespawnAnchorBlockUseHook::unhook();
}
} // namespace BanExplosion