#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"
#include <iostream>
#include "Matrix.h"
#include "Vector3.h"




namespace dae
{
	class Camera final
	{

	private:

		dae::Vector3 origin{};
		float fovAngle{ 90.f };
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };
		int m_Width{};
		int m_Height{};

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{};
		float totalYaw{};
		float senstivity{ .0025f };
		float m_near{ .1f };
		float m_far{ 100.f };

		Matrix m_invViewMatrix{};
		Matrix m_viewMatrix{};
		Matrix m_projectionMatrix{};

	public:
		Camera() = default;
		Camera(const Vector3& _origin, float _fovAngle, int width, int height) :
			origin{ _origin },
			fovAngle{ _fovAngle },
			m_Width{ width },
			m_Height{ height }
		{}

		dae::Vector3 getCameraPos() const { return origin; }

		void Initialize(float _fovAngle = 90.f, Vector3 _origin = { 0.f,0.f,0.f }, int width = 0, int height = 0)
		{
			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);
			m_Width = width;
			m_Height = height;
			origin = _origin;
			CalculateProjectionMatrix();

		}

		void CalculateViewMatrix()
		{


			right = Vector3::Cross(Vector3::UnitY, forward);

			right.Normalize();

			up = Vector3::Cross(forward, right);

			up.Normalize();


			m_invViewMatrix = Matrix(right, up, forward, origin);

			m_viewMatrix = Matrix::Inverse(m_invViewMatrix);



		}

		void CalculateProjectionMatrix()
		{


			const float aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);

			fov = tanf((fovAngle * TO_RADIANS) / 2.f);

			const float screenX{ 1.f / (aspectRatio * fov) };
			const float screenY{ 1.f / fov };

			const float a{ m_far / (m_far - m_near) };
			const float b{ -(m_far * m_near) / (m_far - m_near) };

			m_projectionMatrix = Matrix{ {screenX,0.f,0.f,0.f}
									,{0.f,screenY,0.f,0.f}
									,{0.f,0.f, a ,1.f} ,
									 {0.f,0.f, b ,0.f} };


		}

		Matrix GetViewMatrix() const { return m_viewMatrix; }
		Matrix GetProjectionMatrix() const { return m_projectionMatrix; }

		void Update(const Timer* pTimer)
		{

			const float temp_fov{ fovAngle };

			const float deltaTime = pTimer->GetElapsed();


			float speed = 5.f;
			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			if (pKeyboardState[SDL_SCANCODE_LSHIFT])
			{
				speed = 8.f;
			}
			else
			{
			    speed = 5.f;
			}

			if (pKeyboardState[SDL_SCANCODE_W] || pKeyboardState[SDL_SCANCODE_UP])
			{

				origin += forward * speed * deltaTime;

			}
			else if (pKeyboardState[SDL_SCANCODE_A] || pKeyboardState[SDL_SCANCODE_LEFT])
			{
				origin -= right * speed * deltaTime;
			}
			else if (pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_DOWN])
			{
				origin -= forward * speed * deltaTime;
			}
			else if (pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_RIGHT])
			{
				origin += right * speed * deltaTime;
			}

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			if ((mouseState & SDL_BUTTON_LMASK))
			{
				if ((mouseState & SDL_BUTTON_RMASK) > 0)
				{
					origin -= up * speed * float(mouseY) * senstivity;
					origin -= right * speed * float(mouseX) * senstivity;
				}

				else {
					fovAngle += static_cast<float>(mouseY) * senstivity * 10;
					totalPitch += static_cast<float>(mouseX) * senstivity;
				}
			}

			else if ((mouseState & SDL_BUTTON_RMASK))
			{


				if ((mouseState & SDL_BUTTON_LMASK) > 0)
				{
					origin -= up * speed  * float(mouseY) * senstivity;
					origin -= right * speed  * float(mouseX) * senstivity;
				}


				else
				{

					totalYaw -= static_cast<float>(mouseY) * senstivity;
					totalPitch += static_cast<float>(mouseX) * senstivity;

				}
			}






			Matrix finalRotation{};

			finalRotation = finalRotation.CreateRotation(Vector3(totalYaw, totalPitch, 0));

			forward = finalRotation.TransformVector(Vector3::UnitZ);

			forward.Normalize();


			//Update Matrices
			CalculateViewMatrix();

			if (fovAngle != temp_fov)
			{
				CalculateProjectionMatrix();
			}
		}





	};
}


