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
#include <Core/Command.hpp>
#include <Core/LoggerManager.hpp>
#include <Core/PlatformUtils.hpp>
#include <Core/ExcelSerialiser.hpp>
#include <Core/YAMLSerialiser.hpp>
#include <GUI/ImGuiManager.hpp>
#include <GUI/ViewportPanel.hpp>
#include <GUI/ToolsPanel.hpp>
#include <GUI/ProjectPanel.hpp>
#include <GUI/ConsolePanel.hpp>
#include <GUI/ExplorerPanel.hpp>
#include <GLFW/glfw3.h>


namespace FrameExtractor
{

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
		if (key == GLFW_KEY_Z && action == GLFW_PRESS)
			if (ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
				CommandHistory::undo();

		if (key == GLFW_KEY_Y && action == GLFW_PRESS)
			if (ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
				CommandHistory::redo();
	}
	class EditorColorScheme
	{
		// 0xRRGGBBAA
	public:
		inline static int BackGroundColor = 0x25213100;
		inline static int TextColor = 0xF4F1DE00;
		inline static int MainColor = 0xDA115E00;
		inline static int MainAccentColor = 0x79235900;
		inline static int HighlightColor = 0xC7EF0000;
		inline static int Black = 0x00000000;
		inline static int White = 0xFFFFFF00;
		static ImVec4 GetColor(int c, int a = Alpha80) { return ImVec4(GetR(c), GetG(c), GetB(c), GetA(a)); }
		static int GetColor(const ImVec4& color) {
			int r = static_cast<int>(color.x * 255.0f) & 0xFF;
			int g = static_cast<int>(color.y * 255.0f) & 0xFF;
			int b = static_cast<int>(color.z * 255.0f) & 0xFF;
			int a = static_cast<int>(color.w * 255.0f) & 0xFF;

			return (r << 24) | (g << 16) | (b << 8) | a;
		}
	private:
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
			colors[ImGuiCol_PopupBg] = GetColor(BackGroundColor, AlphaFull);
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
			colors[ImGuiCol_ModalWindowDimBg] = GetColor(Black, Alpha20);

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
		glfwSetKeyCallback((GLFWwindow*)ApplicationManager::GetInstance()->GetWindowManager()->GetNativeWindow(), key_callback);
		{
			std::string name = "Viewport";
			ImVec2 size = ImVec2{ 1000,800 };
			ImVec2 pos = ImVec2{ 0,0 };
			mViewportPanel = new ViewportPanel(name, size, pos);
			mViewportPanel->OnAttach();
		}

		{
			mConsolePanel = new ConsolePanel();
			mConsolePanel->OnAttach();
			LoggerManager::SetConsole(mConsolePanel);
		}

		{
			mExplorerPanel = new ExplorerPanel();
			mExplorerPanel->OnAttach();
		}

		{
			mToolsPanel = new ToolsPanel();
			mToolsPanel->OnAttach();
		}

		{
			mProjectPanel = new ProjectPanel(mExplorerPanel, mViewportPanel);
			mProjectPanel->OnAttach();
		}
	}

	ImGuiManager::~ImGuiManager()
	{
		mToolsPanel->OnDetach();
		mProjectPanel->OnDetach();
		mExplorerPanel->OnDetach();
		mConsolePanel->OnDetach();
		mViewportPanel->OnDetach();

		delete mToolsPanel;
		delete mProjectPanel;
		delete mExplorerPanel;
		delete mConsolePanel;
		delete mViewportPanel;
		Shutdown();
	}

	void ImGuiManager::Init()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); // Use if you want to access ImGui's IO
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

		try {
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

			static bool open_preferences_popup = false;

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace", &p_open, window_flags);

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Open Project")) {
						auto projectFile = OpenFileDialog("FrameEX File (*.FrEX)\0*.FrEX\0");
						if (std::filesystem::exists(projectFile))
						{
							YAMLSerialiser serialiser(projectFile);
							serialiser.ImportProject(mToolsPanel->GetData());
						}
						else
						{
							APP_CORE_ERROR("Spike Dip file does not exist!");
						}
					}

