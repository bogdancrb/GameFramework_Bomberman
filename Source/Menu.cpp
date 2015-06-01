#include "Menu.h"


MainMenu::MainMenu(BackBuffer* buffer, USHORT w, USHORT h)
{
	m_Buffer = buffer;
	m_Active = false;
	m_Width = w;
	m_Height = h;

	m_Menu = new Sprite("data/menu/image1.bmp", RGB(0xff, 0x00, 0xff));
	m_Menu->setBackBuffer(m_Buffer);

	m_Play = new Sprite("data/menu/Play.bmp", RGB(0xFF, 0x00, 0xFF));
	m_Play->setBackBuffer(m_Buffer);
	m_Quit = new Sprite("data/menu/quit.bmp", RGB(0xFF, 0x00, 0xFF));
	m_Quit->setBackBuffer(m_Buffer);
	
	m_Menu->mPosition = Vec2(w / 2, h / 2);
	m_Play->mPosition = Vec2(340,230);
	m_Quit->mPosition = Vec2(1027,428);
	

}

void MainMenu::Draw()
{
	m_Menu->draw();
	m_Play->draw();
	m_Quit->draw();
}

InGameMenu::InGameMenu(BackBuffer* buffer, USHORT w, USHORT h)
{
	m_Buffer = buffer;
	m_S_M_Active = false;
	m_Width = w;
	m_Height = h;

	In_game_Menu_button = new Sprite("data/menu/in_game_button.bmp", RGB(0xFF, 0x00, 0xFF));
	In_game_Menu_button->setBackBuffer(m_Buffer);
	Popup_menu= new Sprite("data/menu/Popup_menu.bmp", RGB(0xFF, 0x00, 0xFF));
	Popup_menu->setBackBuffer(m_Buffer);
	
	In_game_Menu_button->mPosition = Vec2(1345,25);
	Popup_menu->mPosition = Vec2(w / 2, h / 2);


}

void InGameMenu::Draw()
{
	In_game_Menu_button->draw();
	
	if (m_S_M_Active)
	{
		Popup_menu->draw();
	}

}