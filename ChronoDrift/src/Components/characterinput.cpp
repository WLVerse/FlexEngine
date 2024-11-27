/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      characterinput.h
\author    [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
\date      27/11/2024
\brief     Character Input Component


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "characterinput.h"

namespace ChronoDrift
{

  FLX_REFL_REGISTER_START(CharacterInput)
    FLX_REFL_REGISTER_PROPERTY(up)
    FLX_REFL_REGISTER_PROPERTY(down)
    FLX_REFL_REGISTER_PROPERTY(left)
    FLX_REFL_REGISTER_PROPERTY(right)
  FLX_REFL_REGISTER_END;

}
