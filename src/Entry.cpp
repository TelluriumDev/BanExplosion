#include "Entry.h"

#include <ll/api/Config.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/memory/Hook.h>
#include <ll/api/mod/RegisterHelper.h>

#include <mc/deps/core/math/Vec3.h>
#include <mc/world/actor/boss/WitherBoss.h>
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

namespace {

void logExplosionInfo(
    const std::string_view&        typeName,
    const std::string_view&        pos,
    const std::string_view&        dim,
    const Config::ExplosionConfig& setting
) {
    Entry::getInstance().getSelf().getLogger().debug(
        "Explosion: {0} at {1} (Dimension: {2}), setting: {3}",
        typeName,
        pos,
        dim,
        ll::reflection::serialize<nlohmann::ordered_json>(setting).value().dump(4)
    );
}

Config::ExplosionConfig
getExplosionConfig(const Config& config, const std::string& dimName, const std::string& typeName) {
    if (const auto it = config.ExplosionSetting.find(dimName); it != config.ExplosionSetting.end()) {
        if (const auto customIt = it->second.CustomSettings.find(typeName);
            customIt != it->second.CustomSettings.end()) {
            return customIt->second;
        }
        return it->second.DefaultSetting;
    }
    return config.GlobalSetting;
}

} // namespace

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
    WitherHurtExplodeHook,
    HookPriority::Normal,
    WitherBoss,
    &WitherBoss::_destroyBlocks,
    void,
    Level&                       level,
    AABB const&                  bb,
    BlockSource&                 region,
    int                          range,
    WitherBoss::WitherAttackType attackType
) {
    const std::string& typeName = "minecraft:wither";
    const auto&        config   = Entry::getInstance().getConfig();
    const auto&        logger   = Entry::getInstance().getSelf().getLogger();
    const auto&        dimName  = region.getDimension().mName.get();
    const auto&        setting  = getExplosionConfig(config, dimName, typeName);

    if (logger.getLevel() >= ll::io::LogLevel::Debug) {
        logExplosionInfo(typeName, bb.toString(), dimName, setting);
    }

    if (!setting.AllowExplosion || !setting.AllowDestroy) return;
    range = std::min(range, static_cast<int>(setting.MaxRadius));
    return origin(level, bb, region, range, attackType);
}


LL_TYPE_INSTANCE_HOOK(ExplodeHook, HookPriority::Normal, Explosion, &Explosion::explode, bool) {
    const auto& config  = Entry::getInstance().getConfig();
    const auto& logger  = Entry::getInstance().getSelf().getLogger();
    const auto& dimName = mRegion.getDimension().mName.get();

    const std::string& typeName = [&]() -> std::string {
        if (!mTypeName.empty()) return mTypeName;
        const auto* actor = mRegion.getDimension().fetchEntity(mSourceID.get(), false);
        return actor ? actor->getTypeName() : mRegion.getBlock(mPos.get()).getTypeName();
    }();

    const auto setting = getExplosionConfig(config, dimName, typeName);

    if (logger.getLevel() >= ll::io::LogLevel::Debug) {
        logExplosionInfo(typeName, mPos->toString(), dimName, setting);
    }

    if (!setting.AllowExplosion) return false;
    mBreaking &= setting.AllowDestroy;
    mFire     &= setting.AllowFire;
    mRadius    = std::min(mRadius, setting.MaxRadius);

    return origin();
}


using Hooks =
    ll::memory::HookRegistrar<BedBlockUseHook, RespawnAnchorBlockExplodeHook, WitherHurtExplodeHook, ExplodeHook>;

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