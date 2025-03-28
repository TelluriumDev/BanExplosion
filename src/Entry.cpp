#include "Entry.h"

#include <ila/event/minecraft/world/ExplosionEvent.h>

#include <ll/api/Config.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/memory/Hook.h>
#include <ll/api/mod/RegisterHelper.h>
#include <ll/api/service/Bedrock.h>

#include <mc/deps/core/math/Vec3.h>
#include <mc/legacy/ActorUniqueID.h> // IWYU pragma: keep
#include <mc/world/actor/player/Player.h>
#include <mc/world/level/BlockPos.h>
#include <mc/world/level/Level.h>
#include <mc/world/level/block/BedBlock.h>
#include <mc/world/level/block/Block.h>
#include <mc/world/level/block/RespawnAnchorBlock.h>
#include <mc/world/level/dimension/Dimension.h>


namespace BanExplosion {

thread_local std::string mTypeName;

LL_TYPE_INSTANCE_HOOK( // NOLINT
    BedBlockUseHook,
    HookPriority::Normal,
    BedBlock,
    &BedBlock::$use,
    bool,
    Player&         player,
    BlockPos const& pos,
    uchar           face
) {
    mTypeName         = getTypeName();
    const auto result = origin(player, pos, face);
    mTypeName         = "";
    return result;
}

LL_TYPE_STATIC_HOOK( // NOLINT
    RespawnAnchorBlockExplodeHook,
    HookPriority::Normal,
    RespawnAnchorBlock,
    &RespawnAnchorBlock::_explode,
    void,
    Player&         player,
    BlockPos const& pos,
    BlockSource&    region,
    Level&          level
) {
    mTypeName = region.getBlock(pos).getTypeName();
    origin(player, pos, region, level);
    mTypeName = "";
}

Entry& Entry::getInstance() {
    static Entry instance;
    return instance;
}

bool Entry::load() {
    auto const& path = getSelf().getConfigDir() / "config.json";
    try {
        ll::config::loadConfig(getConfig(), path);
    } catch (...) {}
    ll::config::saveConfig(getConfig(), path);
    return true;
}

bool Entry::enable() {
    mListener = ll::event::EventBus::getInstance().emplaceListener<ila::mc::ExplosionBeforeEvent>(
        [this](ila::mc::ExplosionBeforeEvent& event) -> void {
            auto& explosion = event.explosion();
            auto  typeName  = mTypeName;
            if (typeName.empty()) {
                const auto* actor = ll::service::getLevel()->fetchEntity(explosion.mSourceID, false);
                typeName          = actor == nullptr ? event.blockSource().getBlock(explosion.mPos.get()).getTypeName()
                                                     : actor->getTypeName();
            }
            auto setting = getConfig().explosionSetting.contains(typeName) ? getConfig().explosionSetting[typeName]
                                                                           : getConfig().defaultSetting;
            if (getSelf().getLogger().getLevel() >= ll::io::LogLevel::Debug) {
                getSelf().getLogger().debug(
                    "Explosion: {0} at {1}(DimId: {2}), setting: {3}",
                    typeName,
                    explosion.mPos->toString(),
                    explosion.mRegion.getDimension().mName.get(),
                    ll::reflection::serialize<nlohmann::ordered_json>(setting).value().dump(4)
                );
            }
            if (!setting.allowExplosion) return event.cancel();
            if (!setting.allowDestroy) explosion.mBreaking = false;
            if (!setting.allowFire) explosion.mFire = false;
            if (setting.maxRadius < explosion.mRadius) explosion.mRadius = setting.maxRadius;
        }
    );
    ll::memory::HookRegistrar<BedBlockUseHook, RespawnAnchorBlockExplodeHook>::hook();
    return true;
}

bool Entry::disable() /* NOLINT */ {
    ll::event::EventBus::getInstance().removeListener(mListener);
    ll::memory::HookRegistrar<BedBlockUseHook, RespawnAnchorBlockExplodeHook>::unhook();
    return true;
}

bool Entry::unload() /* NOLINT */ { return true; }

} // namespace BanExplosion

LL_REGISTER_MOD(BanExplosion::Entry, BanExplosion::Entry::getInstance());