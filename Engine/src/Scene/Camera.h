#pragma once

// RHI
#include "../RHI/Viewport.h"
#include "../RHI/Image.h"

#include "../Events.h"

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace CGE
{
	namespace Scene
	{
		class Camera
		{
		public:
			enum class Space
			{
				Local,
				World
			};
		private:
			struct CameraMovement
			{
				// Translation movement
				float m_forward, m_back, m_left, m_right, m_up, m_down;
				// Rotation movement
				float m_rollCW, m_rollCCW;
				float m_pitch, m_yaw;
				// Move in/out from pivot point.
				float m_pivotTranslate;
				// Do you want to go faster?
				bool m_translateFaster;
				bool m_rotateFaster;

				CameraMovement()
					: m_forward(0.0f)
					, m_back(0.0f)
					, m_left(0.0f)
					, m_right(0.0f)
					, m_up(0.0f)
					, m_down(0.0f)
					, m_rollCW(0.0f)
					, m_rollCCW(0.0f)
					, m_pitch(0.0f)
					, m_yaw(0.0f)
					, m_pivotTranslate(0.0f)
					, m_translateFaster(false)
					, m_rotateFaster(false)
				{}
			};
		public:
			Camera();
			~Camera() = default;

			void SetViewport(const RHI::Viewport& viewport);
			const RHI::Viewport& GetViewport() const;

			void SetProjectionRH(float fovy, float aspect, float zNear, float zFar);
			void SetProjectionLH(float vFOV, float aspect, float zNear, float zFar);
			void SetOrthographic(float left, float right, float top, float bottom);

			float GetNearClipPlane() const;
			float GetFarClipPlane() const;

			// Rotation about the X-axis
			void AddPitch(float fPitch, Space space = Space::Local);
			// Rotation about the Y-axis
			void AddYaw(float fYaw, Space space = Space::Local);
			// Rotation about the Z-axis
			void AddRoll(float fRoll, Space space = Space::Local);
			// Set Euler angles (in degrees)
			void SetEulerAngles(const glm::vec3& eulerAngles);
			// Add rotation to the current rotation (always in local space)
			void AddRotation(const glm::quat& deltaRot);

			void TranslateX(float x, Space space = Space::Local);
			void TranslateY(float y, Space space = Space::Local);
			void TranslateZ(float z, Space space = Space::Local);

			void SetTranslation(const glm::vec3& translate);
			void SetRotation(float pitch, float yaw, float roll);
			void SetRotation(const glm::vec3& rotate);
			void SetRotation(const glm::quat& rot);
			glm::vec3 GetTranslation() const;
			glm::quat GetRotation() const;
			glm::vec3 GetEulerAngles() const;

			void SetViewMatrix(const glm::mat4& viewMatrix);
			glm::mat4 GetViewMatrix();

			void SetProjectionMatrix(const glm::mat4& projectionMatrix);
			glm::mat4 GetProjectionMatrix() const;

			glm::mat4 GetViewProjectionInverseMatrix();

			boost::function<void(KeyEventArgs&, UpdateEventArgs&)> GetKeyPressedFunctionBindable();
			void OnKeyPressed(KeyEventArgs& keyArgs, UpdateEventArgs& updateArgs);

		private:
			void UpdateViewMatrix();
			void UpdateViewProjectionInverse();

		private:
			RHI::Viewport m_viewport;

			// Projection parameters
			float m_vFOV;
			float m_aspect;
			float m_near;
			float m_far;

			// Cameras world space parameters
			glm::vec3 m_translation;
			glm::quat m_rotation;

			// View and projection matricies
			glm::mat4 m_viewMatrix;
			glm::mat4 m_projectionMatrix;
			glm::mat4 m_viewProjectionInverse;
		};
	}
}