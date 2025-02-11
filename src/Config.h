#pragma once

#include <string>
#include <unordered_map>

namespace BanExplosion {

struct Config {
    int  version       = 1;
    bool PluginEnabled = true;
    struct ExplosionConfig {
        bool allowExplosion = true;
        bool allowDestroy   = true;
        bool allowFire      = true;
    };
    ExplosionConfig                                  defaultSetting;
    std::unordered_map<std::string, ExplosionConfig> explosionSetting = {
        {"minecraft:tnt",                    {true, false, false}},
        {"minecraft:bed",                    {true, false, false}},
        {"minecraft:respawn_anchor",         {true, false, false}},
        {"minecraft:creeper",                {true, false, false}},
        {"minecraft:tnt_minecart",           {true, false, false}},
        {"minecraft:end_crystal",            {true, false, false}},
        {"minecraft:ender_dragon",           {true, false, false}},
        {"minecraft:wither_skull",           {true, false, false}},
        {"minecraft:wither_skull_dangerous", {true, false, false}},
        {"minecraft:fireball",               {true, false, false}},
        {"minecraft:dragon_fireball",        {true, false, false}},
        {"minecraft:wither",                 {true, false, false}}
    };
};

} // namespace BanExplosion