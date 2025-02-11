#include "Entry.h"
#include <ila/event/minecraft/world/ExplosionEvent.h>
#include <ll/api/Config.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/memory/Hook.h>
#include <ll/api/mod/RegisterHelper.h>
#include <ll/api/service/Bedrock.h>
#include <mc/common/ActorUniqueID.h>
#include <mc/deps/core/string/HashedString.h>
#include <mc/world/actor/Actor.h>
#include <mc/world/actor/player/Player.h>
#include <mc/world/level/Level.h>
#include <mc/world/level/block/BedBlock.h>
#include <mc/world/level/block/RespawnAnchorBlock.h>
#include <mc/world/level/block/VanillaBlockTypeIds.h>
#include <mc/world/level/dimension/Dimension.h>

namespace BanExplosion {

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
    if (!player.getDimension().mayRespawnViaBed()) {
        return false;
    }
    return origin(player, pos, face);
}

LL_TYPE_STATIC_HOOK(RespawnAnchorBlockExplodeHook, HookPriority::Normal, RespawnAnchorBlock, &RespawnAnchorBlock::_explode, void, Player&, BlockPos const&, BlockSource&, Level&) {
}

Entry& Entry::getInstance() {
    static Entry instance;
    return instance;
}

bool Entry::load() const {
    auto const& path = getSelf().getConfigDir() / "config.json";
    try {
        ll::config::loadConfig(mConfig, path);
    } catch (...) {}
    ll::config::saveConfig(mConfig, path);
    return true;
}

bool Entry::enable() {
    mListener = ll::event::EventBus::getInstance().emplaceListener<ila::mc::ExplosionBeforeEvent>(
        [this](ila::mc::ExplosionBeforeEvent& event) -> void {
            auto& explosion = event.getExplosion();
            if (auto* actor = ll::service::getLevel()->fetchEntity(explosion.mSourceID, false)) {
                auto setting = getConfig().explosionSetting.contains(actor->getTypeName())
                                 ? getConfig().explosionSetting[actor->getTypeName()]
                                 : getConfig().defaultSetting;
                if (!setting.allowExplosion) return event.cancel();
                if (!setting.allowDestroy) explosion.setBreaking(false);
                if (!setting.allowFire) explosion.setFire(false);
            }
        }
    );
    if (getConfig().explosionSetting.contains(VanillaBlockTypeIds::Bed())
        && !getConfig().explosionSetting[VanillaBlockTypeIds::Bed()].allowExplosion) {
        BedBlockUseHook::hook();
    }
    if (getConfig().explosionSetting.contains(VanillaBlockTypeIds::RespawnAnchor())
        && !getConfig().explosionSetting[VanillaBlockTypeIds::RespawnAnchor()].allowExplosion) {
        RespawnAnchorBlockExplodeHook::hook();
    }
    return true;
}

bool Entry::disable() {
    ll::event::EventBus::getInstance().removeListener(mListener);
    ll::memory::HookRegistrar<BedBlockUseHook, RespawnAnchorBlockExplodeHook>().unhook();
    return true;
}

bool Entry::unload() { return true; }

} // namespace BanExplosion

LL_REGISTER_MOD(BanExplosion::Entry, BanExplosion::Entry::getInstance());