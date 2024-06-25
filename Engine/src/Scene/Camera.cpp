
// Scene
#include "Camera.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

// RHI
#include "../RHI/Graphics.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"

#include <iostream>

namespace CGE
{
	namespace Scene
	{
		Camera::Camera() 
			: m_translation(0.0f, 0.0f, 0.0f)
			, m_rotation(glm::quat(glm::vec3(glm::radians(0.0), glm::radians(0.0), glm::radians(0.0))))
			, m_vFOV(45.0f)
			, m_aspect(RHI::Limits::Device::ClientWidth / (float)RHI::Limits::Device::ClientHeight)
			, m_near(0.1f)
			, m_far(1000.0f)
		{
			UpdateViewMatrix();
			SetProjectionRH(m_vFOV, m_aspect, m_near, m_far);
			m_perViewData = (PerViewData*)_aligned_malloc(sizeof(PerViewData), 16);
			m_perViewData->m_view = GetViewMatrix();
			m_perViewData->m_viewInv = glm::inverse(GetViewMatrix());
			m_perViewData->m_projection = GetProjectionMatrix();
			m_perViewData->m_projectionInv = glm::inverse(GetProjectionMatrix());

			const auto& constantBufferPool = RHI::Graphics::GetBufferSystem().GetCommonBufferPool(RHI::CommonBufferPoolType::Constant);
			auto& rhiFactory = RHI::Graphics::GetFactory();
			m_cameraCbuffer = rhiFactory.CreateBuffer();
			RHI::ResultCode result = RHI::ResultCode::Fail;

			RHI::BufferInitRequest cameraCbufferRequest;
			cameraCbufferRequest.m_buffer = m_cameraCbuffer.get();
			cameraCbufferRequest.m_descriptor.m_byteCount = sizeof(PerViewData);
			cameraCbufferRequest.m_descriptor.m_bindFlags = RHI::BufferBindFlags::Constant;
			cameraCbufferRequest.m_initialData = m_perViewData;
			result = constantBufferPool->InitBuffer(cameraCbufferRequest);
			assert(result == RHI::ResultCode::Success);

			RHI::BufferViewDescriptor cameraBufferViewDescriptor = RHI::BufferViewDescriptor::CreateRaw(0, sizeof(PerViewData));
			m_cameraCbufferView = rhiFactory.CreateBufferView();
			m_cameraCbufferView->Init(*m_cameraCbuffer, cameraBufferViewDescriptor);

			const RHI::ShaderPermutation& defaultPBRForward_MaterialShader = *RHI::Graphics::GetAssetProcessor().GetShaderPermutation("DefaultPBRForward_MaterialShader");
			const RHI::ShaderResourceGroupLayout* viewSrgLayout = defaultPBRForward_MaterialShader.m_pipelineLayoutDescriptor->GetShaderResourceGroupLayout(RHI::ShaderResourceGroupType::View);
			m_viewSrg = rhiFactory.CreateShaderResourceGroup();
			RHI::ShaderResourceGroupData viewSrgData(viewSrgLayout);

			RHI::ShaderInputBufferIndex cameraBufferIdx = viewSrgLayout->FindShaderInputBufferIndex("PerView_CameraMatrix");
			viewSrgData.SetBufferView(cameraBufferIdx, m_cameraCbufferView.get(), 0);
			m_viewSrg->Init(m_cameraCbuffer->GetDevice(), viewSrgData);
			m_viewSrg->Compile();
		}

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

			m_dirty = true;
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

			m_dirty = true;
		}

		void Camera::SetOrthographic(float left, float right, float top, float bottom)
		{
			m_projectionMatrix = glm::ortho(left, right, bottom, top);
			m_dirty = true;
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
			m_dirty = true;
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
			m_dirty = true;
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
			m_dirty = true;
		}

		void Camera::SetEulerAngles(const glm::vec3& eulerAngles)
		{
			m_rotation = glm::quat(glm::radians(eulerAngles));
			m_dirty = true;
		}

		void Camera::AddRotation(const glm::quat& deltaRot)
		{
			m_rotation = m_rotation * deltaRot;
			m_dirty = true;
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
			m_dirty = true;
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
			m_dirty = true;
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
			m_dirty = true;
		}

		void Camera::SetTranslation(const glm::vec3& translation)
		{
			m_translation = translation;
			m_dirty = true;
		}