					if (ImGui::MenuItem("Save Project"))
					{
						auto spikeDipFile = SaveFileDialog("FrameEX File (*.FrEX)\0*.FrEX\0");
						spikeDipFile.replace_extension(".FrEX");
						if (!spikeDipFile.empty())
						{
							YAMLSerialiser serialiser(spikeDipFile);
							serialiser.ExportProject(mToolsPanel->GetData());
						}

					}

					if (ImGui::MenuItem("Import Spike Dip"))
					{
						auto spikeDipFile = OpenFileDialog("Excel File (*.xlsx)\0*.xlsx\0");
						if (std::filesystem::exists(spikeDipFile))
						{
							ExcelSerialiser serialiser(spikeDipFile);
							mToolsPanel->SetData(serialiser.ImportSpikeDipReport());
						}
						else
						{
							APP_CORE_ERROR("Spike Dip file does not exist!");
						}
					}

					if (ImGui::MenuItem("Preferences...##MainMenuPreference"))
					{
						open_preferences_popup = true;
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Edit"))
				{
					if (ImGui::MenuItem("Undo")) { /* do something */ }
					ImGui::EndMenu();
				}


				ImGui::EndMenuBar();
			}

			ImGui::PopStyleVar();

			if (open_preferences_popup)
			{
				ImGui::OpenPopup("Preferences##Menu");
				open_preferences_popup = false;
			}ImGuiIO& io = ImGui::GetIO();
			ImVec2 center = { io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f };
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize({ 900 * styleMultiplier , 600 * styleMultiplier }, ImGuiCond_Appearing);
			if (ImGui::BeginPopupModal("Preferences##Menu", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
			{
				dt = 0;
				float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;

				if (ImGui::Button("X##PreferencesClosing", { lineHeight, lineHeight }))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::Separator();
				if (ImGui::BeginTabBar("PreferencesTabs"))
				{
					if (ImGui::BeginTabItem("General"))
					{
						bool dark_mode = false;
						ImGui::Checkbox("Enable Dark Mode", &dark_mode);
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Apperances"))
					{
						auto vec4BGColor = EditorColorScheme::GetColor(EditorColorScheme::BackGroundColor);
						auto vec4MainColor = EditorColorScheme::GetColor(EditorColorScheme::MainColor);
						auto vec4AccentColor = EditorColorScheme::GetColor(EditorColorScheme::MainAccentColor);
						auto vec4TextColor = EditorColorScheme::GetColor(EditorColorScheme::TextColor);
						auto vec4HighlightColor = EditorColorScheme::GetColor(EditorColorScheme::HighlightColor);
						if (ImGui::Button("Reset to Default## Appearances"))
						{
							EditorColorScheme::SetColors(0x25213100, 0xF4F1DE00, 0xDA115E00, 0x79235900, 0xC7EF0000);
							EditorColorScheme::ApplyTheme();
							mFontSize = 1;
						}
						auto contentRegionAvail = ImGui::GetContentRegionAvail();
						ImGui::BeginChild("AppearancesChild", contentRegionAvail, true);
						ImGui::Columns(2);
						ImGui::SetColumnWidth(0, ImGui::GetFontSize() * 8);
						ImGui::Text("Background Colour: ");
						ImGui::NextColumn();
						ImGui::SetNextItemWidth(contentRegionAvail.x - ImGui::GetFontSize() * 8 - ImGui::GetStyle().FramePadding.x * 4.f);
						if (ImGui::ColorEdit4("##Background Colour", &vec4BGColor.x, ImGuiColorEditFlags_NoAlpha))
						{
							EditorColorScheme::BackGroundColor = EditorColorScheme::GetColor(vec4BGColor);
							EditorColorScheme::ApplyTheme();
						}
						ImGui::NextColumn();
						ImGui::Text("Text Colour: ");
						ImGui::NextColumn();
						ImGui::SetNextItemWidth(contentRegionAvail.x - ImGui::GetFontSize() * 8 - ImGui::GetStyle().FramePadding.x * 4.f);
						if (ImGui::ColorEdit4("##Text Colour", &vec4TextColor.x, ImGuiColorEditFlags_NoAlpha))
						{
							EditorColorScheme::TextColor = EditorColorScheme::GetColor(vec4TextColor);
							EditorColorScheme::ApplyTheme();
						}
						ImGui::NextColumn();
						ImGui::Text("Main Colour: ");
						ImGui::NextColumn();
						ImGui::SetNextItemWidth(contentRegionAvail.x - ImGui::GetFontSize() * 8 - ImGui::GetStyle().FramePadding.x * 4.f);
						if (ImGui::ColorEdit4("##Main Colour", &vec4MainColor.x, ImGuiColorEditFlags_NoAlpha))
						{
							EditorColorScheme::MainColor = EditorColorScheme::GetColor(vec4MainColor);
							EditorColorScheme::ApplyTheme();
						}
						ImGui::NextColumn();
						ImGui::Text("Highlight Colour: ");
						ImGui::NextColumn();
						ImGui::SetNextItemWidth(contentRegionAvail.x - ImGui::GetFontSize() * 8 - ImGui::GetStyle().FramePadding.x * 4.f);
						if (ImGui::ColorEdit4("##Highlight Colour", &vec4HighlightColor.x, ImGuiColorEditFlags_NoAlpha))
						{
							EditorColorScheme::HighlightColor = EditorColorScheme::GetColor(vec4HighlightColor);
							EditorColorScheme::ApplyTheme();
						}
						ImGui::NextColumn();
						ImGui::Text("Main Accent Colour: ");
						ImGui::NextColumn();
						ImGui::SetNextItemWidth(contentRegionAvail.x - ImGui::GetFontSize() * 8 - ImGui::GetStyle().FramePadding.x * 4.f);
						if (ImGui::ColorEdit4("##Main Accent Colour", &vec4AccentColor.x, ImGuiColorEditFlags_NoAlpha))
						{
							EditorColorScheme::MainAccentColor = EditorColorScheme::GetColor(vec4AccentColor);
							EditorColorScheme::ApplyTheme();
						}
						ImGui::NextColumn();
						ImGui::Text("Font Size: ");
						ImGui::NextColumn();
						float displayFontSize = 32 * mFontSize;
						ImGui::SetNextItemWidth(contentRegionAvail.x - ImGui::GetFontSize() * 8 - ImGui::GetStyle().FramePadding.x * 4.f);
						if (ImGui::DragFloat("##FontSize", &displayFontSize, 0.3f, 20, 64, "%.1f"))
						{
							if (displayFontSize < 20) displayFontSize = 20;
							if (displayFontSize > 64) displayFontSize = 64;
							mFontSize = displayFontSize / 32;
						}
						ImGui::Columns(1);

						auto remainingSize = ImGui::GetContentRegionAvail();
						ImGui::BeginChild("AppearancesChildExample", remainingSize, true);

						ImGui::PushFont(BoldFont);
						ImGui::Text("Example Window Background");
						ImGui::PopFont();
						// Text
						ImGui::Text("This is a text sample");

						// Main Button
						ImGui::Button("Main Color Button");

						// Accent Color (e.g. Frame)
						static float slider_value = 0.5f;
						ImGui::SliderFloat("Accent Slider", &slider_value, 0.0f, 1.0f);

						// Highlight (e.g. checkmark, selection)
						static bool check = true;
						ImGui::Checkbox("Highlight Check", &check);

						ImGui::EndChild();
						ImGui::EndChild();
						ImGui::EndTabItem();
					}
					ImGui::EndTabBar();
				}
				ImGui::EndPopup();
			}

			auto windowSize = ImGui::GetWindowSize();
			styleMultiplier = windowSize.x / 1920;
			ImGui::GetIO().FontGlobalScale = styleMultiplier * mFontSize;

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f * styleMultiplier, 10.0f * styleMultiplier));
			mToolsPanel->OnImGuiRender(dt);
			mExplorerPanel->OnImGuiRender(dt);
			mProjectPanel->OnImGuiRender(dt);
			mViewportPanel->OnImGuiRender(dt);
			mConsolePanel->OnImGuiRender(dt);

			ImGui::PopStyleVar();
			ImGui::End();

		}
		catch (...)
		{
			YAMLSerialiser serialiser("Autosave.FrEX");
			serialiser.ExportProject(mToolsPanel->GetData());
			throw ("Exception!");
		}
		
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
