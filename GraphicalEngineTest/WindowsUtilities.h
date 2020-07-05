#pragma once
#include <Windows.h>
#include <comdef.h>
#include <string>
#include <memory>

namespace WindowsUtilities
{
    // A macro used to debug COM functions.
    // Creates a scoped variable of type HRESULT and stores the retured function value,
    // using COM's FAILED macro, checks if the result is valid or not
    #define COM_CALL(function) _COM_CALL(function)              

    void _COM_CALL(HRESULT hResult)
    {
        if (FAILED(hResult))
        {
            _com_error comError(hResult);

            size_t charactersConverted = 0;
            size_t errorMessageLength = wcslen(comError.ErrorMessage()) + 1;
            std::unique_ptr<char[]> errorMessageNarrow = std::make_unique<char[]>(errorMessageLength);

            wcstombs_s(&charactersConverted, errorMessageNarrow.get(), errorMessageLength, comError.ErrorMessage(), errorMessageLength);

            throw std::exception(errorMessageNarrow.get());
        };

    };


    // Takes a DWORD error code and returns its string message 
    std::wstring GetLastErrorAsStringW(DWORD error)
    {
        // Stores the error message as a string in memory
        LPWSTR buffer = nullptr;

        // Format DWORD error ID to a string 
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL,
                       error,
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPWSTR)&buffer, 0, NULL);

        // Create std string from buffer
        return buffer;
    };


};