		void Camera::SetRotation(float pitch, float yaw, float roll)
		{
			SetRotation(glm::vec3(pitch, yaw, roll));
			m_dirty = true;
		}

		void Camera::SetRotation(const glm::vec3& rotation)
		{
			SetRotation(glm::quat(glm::radians(rotation)));
			m_dirty = true;
		}

		void Camera::SetRotation(const glm::quat& rot)
		{
			m_rotation = rot;
			m_dirty = true;
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
			m_dirty = true;
		}

		glm::mat4 Camera::GetViewMatrix()
		{
			return m_viewMatrix;
		}

		void Camera::SetProjectionMatrix(const glm::mat4& projectionMatrix)
		{
			m_projectionMatrix = projectionMatrix;
			m_dirty = true;
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
			glm::mat4 translateMatrix = glm::translate(glm::mat4{ 1.0 }, m_translation);
			glm::mat4 rotationMatrix = glm::toMat4(m_rotation);
			m_viewMatrix = glm::inverse(translateMatrix * rotationMatrix);
		}

		void Camera::UpdateProjectionMatrix()
		{
			m_projectionMatrix = glm::perspective(glm::radians(m_vFOV), m_aspect, m_near, m_far);
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

		void Camera::SpawnCameraImGuiWindow()
		{
			const auto updateChange = [this](bool check) {m_dirty = check || m_dirty; };
			if (ImGui::Begin("Camera"))
			{
				ImGui::Text("Position");
				updateChange(ImGui::SliderFloat("Pos X", &m_translation.x, -80.0f, 80.0f, "%.1f"));
				updateChange(ImGui::SliderFloat("Pos Y", &m_translation.y, -80.0f, 80.0f, "%.1f"));
				updateChange(ImGui::SliderFloat("Pos Z", &m_translation.z, -80.0f, 80.0f, "%.1f"));

				ImGui::Text("Orientation");
				updateChange(ImGui::SliderAngle("Rot X", &m_rotation.x, 0.995f * -90.0f, 0.995f * 90.0f));
				updateChange(ImGui::SliderAngle("Rot Y", &m_rotation.y, 0.995f * -90.0f, 0.995f * 90.0f));
				updateChange(ImGui::SliderAngle("Rot Z", &m_rotation.z, 0.995f * -90.0f, 0.995f * 90.0f));

				ImGui::Text("Projection");
				updateChange(ImGui::SliderFloat("FOV", &m_vFOV, 45.0f, 120.0f, "%.1f"));
				updateChange(ImGui::SliderFloat("Near Z", &m_near, 0.01f, m_far - 0.01f, "%.2f"));
				updateChange(ImGui::SliderFloat("Far Z", &m_far, m_near + 0.01f, 1000.0f, "%.2f"));
				ImGui::End();
			}
		}

		RHI::ResultCode Camera::UpdateCameraBuffer()
		{
			m_perViewData->m_projection = GetProjectionMatrix();
			m_perViewData->m_projectionInv = glm::inverse(GetProjectionMatrix());
			m_perViewData->m_view = GetViewMatrix();
			m_perViewData->m_viewInv = glm::inverse(GetViewMatrix());

			const auto& constantBufferPool = RHI::Graphics::GetBufferSystem().GetCommonBufferPool(RHI::CommonBufferPoolType::Constant);

			RHI::BufferMapRequest mapRequest{};
			mapRequest.m_buffer = m_cameraCbuffer.get();
			mapRequest.m_byteCount = sizeof(PerViewData);
			mapRequest.m_byteOffset = 0;

			RHI::BufferMapResponse mapResponse{};

			RHI::ResultCode mapSuccess = constantBufferPool->MapBuffer(mapRequest, mapResponse);
			if (mapSuccess == RHI::ResultCode::Success)
			{
				memcpy(mapResponse.m_data, m_perViewData, sizeof(PerViewData));
				constantBufferPool->UnmapBuffer(*m_cameraCbuffer);
			}
			return mapSuccess;
		}

		void Camera::Update()
		{
			if (m_dirty)
			{
				UpdateViewMatrix();
				UpdateProjectionMatrix();
				UpdateCameraBuffer();
			}
			m_dirty = false;
		}

		RHI::ShaderResourceGroup* Camera::GetCameraSrg() const
		{
			return m_viewSrg.get();
		}
	}
}