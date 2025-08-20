/******************************************************************************/
/*!
\file       GUIResourcesManager.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 22, 2025
\brief      Declares the Resources Manager that contains all resources needed for
			GUI.
 ******************************************************************************/
 // Standard Library includes
#include <unordered_map>

// Project includes
#include <Core/Core.hpp>
#include <Graphics/Texture.hpp>
namespace FrameExtractor
{
	/*!***********************************************************************
		\brief
			Enum that contains all the icons used in the GUI.
	*************************************************************************/
	enum Icon
	{
		//Keys
		CTRL_ICON,
		SHIFT_ICON,
		RIGHTARROW_ICON,
		DOWNARROW_ICON,
		LEFTARROW_ICON,
		UPARROW_ICON,
		SPACE_ICON,
		HOME_ICON,
		END_ICON,

		PLUS_LOGO,
		PLAY_ICON,
		STOP_ICON,
		FORWARD_ICON,
		BACKWARD_ICON,
		SPEED_UP_ICON,
		SLOW_DOWN_ICON,
		SKIP_TO_END_ICON,
		SKIP_TO_START_ICON,
		FOLDER_ICON,
		FILE_ICON,
		IMPORT_ICON,
		EXPORT_ICON,
		ADDFILE_ICON,
		CLEAR_ICON,
		SETTINGS_ICON,

		TOTAL_ICONS
	};

	/*!***********************************************************************
		\brief
			Class that manages all GUI resources such as icons.
	*************************************************************************/
	class GUIResourcesManager
	{
	public:

		GUIResourcesManager(const GUIResourcesManager&) = delete;

		/*!***********************************************************************
			\brief
				Default destructor for GUIResourcesManager class.
		*************************************************************************/
		~GUIResourcesManager() {};

		/*!***********************************************************************
			\brief
				Gets the singleton instance of GUIResourcesManager.
			\return
				Reference to the singleton instance of GUIResourcesManager.
		*************************************************************************/
		static GUIResourcesManager& GetInstance()
		{
			static GUIResourcesManager instance;
			return instance;
		}

		/*!***********************************************************************
			\brief
				Gets the icon texture for a specific icon.
			\param[in] icon
				The icon to get the texture for.
			\return
				Reference to the texture of the specified icon.
		*************************************************************************/
		Ref<Texture> GetIcon(Icon icon) { return mIcons[icon]; }
	private:

		/*!***********************************************************************
			\brief
				Default constructor for GUIResourcesManager class.
				Loads all the icons used in the GUI.
		*************************************************************************/
		GUIResourcesManager()
		{
			mIcons[Icon::CTRL_ICON] = MakeRef<Texture>("resources/icons/Ctrl.png");
			mIcons[Icon::SHIFT_ICON] = MakeRef<Texture>("resources/icons/Shift.png");
			mIcons[Icon::LEFTARROW_ICON] = MakeRef<Texture>("resources/icons/leftArrow.png");
			mIcons[Icon::RIGHTARROW_ICON] = MakeRef<Texture>("resources/icons/rightArrow.png");
			mIcons[Icon::UPARROW_ICON] = MakeRef<Texture>("resources/icons/upArrow.png");
			mIcons[Icon::DOWNARROW_ICON] = MakeRef<Texture>("resources/icons/downArrow.png");
			mIcons[Icon::SPACE_ICON] = MakeRef<Texture>("resources/icons/Space.png");
			mIcons[Icon::PLUS_LOGO] = MakeRef<Texture>("resources/icons/PlusLogo.png");
			mIcons[Icon::HOME_ICON] = MakeRef<Texture>("resources/icons/Home.png");
			mIcons[Icon::END_ICON] = MakeRef<Texture>("resources/icons/End.png");

			mIcons[Icon::PLAY_ICON] = MakeRef<Texture>("resources/icons/Play.png");
			mIcons[Icon::STOP_ICON] = MakeRef<Texture>("resources/icons/Stop.png");
			mIcons[Icon::FORWARD_ICON] = MakeRef<Texture>("resources/icons/MoveFrameRight.png");
			mIcons[Icon::BACKWARD_ICON] = MakeRef<Texture>("resources/icons/MoveFrameLeft.png");
			mIcons[Icon::SPEED_UP_ICON] = MakeRef<Texture>("resources/icons/FastForward.png");
			mIcons[Icon::SLOW_DOWN_ICON] = MakeRef<Texture>("resources/icons/FastBackward.png");
			mIcons[Icon::SKIP_TO_END_ICON] = MakeRef<Texture>("resources/icons/CutToEnd.png");
			mIcons[Icon::SKIP_TO_START_ICON] = MakeRef<Texture>("resources/icons/CutToFront.png");
			mIcons[Icon::FOLDER_ICON] = MakeRef<Texture>("resources/icons/folder.png");
			mIcons[Icon::FILE_ICON] = MakeRef<Texture>("resources/icons/document.png");
			mIcons[Icon::IMPORT_ICON] = MakeRef<Texture>("resources/icons/import.png");
			mIcons[Icon::EXPORT_ICON] = MakeRef<Texture>("resources/icons/export.png");
			mIcons[Icon::ADDFILE_ICON] = MakeRef<Texture>("resources/icons/addfile.png");
			mIcons[Icon::CLEAR_ICON] = MakeRef<Texture>("resources/icons/clear.png");
			mIcons[Icon::SETTINGS_ICON] = MakeRef<Texture>("resources/icons/settings.png");

			for (auto& icon : mIcons)
			{
				icon.second->Load();
			}
		}
		std::unordered_map<Icon, Ref<Texture>> mIcons;
	};
}
#define Resource(IconName) GUIResourcesManager::GetInstance().GetIcon(IconName)