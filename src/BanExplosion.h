#pragma once

namespace BanExplosion {
void ListenExplosion();
void SetupHooks(const bool BedBlockHook, const bool RespawnAnchorBlockUseHook);
void UnHook(const bool BedBlockHook, const bool RespawnAnchorBlockUseHook);
} // namespace BanExplosion