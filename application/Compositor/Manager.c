#include <libsystem/logger.h>

#include "Compositor/Manager.h"
#include "Compositor/Renderer.h"
#include "Compositor/Window.h"

static List *_managed_windows;

void manager_initialize(void)
{
    _managed_windows = list_create();
}

List *manager_get_windows(void)
{
    return _managed_windows;
}

Window *manager_get_window(struct Client *client, int id)
{
    list_foreach(Window, window, _managed_windows)
    {
        if (window->client == client && window->id == id)
        {
            return window;
        }
    }

    return NULL;
}

Window *manager_get_window_at(Point position)
{
    list_foreach(Window, window, _managed_windows)
    {
        if (rectangle_containe_point(window_bound(window), position))
        {
            return window;
        }
    }

    return NULL;
}

void manager_register_window(Window *window)
{
    manager_set_focus_window(window);
    renderer_region_dirty(window_bound(window));
}

void manager_unregister_window(Window *window)
{
    renderer_region_dirty(window_bound(window));
    list_remove(_managed_windows, window);
}

void manager_set_focus_window(Window *window)
{
    Window *old_focus = manager_focus_window();

    if (old_focus)
    {
        window_lost_focus(old_focus);
    }

    list_remove(_managed_windows, window);
    list_push(_managed_windows, window);

    window_get_focus(window);
}

struct Window *manager_focus_window(void)
{
    Window *result = NULL;

    list_peek(_managed_windows, (void **)&result);

    return result;
}