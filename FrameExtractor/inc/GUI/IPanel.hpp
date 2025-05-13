/******************************************************************************
/*!
\file       IPanel.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 10, 2024
\brief      Declares the Panel virtual interface for all panels

 /******************************************************************************/

#ifndef IPanel_HPP
#define IPanel_HPP

namespace FrameExtractor
{
	class IPanel
	{
	public:
		virtual ~IPanel() = default;
		virtual void OnImGuiRender(float dt) = 0;
		virtual const char* GetName() const = 0;
		virtual bool IsVisible() const { return mVisible; }
		virtual void SetVisible(bool visible) { mVisible = visible; }
		virtual void OnAttach() {};
		virtual void OnDetach() {};

	protected:
		bool mVisible = true;
	};

}

#endif
