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
        "Explosion: {0} at {1} (DimId: {2}), setting: {3}",
        typeName,
        pos,
        dim,
        ll::reflection::serialize<nlohmann::ordered_json>(setting).value().dump(4)
    );
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
    const std::string_view& typeName = "minecraft:wither";
    const auto&             config   = Entry::getInstance().getConfig();
    const auto&             logger   = Entry::getInstance().getSelf().getLogger();
    const auto&             setting =
        config.explosionSetting.contains(typeName.data())
                        ? config.explosionSetting.at(std::unordered_map<std::string, Config::ExplosionConfig>::key_type(typeName))
                        : config.defaultSetting;

    if (logger.getLevel() >= ll::io::LogLevel::Debug) {
        logExplosionInfo(typeName, bb.toString(), region.getDimension().mName.get(), setting);
    }
    if (!setting.allowExplosion || !setting.allowDestroy) return;
    if (setting.maxRadius < range) range = static_cast<int>(setting.maxRadius);
    return origin(level, bb, region, range, attackType);
}

LL_TYPE_INSTANCE_HOOK( // NOLINT
    ExplodeHook, HookPriority::Normal, Explosion, &Explosion::explode, bool) {
    const auto& config   = Entry::getInstance().getConfig();
    const auto& logger   = Entry::getInstance().getSelf().getLogger();
    auto&       typeName = mTypeName;
    const auto& setting =
        config.explosionSetting.contains(typeName) ? config.explosionSetting.at(typeName) : config.defaultSetting;

    if (typeName.empty()) {
        const auto* actor = mRegion.getDimension().fetchEntity(mSourceID.get(), false);
        typeName          = actor == nullptr ? mRegion.getBlock(mPos.get()).getTypeName() : actor->getTypeName();
    }

    if (logger.getLevel() >= ll::io::LogLevel::Debug) {
        logExplosionInfo(typeName, mPos->toString(), mRegion.getDimension().mName.get(), setting);
    }

    if (!setting.allowExplosion) return false;
    if (!setting.allowDestroy) mBreaking = false;
    if (!setting.allowFire) mFire = false;
    if (setting.maxRadius < mRadius) mRadius = setting.maxRadius;
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