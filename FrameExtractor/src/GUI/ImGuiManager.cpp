/******************************************************************************
/*!
\file       ImGuiManager.cpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 10, 2024
\brief      Defines the ImGui Manager class which manages the overall ImGui
			interface and its components.

 /******************************************************************************/

#include <FrameExtractorPCH.hpp>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <backends/imgui_impl_glfw.cpp>
#include <backends/imgui_impl_opengl3.cpp>

#include <imgui.h>

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <Core/ApplicationManager.hpp>
#include <Core/LoggerManager.hpp>
#include <GUI/ImGuiManager.hpp>
#include <GUI/ViewportPanel.hpp>
#include <GUI/ToolsPanel.hpp>
#include <GUI/ConsolePanel.hpp>
#include <GLFW/glfw3.h>


namespace FrameExtractor
{

	class EditorColorScheme
	{
		// 0xRRGGBBAA
		inline static int BackGroundColor = 0x25213100;
		inline static int TextColor = 0xF4F1DE00;
		inline static int MainColor = 0xDA115E00;
		inline static int MainAccentColor = 0x79235900;
		inline static int HighlightColor = 0xC7EF0000;
		inline static int Black = 0x00000000;
		inline static int White = 0xFFFFFF00;

		inline static int AlphaTransparent = 0x00;
		inline static int Alpha20 = 0x33;
		inline static int Alpha40 = 0x66;
		inline static int Alpha50 = 0x80;
		inline static int Alpha60 = 0x99;
		inline static int Alpha80 = 0xCC;
		inline static int Alpha90 = 0xE6;
		inline static int AlphaFull = 0xFF;

		static float GetR(int colorCode) { return (float)((colorCode & 0xFF000000) >> 24) / (float)(0xFF); }
		static float GetG(int colorCode) { return (float)((colorCode & 0x00FF0000) >> 16) / (float)(0xFF); }
		static float GetB(int colorCode) { return (float)((colorCode & 0x0000FF00) >> 8) / (float)(0xFF); }
		static float GetA(int alphaCode) { return ((float)alphaCode / (float)0xFF); }

		static ImVec4 GetColor(int c, int a = Alpha80) { return ImVec4(GetR(c), GetG(c), GetB(c), GetA(a)); }
		static ImVec4 Darken(ImVec4 c, float p) { return ImVec4(fmax(0.f, c.x - 1.0f * p), fmax(0.f, c.y - 1.0f * p), fmax(0.f, c.z - 1.0f * p), c.w); }
		static ImVec4 Lighten(ImVec4 c, float p) { return ImVec4(fmax(0.f, c.x + 1.0f * p), fmax(0.f, c.y + 1.0f * p), fmax(0.f, c.z + 1.0f * p), c.w); }

		static ImVec4 Disabled(ImVec4 c) { return Darken(c, 0.6f); }
		static ImVec4 Hovered(ImVec4 c) { return Lighten(c, 0.4f); }
		static ImVec4 Active(ImVec4 c) { return Lighten(ImVec4(c.x, c.y, c.z, 1.0f), 0.3f); }
		static ImVec4 Collapsed(ImVec4 c) { return Darken(c, 0.2f); }

	public:

		static void SetColors(int backGroundColor, int textColor, int mainColor, int mainAccentColor, int highlightColor)
		{
			BackGroundColor = backGroundColor;
			TextColor = textColor;
			MainColor = mainColor;
			MainAccentColor = mainAccentColor;
			HighlightColor = highlightColor;
		}

