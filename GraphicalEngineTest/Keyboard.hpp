#pragma once

#include "KeyState.hpp"

struct Key
{
    unsigned char Key;
    KeyState KeyState;
    bool TextAutoRepeat;
};


/// <summary>
/// A class responsible for Keyboard interaction
/// </summary>
class Keyboard
{
    friend class Window;

private:

    /// <summary>
    /// The number of available keys
    /// </summary>
    static constexpr std::size_t NUMBER_OF_KEYS = 256;

private:

    /// <summary>
    /// An array that stores the keys 
    /// </summary>
    Key _keys[NUMBER_OF_KEYS];

public:

    Keyboard() :
        _keys { 0 }
    {
        // Initialize the keys array
        for (int a = 0; a < NUMBER_OF_KEYS; a++)
        {
            _keys[a] = { static_cast<unsigned char>(a), KeyState::None, false };
        };
    };

public:

    /// <summary>
    /// Get a reference to a Key
    /// </summary>
    /// <param name="keycode"> The key's keycode value </param>
    /// <returns></returns>
    const Key& GetKey(int keycode) const
    {
        return _keys[keycode];
    };

    /// <summary>
    /// Get a key state of a certain key
    /// </summary>
    /// <param name="keycode"> The key's keycode </param>
    /// <returns></returns>
    KeyState GetKeyState(int keycode) const
    {
        return _keys[keycode].KeyState;
    };

};