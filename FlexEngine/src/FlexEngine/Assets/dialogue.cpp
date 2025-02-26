#include "pch.h"

#include "dialogue.h"

namespace FlexEngine
{
  namespace Asset
  {
    Dialogue::Dialogue(File& _metadata)
      : metadata(_metadata)
    {
      std::stringstream ss(metadata.Read());
      std::string line;
      while (std::getline(ss, line))
      {
          text.push_back(line);
      }
    }
  } // namespace Asset
} // namespace FlexEngine
