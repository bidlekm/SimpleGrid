#include "pch.h"
#include "demoMain.h"
#include "Common\DirectXHelper.h"
#include "Camera.h"

typedef interface ID3DXLine ID3DXLine;
typedef interface ID3DXLine* LPD3DXLINE;
using namespace demo;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;


const DXGI_FORMAT c_backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
const DXGI_FORMAT c_depthBufferFormat = DXGI_FORMAT_D32_FLOAT;

unsigned int c_targetSampleCount = 4;


// Loads and initializes application assets when the application is loaded.
demoMain::demoMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{
	Camera camera({ -1.0f, 1.7f, 1.5f, 0.0f },
				  { -0.5f, -0.1f, 0.0f, 0.0f },
				  { 0.0f, 1.0f, 0.0f, 0.0f });


	// Register to be notified if the Device is lost or recreated
	m_deviceResources->RegisterDeviceNotify(this);

	// TODO: Replace this with your app's content initialization.
	m_sceneRenderer = std::unique_ptr<GridRenderer>(new GridRenderer(m_deviceResources,std::make_shared<Camera>(camera)));

		// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	OnDeviceRestored();
}

demoMain::~demoMain()
{
	// Deregister device notification
	m_deviceResources->RegisterDeviceNotify(nullptr);
}

// Updates application state when the window size changes (e.g. device orientation change)
void demoMain::CreateWindowSizeDependentResources() 
{
	auto output = m_deviceResources->GetOutputSize();

	// TODO: Replace this with the size-dependent initialization of your app's content.
	m_sceneRenderer->CreateWindowSizeDependentResources();
}

// Updates the application state once per frame.
void demoMain::Update() 
{
	// Update scene objects.
	m_timer.Tick([&]()
	{
		// TODO: Replace this with your app's content update functions.
		m_sceneRenderer->Update(m_timer);
		
	});
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool demoMain::Render() 
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	 //Reset the viewport to target the whole screen.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);
	
	// Reset render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

	// Clear the back buffer and depth stencil view.
	context->ClearRenderTargetView(m_deviceResources->GetRenderTargetView(), DirectX::Colors::White);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	// Render the scene objects.
	// TODO: Replace this with your app's content rendering functions.
	m_sceneRenderer->Render();


	return true;
}



// Notifies renderers that device resources need to be released.
void demoMain::OnDeviceLost()
{
	m_sceneRenderer->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void demoMain::OnDeviceRestored()
{
	m_sceneRenderer->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}
