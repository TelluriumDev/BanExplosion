#include <cfloat>
#include <string>
#include <unordered_map>

namespace BanExplosion {
struct Config {
public:
    struct ExplosionConfig {
        bool  allowExplosion = true;
        float maxRadius      = FLT_MAX;
        bool  allowDestroy   = false;
        bool  allowFire      = false;
    };

public:
    int                                              version = 1;
    ExplosionConfig                                  defaultSetting;
    std::unordered_map<std::string, ExplosionConfig> explosionSetting;
};
} // namespace BanExplosion