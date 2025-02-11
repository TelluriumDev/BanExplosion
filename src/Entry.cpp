#include "Entry.h"
#include "BanExplosion.h"

#include "Global.h"
#include "ll/api/Config.h"
#include "ll/api/mod/RegisterHelper.h"

namespace BanExplosion {


Entry& Entry::getInstance() {
    static Entry instance;
    return instance;
}

bool Entry::load() const {
    // Read config
    logger->info("Loading configurations...");
    const auto& configFilePath = getSelf().getConfigDir() / "config.json";
    ll::config::loadConfig(config, configFilePath);
    if (!ll::config::loadConfig(config, configFilePath)) {
        logger->warn("Cannot load configurations from {}", configFilePath);
        logger->info("Saving default configurations...");
        if (!ll::config::saveConfig(config, configFilePath)) {
            logger->error("Failed to save default configurations to {}", configFilePath);
        }
    }
    // Check if the plugin is enabled
    if (!config.PluginEnabled) {
        logger->warn("This plugin has been disabled in the configuration file.");
        logger->warn("This plugin will not work until you enable it in the configuration file.");
        return false;
    }

    return true;
}

bool Entry::enable() {
    // Setup Hooks
    SetupHooks(
        !config.explosionSetting["minecraft:bed"].allowExplosion,
        !config.explosionSetting["minecraft:respawn_anchor"].allowExplosion
    );
    logger->info("Setting up listeners...");
    ListenExplosion();
    return true;
}

bool Entry::disable() {
    logger->info("Removing listeners...");
    ll::event::EventBus::getInstance().removeListener(ExplosionBeforeEvent);
    UnHook(
        !config.explosionSetting["minecraft:bed"].allowExplosion,
        !config.explosionSetting["minecraft:respawn_anchor"].allowExplosion
    );
    return true;
}

bool Entry::unload() { return true; }

} // namespace BanExplosion

LL_REGISTER_MOD(BanExplosion::Entry, BanExplosion::Entry::getInstance());
