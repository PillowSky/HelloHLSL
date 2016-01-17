//--------------------------------------------------------------------------------------
// File: HelloHLSL.cpp
//
// Empty starting point for new Direct3D 11 Win32 desktop applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "VertexShader.h"
#include "PixelShader.h"

#pragma warning(disable : 4100)

using namespace DirectX;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct Vertex {
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT4 Color;
};

struct ConstantBufferPerFrame {
	XMFLOAT4X4 ModelViewProjection;
	XMFLOAT4X4 Model;
	XMFLOAT4 LightDir[2];
};

struct ConstantBufferPersist {
	XMFLOAT4 LightColor[2];
};


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
ID3D11VertexShader*         g_pVertexShader = nullptr;
ID3D11PixelShader*          g_pPixelShader = nullptr;
ID3D11InputLayout*          g_pVertexLayout = nullptr;
ID3D11Buffer*               g_pVertexBuffer = nullptr;
ID3D11Buffer*               g_pIndexBuffer = nullptr;
ID3D11Buffer*               g_pConstantBufferPerFrame = nullptr;
ID3D11Buffer*               g_pConstantBufferPersist = nullptr;
XMMATRIX                    g_Model;
XMMATRIX                    g_View;
XMMATRIX                    g_Projection;
XMVECTOR                    g_LightDir[2];

//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext) {
	return true;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext) {
	return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,	void* pUserContext) {
	HRESULT hr = S_OK;

	auto pd3dImmediateContext = DXUTGetD3D11DeviceContext();

	// Create the vertex shader
	V_RETURN(pd3dDevice->CreateVertexShader(g_vertexshader_byte, sizeof(g_vertexshader_byte), nullptr, &g_pVertexShader));

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// Create the input layout
	V_RETURN(pd3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout), g_vertexshader_byte, sizeof(g_vertexshader_byte), &g_pVertexLayout));

	// Set the input layout
	pd3dImmediateContext->IASetInputLayout(g_pVertexLayout);

	// Create the pixel shader
	V_RETURN(pd3dDevice->CreatePixelShader(g_pixelshader_byte, sizeof(g_pixelshader_byte), nullptr, &g_pPixelShader));

	// Create vertex buffer
	Vertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	V_RETURN(pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer));

	// Set vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pd3dImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// Create index buffer
	DWORD indices[] =
	{
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		22,20,21,
		23,20,22
	};

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(DWORD) * ARRAYSIZE(indices);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	V_RETURN(pd3dDevice->CreateBuffer(&bd, &InitData, &g_pIndexBuffer));

	// Set index buffer
	pd3dImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set primitive topology
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the per-frame constant buffers
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(ConstantBufferPerFrame);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	V_RETURN(pd3dDevice->CreateBuffer(&bd, nullptr, &g_pConstantBufferPerFrame));

	// Initialize the persists
	ConstantBufferPersist persists = { { XMFLOAT4{ 0.5f, 0.5f, 0.5f, 1.0f }, XMFLOAT4{ 0.5f, 0.0f, 0.0f, 1.0f } } };

	// Create the persist constant buffers
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBufferPersist);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = &persists;
	V_RETURN(pd3dDevice->CreateBuffer(&bd, &InitData, &g_pConstantBufferPersist));

	// Initialize the model matrices
	g_Model = XMMatrixIdentity();

	// Initialize the view matrix
	static const XMVECTORF32 s_Eye = { 0.0f, 2.5f, 5.0f, 0.f };
	static const XMVECTORF32 s_At = { 0.0f, 0.0f, 0.0f, 0.f };
	static const XMVECTORF32 s_Up = { 0.0f, 1.0f, 0.0f, 0.f };
	g_View = XMMatrixLookAtLH(s_Eye, s_At, s_Up);

	return hr;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext) {
	// Setup the projection parameters
	float fAspect = static_cast<float>(pBackBufferSurfaceDesc->Width) / static_cast<float>(pBackBufferSurfaceDesc->Height);
	g_Projection = XMMatrixPerspectiveFovLH(XM_PI * 0.5f, fAspect, 0.1f, 100.0f);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext) {
	// Rotate cube
	g_Model = XMMatrixRotationY(fTime);

	// Rotate light
	g_LightDir[0] = XMVector3Transform(XMVECTORF32{ 0.0f, 0.0f, 1.0f, 1.0f }, XMMatrixRotationX(-2.0f * fTime));
	g_LightDir[1] = XMVector3Transform(XMVECTORF32{ 0.0f, 0.0f, -1.0f, 1.0f }, XMMatrixRotationY(-2.0f * fTime));
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext) {
	//
	// Clear the back buffer
	//
	auto pRTV = DXUTGetD3D11RenderTargetView();
	pd3dImmediateContext->ClearRenderTargetView(pRTV, Colors::MidnightBlue);

	//
	// Clear the depth stencil
	//
	auto pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

	//
	// Update variables for the first cube
	//
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	V(pd3dImmediateContext->Map(g_pConstantBufferPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	auto pCB = reinterpret_cast<ConstantBufferPerFrame*>(MappedResource.pData);
	XMStoreFloat4x4(&pCB->ModelViewProjection, XMMatrixTranspose(g_Model * g_View * g_Projection));
	XMStoreFloat4x4(&pCB->Model, XMMatrixTranspose(g_Model));

	XMStoreFloat4(&pCB->LightDir[0], g_LightDir[0]);
	XMStoreFloat4(&pCB->LightDir[1], g_LightDir[1]);
	pd3dImmediateContext->Unmap(g_pConstantBufferPerFrame, 0);

	//
	// Render the center cube
	//
	pd3dImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBufferPerFrame);
	pd3dImmediateContext->VSSetConstantBuffers(1, 1, &g_pConstantBufferPersist);
	pd3dImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);
	pd3dImmediateContext->PSSetConstantBuffers(0, 1, &g_pConstantBufferPerFrame);
	pd3dImmediateContext->PSSetConstantBuffers(1, 1, &g_pConstantBufferPersist);
	pd3dImmediateContext->DrawIndexed(36, 0, 0);

	//
	// Update variables for the first light
	//
	XMMATRIX mLight = XMMatrixScaling(0.25f, 0.25f, 0.25f) * XMMatrixTranslationFromVector(4 * g_LightDir[0]);
	V(pd3dImmediateContext->Map(g_pConstantBufferPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	pCB = reinterpret_cast<ConstantBufferPerFrame*>(MappedResource.pData);
	XMStoreFloat4x4(&pCB->ModelViewProjection, XMMatrixTranspose(mLight * g_View * g_Projection));
	XMStoreFloat4x4(&pCB->Model, XMMatrixTranspose(mLight));

	XMStoreFloat4(&pCB->LightDir[0], g_LightDir[0]);
	XMStoreFloat4(&pCB->LightDir[1], g_LightDir[1]);
	pd3dImmediateContext->Unmap(g_pConstantBufferPerFrame, 0);

	//
	// Render the first light
	//
	pd3dImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBufferPerFrame);
	pd3dImmediateContext->VSSetConstantBuffers(1, 1, &g_pConstantBufferPersist);
	pd3dImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);
	pd3dImmediateContext->PSSetConstantBuffers(0, 1, &g_pConstantBufferPerFrame);
	pd3dImmediateContext->PSSetConstantBuffers(1, 1, &g_pConstantBufferPersist);
	pd3dImmediateContext->DrawIndexed(36, 0, 0);

	//
	// Update variables for the second light
	//
	mLight = XMMatrixScaling(0.25f, 0.25f, 0.25f) * XMMatrixTranslationFromVector(4 * g_LightDir[1]);
	V(pd3dImmediateContext->Map(g_pConstantBufferPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	pCB = reinterpret_cast<ConstantBufferPerFrame*>(MappedResource.pData);
	XMStoreFloat4x4(&pCB->ModelViewProjection, XMMatrixTranspose(mLight * g_View * g_Projection));
	XMStoreFloat4x4(&pCB->Model, XMMatrixTranspose(mLight));

	XMStoreFloat4(&pCB->LightDir[0], g_LightDir[0]);
	XMStoreFloat4(&pCB->LightDir[1], g_LightDir[1]);
	pd3dImmediateContext->Unmap(g_pConstantBufferPerFrame, 0);

	//
	// Render the second light
	//
	pd3dImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBufferPerFrame);
	pd3dImmediateContext->VSSetConstantBuffers(1, 1, &g_pConstantBufferPersist);
	pd3dImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);
	pd3dImmediateContext->PSSetConstantBuffers(0, 1, &g_pConstantBufferPerFrame);
	pd3dImmediateContext->PSSetConstantBuffers(1, 1, &g_pConstantBufferPersist);
	pd3dImmediateContext->DrawIndexed(36, 0, 0);
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext) {
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice(void* pUserContext) {
	SAFE_RELEASE(g_pVertexShader);
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pConstantBufferPerFrame);
	SAFE_RELEASE(g_pConstantBufferPersist);
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext) {
	return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext) {
}

