#include <iostream>

extern "C" __declspec(dllexport) float EditingAValue(float deltaTime) {
  std::cout << "Custom script logic with deltaTime = " << deltaTime << std::endl;
  return deltaTime * 2.0f;
}