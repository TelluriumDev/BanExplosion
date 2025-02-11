#pragma once

#include "ll/api/mod/NativeMod.h"

namespace BanExplosion {

class Entry {

public:
    static Entry& getInstance();

    Entry() : mSelf(*ll::mod::NativeMod::current()) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    /// @return True if the mod is loaded successfully.
    bool load() const;

    /// @return True if the mod is enabled successfully.
    static bool enable();

    /// @return True if the mod is disabled successfully.
    static bool disable();

    // TODO: Implement this method if you need to unload the mod.
    // /// @return True if the mod is unloaded successfully.
    static bool unload();

private:
    ll::mod::NativeMod& mSelf;
};

} // namespace my_mod