//--------------------------------------------------------------------------------------
// Handle mouse button presses
//--------------------------------------------------------------------------------------
void CALLBACK OnMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext) {
}

//--------------------------------------------------------------------------------------
// Call if device was removed.  Return true to find a new device, false to quit
//--------------------------------------------------------------------------------------
bool CALLBACK OnDeviceRemoved(void* pUserContext) {
	return true;
}

//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	// Enable run-time memory check for debug builds.
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// DXUT will create and use the best device
	// that is available on the system depending on which D3D callbacks are set below

	// Set general DXUT callbacks
	DXUTSetCallbackFrameMove(OnFrameMove);
	DXUTSetCallbackKeyboard(OnKeyboard);
	DXUTSetCallbackMouse(OnMouse);
	DXUTSetCallbackMsgProc(MsgProc);
	DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);
	DXUTSetCallbackDeviceRemoved(OnDeviceRemoved);

	// Set the D3D11 DXUT callbacks. Remove these sets if the app doesn't need to support D3D11
	DXUTSetCallbackD3D11DeviceAcceptable(IsD3D11DeviceAcceptable);
	DXUTSetCallbackD3D11DeviceCreated(OnD3D11CreateDevice);
	DXUTSetCallbackD3D11SwapChainResized(OnD3D11ResizedSwapChain);
	DXUTSetCallbackD3D11FrameRender(OnD3D11FrameRender);
	DXUTSetCallbackD3D11SwapChainReleasing(OnD3D11ReleasingSwapChain);
	DXUTSetCallbackD3D11DeviceDestroyed(OnD3D11DestroyDevice);

	// Perform any application-level initialization here

	DXUTInit(true, true, nullptr); // Parse the command line, show msgboxes on error, no extra command line params
	DXUTSetCursorSettings(true, true); // Show the cursor and clip it when in full screen
	DXUTCreateWindow(L"HelloHLSL");

	// Only require 10-level hardware or later
	DXUTCreateDevice(D3D_FEATURE_LEVEL_11_0, true, 800, 600);
	DXUTMainLoop(); // Enter into the DXUT ren  der loop

	// Perform any application-level cleanup here

	return DXUTGetExitCode();
}