/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      characterinput.h
\author    [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
\date      27/11/2024
\brief     Character Input Component Declaration


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace ChronoDrift
{
  class CharacterInput
  {
    FLX_REFL_SERIALIZABLE
  public:
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
  };
}