		static void ApplyTheme()
		{
			ImVec4* colors = ImGui::GetStyle().Colors;

			colors[ImGuiCol_Text] = GetColor(TextColor);
			colors[ImGuiCol_TextDisabled] = Disabled(colors[ImGuiCol_Text]);
			colors[ImGuiCol_WindowBg] = GetColor(BackGroundColor);
			colors[ImGuiCol_ChildBg] = GetColor(Black, Alpha20);
			colors[ImGuiCol_PopupBg] = GetColor(BackGroundColor, Alpha90);
			colors[ImGuiCol_Border] = Lighten(GetColor(BackGroundColor), 0.4f);
			colors[ImGuiCol_BorderShadow] = GetColor(Black);
			colors[ImGuiCol_FrameBg] = GetColor(MainAccentColor, Alpha40);
			colors[ImGuiCol_FrameBgHovered] = Hovered(colors[ImGuiCol_FrameBg]);
			colors[ImGuiCol_FrameBgActive] = Active(colors[ImGuiCol_FrameBg]);
			colors[ImGuiCol_TitleBg] = GetColor(BackGroundColor, Alpha80);
			colors[ImGuiCol_TitleBgActive] = Active(colors[ImGuiCol_TitleBg]);
			colors[ImGuiCol_TitleBgCollapsed] = Collapsed(colors[ImGuiCol_TitleBg]);
			colors[ImGuiCol_MenuBarBg] = Darken(GetColor(BackGroundColor), 0.2f);
			colors[ImGuiCol_ScrollbarBg] = Lighten(GetColor(BackGroundColor, Alpha50), 0.4f);
			colors[ImGuiCol_ScrollbarGrab] = Lighten(GetColor(BackGroundColor), 0.3f);
			colors[ImGuiCol_ScrollbarGrabHovered] = Hovered(colors[ImGuiCol_ScrollbarGrab]);
			colors[ImGuiCol_ScrollbarGrabActive] = Active(colors[ImGuiCol_ScrollbarGrab]);
			colors[ImGuiCol_CheckMark] = GetColor(HighlightColor);
			colors[ImGuiCol_SliderGrab] = GetColor(HighlightColor);
			colors[ImGuiCol_SliderGrabActive] = Active(colors[ImGuiCol_SliderGrab]);
			colors[ImGuiCol_Button] = GetColor(MainColor, Alpha80);
			colors[ImGuiCol_ButtonHovered] = Hovered(colors[ImGuiCol_Button]);
			colors[ImGuiCol_ButtonActive] = Active(colors[ImGuiCol_Button]);
			colors[ImGuiCol_Header] = GetColor(MainAccentColor, Alpha80);
			colors[ImGuiCol_HeaderHovered] = Hovered(colors[ImGuiCol_Header]);
			colors[ImGuiCol_HeaderActive] = Active(colors[ImGuiCol_Header]);
			colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
			colors[ImGuiCol_SeparatorHovered] = Hovered(colors[ImGuiCol_Separator]);
			colors[ImGuiCol_SeparatorActive] = Active(colors[ImGuiCol_Separator]);
			colors[ImGuiCol_ResizeGrip] = GetColor(MainColor, Alpha20);
			colors[ImGuiCol_ResizeGripHovered] = Hovered(colors[ImGuiCol_ResizeGrip]);
			colors[ImGuiCol_ResizeGripActive] = Active(colors[ImGuiCol_ResizeGrip]);
			colors[ImGuiCol_Tab] = GetColor(MainColor, Alpha60);
			colors[ImGuiCol_TabHovered] = Hovered(colors[ImGuiCol_Tab]);
			colors[ImGuiCol_TabActive] = Active(colors[ImGuiCol_Tab]);
			colors[ImGuiCol_TabUnfocused] = colors[ImGuiCol_Tab];
			colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_TabActive];
			colors[ImGuiCol_DockingPreview] = Darken(colors[ImGuiCol_HeaderActive], 0.2f);
			colors[ImGuiCol_DockingEmptyBg] = Darken(colors[ImGuiCol_HeaderActive], 0.6f);
			colors[ImGuiCol_PlotLines] = GetColor(HighlightColor);
			colors[ImGuiCol_PlotLinesHovered] = Hovered(colors[ImGuiCol_PlotLines]);
			colors[ImGuiCol_PlotHistogram] = GetColor(HighlightColor);
			colors[ImGuiCol_PlotHistogramHovered] = Hovered(colors[ImGuiCol_PlotHistogram]);
			colors[ImGuiCol_TextSelectedBg] = GetColor(HighlightColor, Alpha40);
			colors[ImGuiCol_DragDropTarget] = GetColor(HighlightColor, Alpha80);;
			colors[ImGuiCol_NavHighlight] = GetColor(White);
			colors[ImGuiCol_NavWindowingHighlight] = GetColor(White, Alpha80);
			colors[ImGuiCol_NavWindowingDimBg] = GetColor(White, Alpha20);
			colors[ImGuiCol_ModalWindowDimBg] = GetColor(Black, Alpha60);

			ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_Right;

		}
	};

	float ImGuiManager::styleMultiplier = 1.f;
	float ImGuiManager::mFontSize = 1.f;
	ImFont* ImGuiManager::BoldFont = nullptr;
	ImFont* ImGuiManager::LightFont = nullptr;
	ImFont* ImGuiManager::RegularFont = nullptr;
	ImFont* ImGuiManager::FrameRateFont = nullptr;

	ImGuiManager::ImGuiManager()
	{
		Init();

		{
			std::string name = "Viewport";
			ImVec2 size = ImVec2{ 1000,800 };
			ImVec2 pos = ImVec2{ 0,0 };
			mViewportPanel = new ViewportPanel(name, size, pos);
			mViewportPanel->OnAttach();
		}

		{
			std::string name = "Tools";
			ImVec2 size = ImVec2{ 1000,800 };
			ImVec2 pos = ImVec2{ 0,0 };
			mToolsPanel = new ToolsPanel(name, size, pos);
			mToolsPanel->OnAttach();
		}

		{
			std::string name = "Console";
			ImVec2 size = ImVec2{ 1000,800 };
			ImVec2 pos = ImVec2{ 0,0 };
			mConsolePanel = new ConsolePanel();
			mConsolePanel->OnAttach();
			LoggerManager::SetConsole(mConsolePanel);
		}
	}

	ImGuiManager::~ImGuiManager()
	{
		mConsolePanel->OnDetach();
		mToolsPanel->OnDetach();
		mViewportPanel->OnDetach();

		delete mConsolePanel;
		delete mToolsPanel;
		delete mViewportPanel;
		Shutdown();
	}

	void ImGuiManager::Init()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); // Use if you want to access ImGui's IO
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)ApplicationManager::GetInstance()->GetWindowManager()->GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 410");


		EditorColorScheme::ApplyTheme();

		BoldFont = io.Fonts->AddFontFromFileTTF("resources/fonts/OpenSans-Bold.ttf", 24.f);
		LightFont = io.Fonts->AddFontFromFileTTF("resources/fonts/OpenSans-Light.ttf", 24.f);
		RegularFont = io.Fonts->AddFontFromFileTTF("resources/fonts/OpenSans-Regular.ttf", 24.f);
		FrameRateFont = io.Fonts->AddFontFromFileTTF("resources/fonts/OpenSans-Bold.ttf", 32.f);
		io.FontDefault = RegularFont;
	}

	void ImGuiManager::Shutdown()
	{
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiManager::Update(float dt)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		bool p_open = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;



		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &p_open, window_flags);
		ImGui::PopStyleVar();

		auto windowSize = ImGui::GetWindowSize();
		styleMultiplier = windowSize.x / 1920;
		ImGui::GetIO().FontGlobalScale = styleMultiplier * mFontSize;

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);	

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f * styleMultiplier, 10.0f * styleMultiplier));

		mViewportPanel->OnImGuiRender(dt);
		mToolsPanel->OnImGuiRender(dt);
		mConsolePanel->OnImGuiRender(dt);

		ImGui::PopStyleVar();
		ImGui::End();
	}

	void ImGuiManager::Render()
	{

		ImGuiIO& io = ImGui::GetIO();
		ApplicationManager* app = ApplicationManager::GetInstance();
		io.DisplaySize = ImVec2(static_cast<float>(app->GetWindowManager()->GetWidth()), static_cast<float>(app->GetWindowManager()->GetHeight()));

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}
}
