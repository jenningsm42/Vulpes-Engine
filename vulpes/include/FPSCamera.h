#ifndef _VUL_FPSCAMERA_H
#define _VUL_FPSCAMERA_H
#include <memory>
#include "Export.h"
#include "Camera.h"

namespace vul
{
	class InputHandler;

	class VEAPI FPSCamera : public Camera
	{
	public:
		FPSCamera(Engine*, bool fly);
		~FPSCamera();

		void setSpeed(float speed);
		void setSensitivity(float sensitivity);

		void update(float dt);

	private:
		InputHandler* m_ih;
		bool m_fly;
		float m_speed;
		float m_sensitivity;
		float m_hAngle; // Horizontal angle
		float m_vAngle; // Vertical angle
		int m_halfWidth;
		int m_halfHeight;
	};
}

#endif // _VUL_FPSCAMERA_H