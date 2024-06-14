
// Scene
#include "Camera.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

// RHI
#include "../RHI/Graphics.h"

namespace CGE
{
	namespace Scene
	{
		Camera::Camera() : m_translation(0) {}

		void Camera::SetViewport(const RHI::Viewport& viewport)
		{
			m_viewport = viewport;
		}

		const RHI::Viewport& Camera::GetViewport() const
		{
			return m_viewport;
		}

		void Camera::SetProjectionRH(float vFOV, float aspect, float zNear, float zFar)
		{
			m_vFOV = vFOV;
			m_aspect = aspect;
			m_near = zNear;
			m_far = zFar;

			m_projectionMatrix = glm::perspective(glm::radians(vFOV), aspect, zNear, zFar);
		}

		void Camera::SetProjectionLH(float vFOV, float aspect, float zNear, float zFar)
		{
			m_vFOV = vFOV;
			m_aspect = aspect;
			m_near = zNear;
			m_far = zFar;

			glm::mat4 fix(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 2.0f, 0.0f,
				0.0f, 0.0f, -1.0f, 1.0f);

			m_projectionMatrix = fix * glm::perspective(glm::radians(vFOV), aspect, zNear, zFar);
		}

		void Camera::SetOrthographic(float left, float right, float top, float bottom)
		{
			m_projectionMatrix = glm::ortho(left, right, bottom, top);
		}

		float Camera::GetNearClipPlane() const
		{
			return m_near;
		}

		float Camera::GetFarClipPlane() const
		{
			return m_far;
		}

		void Camera::AddPitch(float fPitch, Space space)
		{
			switch(space)
			{
			case Space::Local:
				m_rotation = glm::angleAxis(glm::radians(fPitch), m_rotation * glm::vec3(1, 0, 0)) * m_rotation;
				break;
			case Space::World:
				m_rotation = glm::angleAxis(glm::radians(fPitch), glm::vec3(1, 0, 0)) * m_rotation;
			}
		}

		void Camera::AddYaw(float fYaw, Space space)
		{
			switch(space)
			{
			case Space::Local:
				m_rotation = glm::angleAxis(glm::radians(fYaw), m_rotation * glm::vec3(0, 1, 0)) * m_rotation;
				break;
			case Space::World:
				m_rotation = glm::angleAxis(glm::radians(fYaw), glm::vec3(0, 1, 0)) * m_rotation;
				break;
			}
		}

		void Camera::AddRoll(float fRoll, Space space)
		{
			switch(space)
			{
			case Space::Local:
				m_rotation = glm::angleAxis(glm::radians(fRoll), m_rotation * glm::vec3(0, 0, 1)) * m_rotation;
				break;
			case Space::World:
				m_rotation = glm::angleAxis(glm::radians(fRoll), glm::vec3(0, 0, 1)) * m_rotation;
				break;
			}
		}

		void Camera::SetEulerAngles(const glm::vec3& eulerAngles)
		{
			m_rotation = glm::quat(glm::radians(eulerAngles));
		}

		void Camera::AddRotation(const glm::quat& deltaRot)
		{
			m_rotation = m_rotation * deltaRot;
		}

		void Camera::TranslateX(float x, Space space)
		{
			switch(space)
			{
			case Space::Local:
				m_translation += m_rotation * glm::vec3(x, 0, 0);
				break;
			case Space::World:
				m_translation += glm::vec3(x, 0, 0);
				break;
			}
		}

		void Camera::TranslateY(float y, Space space)
		{
			switch(space)
			{
			case Space::Local:
				m_translation += m_rotation * glm::vec3(0, y, 0);
				break;
			case Space::World:
				m_translation += glm::vec3(0, y, 0);
				break;
			}
		}

		void Camera::TranslateZ(float z, Space space)
		{
			switch(space)
			{
			case Space::Local:
				m_translation += m_rotation * glm::vec3(0, 0, z);
				break;
			case Space::World:
				m_translation += glm::vec3(0, 0, z);
				break;
			}
		}

		void Camera::SetTranslation(const glm::vec3& translation)
		{
			m_translation = translation;
		}

