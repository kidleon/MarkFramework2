#pragma once


class D3D11Application
{
public:
	~D3D11Application() noexcept;

	BOOL OnInit(HWND hWnd, int width, int height);
	void OnUpdate();
	void OnDestroy();

private:
	HWND m_hWnd = nullptr;
	int m_Width = 0;
	int m_Height = 0;

	IRenderSystem* m_pRenderSystem = nullptr;
	IRenderCamera* m_pRenderCamera = nullptr;
	IShaderProgram* m_pShaderProgram_VS = nullptr;
	IShaderProgram* m_pShaderProgram_PS = nullptr;

};

