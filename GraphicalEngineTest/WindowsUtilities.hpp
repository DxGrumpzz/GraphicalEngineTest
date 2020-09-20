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


    /// <summary>
    /// Takes a DWORD error code and returns its string message  
    /// </summary>
    /// <param name="error"></param>
    /// <returns></returns>
    std::wstring ErrorToStringW(DWORD error)
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

    /// <summary>
    /// Returns the last windows error as a wide string
    /// </summary>
    /// <returns></returns>
    std::wstring GetLastErrorAsStringW()
    {
        return ErrorToStringW(GetLastError());
    };


    /// <summary>
    /// A function that displays a MessageBox with some error details
    /// </summary>
    /// <param name="error"> The error to display </param>
    /// <param name="title"> The error title </param>
    /// <param name="line"> Current line number, ideally do not pass a value here </param>
    /// <param name="file"> Current file, ideally do not pass a value here</param>
    void ShowWinErrorW(const std::wstring& error, const std::wstring& title = L"WinError", 
                      std::intmax_t line = __LINE__, const wchar_t* file = __FILEW__)
    {
        // Create a digestable error message
        std::wstring outputError;
        outputError.reserve(256);

        outputError.append(L"An error occured in ")
            .append(file)
            .append(L"\n")
            .append(L"Line: ")
            .append(std::to_wstring(line))
            .append(L"\n")
            .append(L"Error:\n")
            .append(error);

        MessageBoxW(NULL, outputError.c_str(), title.c_str(), MB_ICONERROR);

    };

};