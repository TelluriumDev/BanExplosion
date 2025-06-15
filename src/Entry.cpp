#include "Entry.h"

#include <ll/api/Config.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/memory/Hook.h>
#include <ll/api/mod/RegisterHelper.h>
#include <ll/api/service/Bedrock.h>

#include <mc/deps/core/math/Vec3.h>
#include <mc/world/actor/player/Player.h>
#include <mc/world/level/BlockPos.h>
#include <mc/world/level/BlockSource.h>
#include <mc/world/level/Explosion.h>
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
    &BedBlock::use,
    void,
    BlockEvents::BlockPlayerInteractEvent& eventData
) {
    mTypeName = getTypeName();
    origin(eventData);
    mTypeName = "";
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

LL_TYPE_INSTANCE_HOOK( // NOLINT
    ExplodeHook, HookPriority::Normal, Explosion, &Explosion::explode, bool) {
    auto& typeName = mTypeName;
    if (typeName.empty()) {
        const auto* actor = ll::service::getLevel()->fetchEntity(mSourceID, false);
        typeName          = actor == nullptr ? mRegion.getBlock(mPos.get()).getTypeName() : actor->getTypeName();
    }
    const auto& config = Entry::getInstance().getConfig();
    const auto& logger = Entry::getInstance().getSelf().getLogger();
    const auto& setting =
        config.explosionSetting.contains(typeName) ? config.explosionSetting.at(typeName) : config.defaultSetting;
    if (logger.getLevel() >= ll::io::LogLevel::Debug) {
        logger.debug(
            "Explosion: {0} at {1}(DimId: {2}), setting: {3}",
            typeName,
            mPos->toString(),
            mRegion.getDimension().mName.get(),
            ll::reflection::serialize<nlohmann::ordered_json>(setting).value().dump(4)
        );
    }
    if (!setting.allowExplosion) return false;
    if (!setting.allowDestroy) mBreaking = false;
    if (!setting.allowFire) mFire = false;
    if (setting.maxRadius < mRadius) mRadius = setting.maxRadius;
    return origin();
}

using Hooks = ll::memory::HookRegistrar<BedBlockUseHook, RespawnAnchorBlockExplodeHook, ExplodeHook>;

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

bool Entry::enable() /* NOLINT */ {
    Hooks::hook();
    return true;
}

bool Entry::disable() /* NOLINT */ {
    Hooks::unhook();
    return true;
}

bool Entry::unload() /* NOLINT */ { return true; }

} // namespace BanExplosion

LL_REGISTER_MOD(BanExplosion::Entry, BanExplosion::Entry::getInstance());