/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// camerasystemlayer.h / cameraservicelayer.h
// 
// Layer that handles the updates to validating cameras to update in CamManager
//
// The CameraSystemLayer class provides camera shake and zoom effects for enhanced
// gameplay experience. It manages multiple shake effects and zoom parameters,
// including both persistent and auto-return zoom behaviors.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game {

    // The CameraSystemLayer handles camera shake and zoom effects.
    class CameraSystemLayer : public FlexEngine::Layer {
    public:
        // Constructs a new CameraSystemLayer.
        CameraSystemLayer();

        // Default destructor.
        ~CameraSystemLayer() override = default;

        // Called when the layer is attached to the engine.
        // Initializes camera parameters and effects.
        void OnAttach() override;

        // Called when the layer is detached from the engine.
        // Cleans up any allocated resources or state.
        void OnDetach() override;

        // Main update loop called every frame.
        // Processes active shake and zoom effects.
        void Update() override;

    private:
        // Represents a shake effect applied to the camera.
        struct ShakeEffect
        {
            float duration;   // Total duration of the effect.
            float elapsed;    // Time elapsed so far.
            float intensity;  // Base shake intensity.
            bool lerp;        // If true, intensity ramps up then down.
        };

        // Represents a zoom effect with parameters for both persistent and auto-return modes.
        struct ZoomEffect
        {
            // Persistent zoom effect parameters.
            float duration;         // Total duration for persistent zoom effect.

            // Common parameters:
            float elapsed;          // Time elapsed so far.
            float targetOrthoWidth; // Desired target orthographic width.
            float initialOrthoWidth;// Starting orthographic width.
            bool autoReturn;        // If true, zoom will ramp in then return.

            // Auto-return effect parameters (used when autoReturn is true):
            float lerpDuration;     // Lerp duration for ramping up and down.
            float holdDuration;     // Duration to hold at target zoom.
            float totalDuration;    // Total duration (2 * lerpDuration + holdDuration).
        };

        // Collection of active shake effects.
        std::vector<ShakeEffect> m_shakeEffects;

        // Current zoom effect configuration.
        ZoomEffect m_zoomEffect;

        // Original camera position used as the base for applying shake effects.
        Vector3 m_originalCameraPos;

        // Original orthographic width of the camera.
        float m_zoomBase = 1920.0f;

        // Indicates if a zoom effect is currently active.
        bool m_zoomActive = false;

        // Base aspect ratio of the camera.
        const float m_baseAspectRatio = 16.0f / 9.0f;

        // Minimum allowed orthographic width.
        const float m_minOrthoWidth = 640.0f;

        #pragma region Helper Functions
        // Generates a random shake offset based on the given intensity.
        // @param intensity The base intensity for the shake effect.
        // @return A Vector3 representing the offset to be applied to the camera.
        Vector3 GenerateShakeOffset(float intensity);
        #pragma endregion
    };

} // namespace Game
