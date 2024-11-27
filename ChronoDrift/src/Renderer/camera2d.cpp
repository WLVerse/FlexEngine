#include "camera2d.h"

namespace FlexEngine 
{
    #pragma region Set Functions

    void Camera2D::SetDirection(CameraData& curr, const Vector3& target)
    {
        curr.target = target;
        curr.right = Cross(curr.up, target).Normalize();
        curr.viewMatrix = Matrix4x4::LookAt(curr.position, curr.position + curr.target, curr.up);
    }

    void Camera2D::SetFieldOfView(CameraData& curr, float fov)
    {
        curr.fieldOfView = fov;
        curr.projMatrix = Matrix4x4::Perspective(fov, curr.aspectRatio, curr.nearClip, curr.farClip);
    }

    void Camera2D::SetAspectRatio(CameraData& curr, float aspect)
    {
        curr.aspectRatio = aspect;
        curr.projMatrix = Matrix4x4::Perspective(curr.fieldOfView, aspect, curr.nearClip, curr.farClip);
    }

    void Camera2D::SetClippingPlanes(CameraData& curr, float near, float far)
    {
        curr.nearClip = near;
        curr.farClip = far;
        curr.projMatrix = Matrix4x4::Perspective(curr.fieldOfView, curr.aspectRatio, near, far);
    }

    #pragma endregion

    #pragma region Camera Management

    void Camera2D::UpdateViewMatrix(CameraData& curr)
    {
        //curr.position, curr.target, curr.up
        curr.viewMatrix = Matrix4x4::LookAt(Vector3::Zero, Vector3::Back, Vector3::Up);
        curr.proj_viewMatrix = curr.projMatrix * curr.viewMatrix;
    }

    void Camera2D::UpdateProjectionMatrix(CameraData& curr)
    {
        if (curr.m_isOrthographic)
        {
            float width = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth());
            float height = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight());

            //Center Cam
            curr.projMatrix = Matrix4x4::Orthographic(
                curr.position.x + width / 2.0f,
                curr.position.x - width / 2.0f,
                curr.position.y + height / 2.0f, 
                curr.position.y - height / 2.0f, 
                curr.nearClip,
                curr.farClip
            );
        }
        else
        {
            curr.projMatrix = Matrix4x4::Perspective(curr.fieldOfView, curr.aspectRatio, curr.nearClip, curr.farClip);
        }
        curr.proj_viewMatrix = curr.projMatrix * curr.viewMatrix;
    }

    #pragma endregion

    #pragma region Camera Transitions

    void Camera2D::LookAt(CameraData& curr, const Vector3& target)
    {
        curr.target = target;
        curr.right = Cross(curr.up, curr.position - curr.target).Normalize();
        curr.viewMatrix = Matrix4x4::LookAt(curr.position, target, curr.up);
    }

    #pragma endregion

    #pragma region Camera Movement

    void Camera2D::MoveTo(CameraData& curr, const Vector3& targetPosition)
    {
        curr.position = targetPosition;
        UpdateViewMatrix(curr);
    }

    void Camera2D::LerpTo(CameraData& curr, const Vector3& targetPosition, float t)
    {
        curr.position = Lerp(curr.position, targetPosition, t);
        UpdateViewMatrix(curr);
    }
    #pragma endregion
}