#pragma once

#include "flx_api.h"

#include "FlexMath/matrix4x4.h"
#include <Renderer/cameramanager.h>

namespace FlexEngine
{
    /*!************************************************************************
    * \class Camera2D
    * \brief Utility class for managing 2D camera transformations and properties.
    *************************************************************************/
    class __FLX_API Camera2D
    {
    public:
        #pragma region Set Functions

        /*!************************************************************************
        * \brief
        * Sets the camera's direction vector.
        * \param curr The CameraData to modify.
        * \param direction New direction vector.
        *************************************************************************/
        static void SetDirection(CameraData& curr, const Vector3& direction);

        /*!************************************************************************
        * \brief
        * Sets the camera's field of view for perspective projection.
        * \param curr The CameraData to modify.
        * \param fov Vertical field of view in degrees.
        *************************************************************************/
        static void SetFieldOfView(CameraData& curr, float fov);

        /*!************************************************************************
        * \brief
        * Sets the aspect ratio of the camera.
        * \param curr The CameraData to modify.
        * \param aspect Aspect ratio (width/height).
        *************************************************************************/
        static void SetAspectRatio(CameraData& curr, float aspect);

        /*!************************************************************************
        * \brief
        * Sets the near and far clipping planes.
        * \param curr The CameraData to modify.
        * \param near Near clipping distance.
        * \param far Far clipping distance.
        *************************************************************************/
        static void SetClippingPlanes(CameraData& curr, float near, float far);

        #pragma endregion

        #pragma region Camera Movement

        static void MoveTo(CameraData& curr, const Vector3& targetPosition);
        static void LerpTo(CameraData& curr, const Vector3& targetPosition, float t);
        
        #pragma endregion

        #pragma region Camera Transitions

        /*!************************************************************************
        * \brief
        * Points the camera at a target position.
        * \param curr The CameraData to modify.
        * \param target Target position to look at.
        *************************************************************************/
        static void LookAt(CameraData& curr, const Vector3& target);

        #pragma endregion

        #pragma region Camera Management

        /*!************************************************************************
        * \brief Updates the camera's view matrix based on current position,
        *        direction, and up vector.
        *************************************************************************/
        static void UpdateViewMatrix(CameraData& curr);

        /*!************************************************************************
        * \brief Updates the camera's projection matrix based on its type (orthographic
        *        or perspective), FOV, aspect ratio, and clipping planes.
        *************************************************************************/
        static void UpdateProjectionMatrix(CameraData& curr);

        #pragma endregion
    };
}