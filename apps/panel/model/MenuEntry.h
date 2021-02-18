#pragma once

#include <libgraphic/Icon.h>
#include <libjson/Json.h>
#include <libutils/String.h>
#include <libutils/Vector.h>

namespace panel
{

struct MenuEntry
{
    String name;
    String comment;
    RefPtr<Icon> icon;
    String command;

    MenuEntry(const Json::Value &value);

    static Vector<MenuEntry> load();
};

} // namespace panel
