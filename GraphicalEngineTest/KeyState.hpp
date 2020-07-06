#pragma once

enum class KeyState
{
    /// <summary>
    /// key hasn't changed
    /// </summary>
    None = 0,

    /// <summary>
    /// Key was pressed or held and then released
    /// </summary>
    Released = 1,

    /// <summary>
    /// Key was pressed down
    /// </summary>
    Pressed = 2,

    /// <summary>
    /// Key is being held
    /// </summary>
    Held = 3,
};