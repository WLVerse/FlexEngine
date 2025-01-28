// WLVERSE [https://wlverse.web.app]
// flexbase64.cpp
// 
// Base64 encoding and decoding functions.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "pch.h"

#include "flexbase64.h"

#include <wincrypt.h> // CryptBinaryToStringA, CryptStringToBinaryA

namespace FlexEngine
{
  namespace Base64
  {

    #pragma region Internal Function

    bool Internal_ValidateBase64(const std::string& data)
    {
      // guard
      if (data.empty()) return false;

      static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/"
      ;

      // check if the data is a valid base64 string
      for (char c : data)
      {
        if (base64_chars.find(c) == std::string::npos && c != '=') return false;
      }

      return true;
    }

    std::string Internal_GetErrorMessage()
    {
      DWORD error_code = GetLastError();
      LPSTR message_buffer = nullptr;
      DWORD size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message_buffer, 0, nullptr
      );
      std::string message(message_buffer, size);
      LocalFree(message_buffer);
      return message;
    }

    #pragma endregion

    __FLX_API std::string Encode(const std::vector<BYTE>& data)
    {
      // guard
      if (data.empty()) return "";

      DWORD data_size = static_cast<DWORD>(data.size());
      DWORD size = 0;

      DWORD flags = CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF;

      // get size
      if (!CryptBinaryToStringA(data.data(), data_size, flags, nullptr, &size))
      {
        Log::Error("Base64 encoding: Failed to get the size of the encoded data. " + Internal_GetErrorMessage());
        return "";
      }

      // allocate
      std::string result(size, '\0');

      // encode
      if (!CryptBinaryToStringA(data.data(), data_size, flags, &result[0], &size))
      {
        Log::Error("Base64 encoding: Failed to encode the data. " + Internal_GetErrorMessage());
        return "";
      }

      // CryptBinaryToStringA includes the null terminator in the string.
      // This removes it.
      result.pop_back();

      return result;
    }

    __FLX_API std::vector<BYTE> Decode(const std::string& data)
    {
      // guard
      if (data.empty()) return {};

      // validate
      if (!Internal_ValidateBase64(data))
      {
        Log::Error("Base64 decoding: The input data is not a valid base64 string.");
        return {};
      }

      DWORD data_size = static_cast<DWORD>(data.size() + 1);
      DWORD size = 0;

      DWORD flags = CRYPT_STRING_BASE64;// | CRYPT_STRING_NOCRLF;

      // get size
      if (!CryptStringToBinaryA(data.c_str(), data_size, flags, nullptr, &size, nullptr, nullptr))
      {
        Log::Error("Base64 decoding: Failed to get the size of the decoded data. " + Internal_GetErrorMessage());
        return {};
      }

      // allocate
      std::vector<BYTE> result(size);

      // decode
      if (!CryptStringToBinaryA(data.c_str(), data_size, flags, result.data(), &size, nullptr, nullptr))
      {
        Log::Error("Base64 decoding: Failed to decode the data. " + Internal_GetErrorMessage());
        return {};
      }

      return result;
    }

  }
}