#ifndef _VUL_FPSCAMERA_HPP
#define _VUL_FPSCAMERA_HPP

#include "Camera.hpp"
#include "Export.hpp"
#include "InputHandler.hpp"

namespace vul
{
	class VEAPI FPSCamera : public Camera
	{
	public:
		FPSCamera(Engine&, bool fly = false);
		~FPSCamera();

		void setSpeed(float speed);
		void setSensitivity(float sensitivity);

		void update(float dt);

	private:
		Handle<InputHandler> m_ih;
		bool m_fly;
		float m_speed;
		float m_sensitivity;
		float m_hAngle; // Horizontal angle
		float m_vAngle; // Vertical angle
		int m_halfWidth;
		int m_halfHeight;
	};
}

#endif // _VUL_FPSCAMERA_HPP
