#pragma once
#include "Config.h"
#include "Entry.h"
#include <ll/api/event/EventBus.h>

namespace BanExplosion {
static Config                 config;
static auto                   logger = &Entry::getInstance().getSelf().getLogger();
static ll::event::ListenerPtr ExplosionBeforeEvent;
} // namespace BanExplosion