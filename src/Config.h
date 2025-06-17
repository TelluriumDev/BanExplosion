#pragma once
#include <ll/api/base/Containers.h>

#include <cfloat>
#include <string>

namespace BanExplosion {

struct ConfigV1 {
    struct ExplosionConfig {
        bool  allowExplosion = true;
        float maxRadius      = FLT_MAX;
        bool  allowDestroy   = false;
        bool  allowFire      = false;
    };

    int                                        version = 1;
    ExplosionConfig                            defaultSetting;
    ll::DenseMap<std::string, ExplosionConfig> explosionSetting;
};

struct ConfigV2 {
    struct ExplosionConfig {
        bool  AllowExplosion = true;
        bool  AllowDestroy   = false;
        bool  AllowFire      = false;
        float MaxRadius      = FLT_MAX;
    };

    struct ExplosionDimensionSettings {
        ExplosionConfig                            DefaultSetting;
        ll::DenseMap<std::string, ExplosionConfig> CustomSettings;
    };

    int version = 2;

    ExplosionConfig GlobalSetting;
    ll::DenseMap<std::string, ExplosionDimensionSettings> ExplosionSetting;
};

using Config = ConfigV2;

} // namespace BanExplosion