#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\GridRenderer.h"
#include "Content\SampleFpsTextRenderer.h"


// Renders Direct2D and 3D content on the screen.
namespace demo
{
	class demoMain : public DX::IDeviceNotify
	{
	public:
		demoMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~demoMain();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();


		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;



		// TODO: Replace with your own content renderers.
		std::unique_ptr<GridRenderer> m_sceneRenderer;

		// Rendering loop timer.
		DX::StepTimer m_timer;
	};
}