		void Camera::SetRotation(float pitch, float yaw, float roll)
		{
			SetRotation(glm::vec3(pitch, yaw, roll));
		}

		void Camera::SetRotation(const glm::vec3& rotation)
		{
			SetRotation(glm::quat(glm::radians(rotation)));
		}

		void Camera::SetRotation(const glm::quat& rot)
		{
			m_rotation = rot;
		}

		glm::vec3 Camera::GetTranslation() const
		{
			return m_translation;
		}

		glm::quat Camera::GetRotation() const
		{
			return m_rotation;
		}

		glm::vec3 Camera::GetEulerAngles() const
		{
			return glm::degrees(glm::eulerAngles(m_rotation));
		}

		void Camera::SetViewMatrix(const glm::mat4& viewMatrix)
		{
			// Inverse the view matrix to get the world matrix of the camera
			glm::mat4 inverseView = glm::inverse(viewMatrix);

			// Extract the translation
			m_translation = glm::vec3(inverseView[3]);

			// Extract the top-left 3x3 matrix to decompose the scale and rotation
			glm::mat3 tmp = glm::mat3(inverseView);
			float sx = glm::length(tmp[0]);
			float sy = glm::length(tmp[1]);
			float sz = glm::length(tmp[2]);
			glm::mat3 invScale = glm::mat3(glm::scale(glm::mat4{}, glm::vec3(1.0f / sx, 1.0f / sy, 1.0f / sz)));
			// This will remove the scale factor (if there is one) so we can extract
			// the unit quaternion.
			tmp = tmp * invScale;
			m_rotation = glm::toQuat(tmp);
		}

		glm::mat4 Camera::GetViewMatrix()
		{
			UpdateViewMatrix();
			return m_viewMatrix;
		}

		void Camera::SetProjectionMatrix(const glm::mat4& projectionMatrix)
		{
			m_projectionMatrix = projectionMatrix;
		}

		glm::mat4 Camera::GetProjectionMatrix() const
		{
			return m_projectionMatrix;
		}

		glm::mat4 Camera::GetViewProjectionInverseMatrix()
		{
			UpdateViewProjectionInverse();
			return m_viewProjectionInverse;
		}

		void Camera::UpdateViewMatrix()
		{
			glm::mat4 translateMatrix = glm::translate(glm::mat4{1.0}, m_translation);
			glm::mat4 rotationMatrix = glm::toMat4(m_rotation);
			m_viewMatrix = glm::inverse(translateMatrix * rotationMatrix);
		}

		void Camera::UpdateViewProjectionInverse()
		{
			UpdateViewMatrix();
			m_viewProjectionInverse = glm::inverse(m_projectionMatrix * m_viewMatrix);
		}

		void Camera::OnKeyPressed(KeyEventArgs& keyArgs, UpdateEventArgs& updateArgs)
		{
			CameraMovement movement;
			switch (keyArgs.m_key)
			{
			case GLFW_KEY_W:
				movement.m_forward = 1.0f;
				break;
			case GLFW_KEY_A:
				movement.m_left = 1.0f;
				break;
			case GLFW_KEY_S:
				movement.m_back = 1.0f;
				break;
			case GLFW_KEY_D:
				movement.m_right = 1.0f;
				break;
			case GLFW_KEY_Q:
				movement.m_down = 1.0f;
				break;
			case GLFW_KEY_E:
				movement.m_up = 1.0f;
				break;
			}

			// [todo] Expose to user
			float moveMultiplier = 100.0;
			TranslateX((movement.m_right - movement.m_left) * updateArgs.m_elapsedTime * moveMultiplier);
			TranslateY((movement.m_up - movement.m_down) * updateArgs.m_elapsedTime * moveMultiplier);
			TranslateZ((movement.m_back - movement.m_forward) * updateArgs.m_elapsedTime * moveMultiplier);
		}

		boost::function<void(KeyEventArgs&, UpdateEventArgs&)> Camera::GetKeyPressedFunctionBindable()
		{
			return boost::bind(&Camera::OnKeyPressed, this, _1, _2);
		}
	}
}