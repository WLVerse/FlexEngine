//LEGACY CODE 
//#pragma once
//#include "flx_api.h"
//
//#include "FlexMath/vector4.h"
//#include "../OpenGL/opengltexture.h"
//#include "../OpenGL/openglrenderer.h"
//
//
//#include <glad/glad.h>
//
//
//
//namespace FlexEngine 
//{
//  struct __FLX_API Line2D
//  {
//    Vector2 start;
//    Vector2 end;
//    Vector3 color = {1.0f, 0.0f, 0.0f};
//  };
//  
//  /* NOTE
//  * right now, The debug drawer does not support global position.
//  * coordinates entered are in screen space, (0,0) being top left.
//  * 
//  * TODO
//  * After our Scene gets a Camera.GetViewMatrix function, convert positions to global pos.
//  * 
//  * IMPROVEMENTS
//  * Add a duration for debug draw
//  * DebugDraw function queue
//  * 
//  */
//  class __FLX_API DebugRenderer
//  {
//  public:
//
//    static void DrawLine2D(Vector2 start, Vector2 end, 
//                          Vector3 color = {1.0f, 0.0f, 0.0f}, float line_width = 2.0f);
//    static void DrawAABB(Vector3 position, float width, float height, Vector3 color = {1.0f, 0.0f, 0.0f}, float line_width = 2.0f);
//
//
//  private:
//    static Asset::Shader m_line_shader;
//
//
//  };
//
//}