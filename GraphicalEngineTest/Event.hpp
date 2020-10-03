#pragma once
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <functional>
#include <cstdint>
#include <debugapi.h>
#include <map>

/// <summary>
/// A generic callback event handling  class
/// </summary>
/// <typeparam name="...TFuncReturn"> The argument(s) type </typeparam>
template<typename ... FuncArgs>
class Event
{
private:

    /// <summary>
    /// A shorthand way of writing the callback "signature"
    /// </summary>
    using CallbackSig = void(FuncArgs...);

    /// <summary>
    /// A shorthand way of writing the callback as an std::function type
    /// </summary>
    using Callback = std::function<void(FuncArgs...)>;


private:

    /// <summary>
    /// A list containing the functions that will be invoked later
    /// </summary>
    std::map<std::uintptr_t, Callback> _callbacks;

public:


    /// <summary>
    /// Invoke or execute the bound functions
    /// </summary>
    /// <param name="...arguments"> The arguments to pass to the functions </param>
    void Invoke(const FuncArgs&... arguments) const
    {
        // Loop through every bound function...
        for (const std::pair<std::uintptr_t, Callback>& callbackPair : _callbacks)
        {
            const Callback& func = callbackPair.second;

            // And invoke it with given parameter(s)
            func(arguments...);
        };
    };


    /// <summary>
    /// Add a new callback function
    /// </summary>
    /// <param name="func"></param>
    void AddCallback(const Callback& func)
    {
        // Get func address as an integer
        const std::uintptr_t funcAddress = GetNumericAddress(func);

        // Try to add the function
        const auto result = _callbacks.emplace(funcAddress, func);
        const bool isSuccesful = result.second;

        // If func already exists...
        if (isSuccesful == false)
        {
            DebugBreak();
            throw std::exception("Callback function is a duplicate");
        };
    };


    /// <summary>
    /// Remove a bound callback function
    /// </summary>
    /// <param name="funcToRemove"></param>
    void RemoveCallback(const Callback& funcToRemove)
    {
        // Get func address as an integer
        const std::uintptr_t funcAddress = GetNumericAddress(funcToRemove);

        const auto func = _callbacks.find(funcAddress);

        // If no func was found
        if (func == _callbacks.cend())
        {
            DebugBreak();
            throw std::exception("No callback was found");
        };

        // Remove the func from the callbacks
        _callbacks.erase(funcAddress);
    };


public:

    /// <summary>
    /// Calls Event::Invoke()
    /// </summary>
    /// <param name="...arguments"></param>
    void operator () (const FuncArgs&... arguments) const
    {
        Invoke(arguments...);
    };

    /// <summary>
    /// Calls Event::AddCallback()
    /// </summary>
    /// <param name="func"></param>
    void operator += (const Callback& func)
    {
        AddCallback(func);
    };

    /// <summary>
    /// Calls Event::RemoveCallback()
    /// </summary>
    /// <param name="func"></param>
    void operator -= (const Callback& func)
    {
        RemoveCallback(func);
    };


public:

    /// <summary>
    /// Retreive an numeric representation of a func address
    /// </summary>
    /// <param name="func"></param>
    /// <returns></returns>
    std::uintptr_t GetNumericAddress(const Callback& func) const
    {
        // Reinterpert cast the address into an unsgiend integer
        const std::uintptr_t funcAddress = reinterpret_cast<std::uintptr_t>(&func);

        return funcAddress;
    };

};

