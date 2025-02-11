#include "Config.h"
#include <ll/api/event/ListenerBase.h>
#include <ll/api/mod/NativeMod.h>

namespace BanExplosion {

class Entry {

public:
    static Entry& getInstance();

    Entry() : mSelf(*ll::mod::NativeMod::current()) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    bool load() const;

    bool enable();

    bool disable();

    bool unload();

    Config& getConfig() { return mConfig; }

private:
    ll::mod::NativeMod&    mSelf;
    Config                 mConfig;
    ll::event::ListenerPtr mListener;
};

} // namespace BanExplosion