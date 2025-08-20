/******************************************************************************/
/*!
\file       IPanel.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 10, 2025
\brief      Declares the Panel virtual interface for all panels

 ******************************************************************************/

#ifndef IPanel_HPP
#define IPanel_HPP

namespace FrameExtractor
{
	/*!***********************************************************************
		\brief
			Interface for all panels in the GUI.
	*************************************************************************/
	class IPanel
	{
	public:

		/*!***********************************************************************
			\brief
				Virtual destructor for IPanel class.
		*************************************************************************/
		virtual ~IPanel() = default;

		/*!***********************************************************************
			\brief
				Pure virtual function to render the ImGui interface for the panel.
			\param[in] dt
				The delta time since the last frame.
		*************************************************************************/
		virtual void OnImGuiRender(float dt) = 0;

		/*!***********************************************************************
			\brief
				Pure virtual function to get the name of the panel.
			\return
				The name of the panel as a string.
		*************************************************************************/
		virtual const char* GetName() const = 0;

		/*!***********************************************************************
			\brief
				Pure virtual function to check if the panel is visible.
			\return
				True if the panel is visible, false otherwise.
		*************************************************************************/
		bool IsVisible() const { return mVisible; }

		/*!***********************************************************************
			\brief
				Pure virtual function to set the visibility of the panel.
			\param[in] visible
				The visibility state to set for the panel.
		*************************************************************************/
		void SetVisible(bool visible) { mVisible = visible; }

		/*!***********************************************************************
			\brief
				Pure virtual callback function that is called when the panel is attached.
		*************************************************************************/
		virtual void OnAttach() {};

		/*!***********************************************************************
			\brief
				Pure virtual callback function that is called when the panel is detached.
		*************************************************************************/
		virtual void OnDetach() {};

	protected:
		bool mVisible = true; // Visibility state of the panel
	};
}

#endif