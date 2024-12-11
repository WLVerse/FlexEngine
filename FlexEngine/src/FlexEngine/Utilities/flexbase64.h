// WLVERSE [https://wlverse.web.app]
// flexbase64.h
// 
// Base64 encoding and decoding functions.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"
#include "flx_windows.h" // BYTE

#include <string>
#include <vector>

namespace FlexEngine
{
  namespace Base64
  {

    // Encodes a vector of bytes to a base64 string.
    __FLX_API std::string Encode(const std::vector<BYTE>& data);

    // Decodes a base64 string to a vector of bytes.
    __FLX_API std::vector<BYTE> Decode(const std::string& data);

  }
}
