#pragma once


class Application
{
public:
	~Application() noexcept;

	BOOL OnInit(HWND hWnd, int width, int height);
	void OnUpdate();
	void OnDestroy();

private:
	HMODULE m_hRenderSystemModule = nullptr;
	HWND m_hWnd = nullptr;
	int m_Width = 0;
	int m_Height = 0;

};

