//-----------------------------------------------------------------------------
// File: CMap.h
//
// Desc: Map loading and colisions
//
//-----------------------------------------------------------------------------


#pragma once

#include "Main.h"
#include "ResizeEngine.h"
#include "CPlayer.h"
#include <vector>
#include <fstream>
#include <iostream>

class CMap;

class Object
{
	friend class CMap;

public:
	Object(int i, int j, char code);
	~Object(){}
	void					Update();
	void					Draw();
	bool					m_Visible;  //true pentru zid vizibil si true pentru powerup invizibil
	char&					getCode() { return m_Code; }
	void					Change(char val); //schimba m_code
	int&					GetI() { return m_I; }
	int&					GetJ() { return m_J; }

private:
	int						m_I;       // Indice matrice
	int						m_J;		// Indice matrice
	char   					m_Code;		// Cod pentru verificare tip block
};


class CMap
{
public:
	enum ELEMENT
	{
		DIRT = '0',
		WALL = '1',
		D_BOX = 'd',
		I_BOX = 'i'
	};

	CMap(const char*, BackBuffer *); 
	~CMap(void);

	int											Width(){ return m_BgImage.Width(); }
	int											Height(){ return m_BgImage.Height(); }

	void										Draw(double x, double y);   //x si y sunt coordonatele de la care incepe desenarea
	void										Change(char val, int i, int j);  //schimba in harta elementul de la poz. i, j la codul val
	void										LoadElement(char val, int x, int y);

	Object*										GetElement(Vec2 pos);
	void										GeneratePowerups();

	std::vector<std::vector<Object*> >*			GetMap() { return &m_MapMatrix; }
	BackBuffer*									GetBgBuffer(){ return m_BgBuffer; }
	double										GetXOffset() { return xOffset; }
	double										GetYOffset() { return yOffset; }
	Sprite*										GetWall(int i) { return m_Wall[i]; }

	void										Colision(CPlayer*, Vec2 OldPos);	// Detectare coliziune intre jucator si block

	std::vector<int>							OpenMap(const char* FileName);	// Deschidere harta .txt

private:
	std::vector<Sprite*>						m_Wall;
	std::vector<Sprite*>						m_indestructable_box;
	std::vector<Sprite*>						m_destructable_box;

	std::vector<std::vector<Object*>>			m_MapMatrix;	// Matrice pentru incarcare harta
	std::vector<int>							NrOfWalls;	// Numarul de block-uri de fiecare tip

	double										xOffset;
	double										yOffset;

	CResizableImage								m_BgImage;
	BackBuffer*									m_BgBuffer;

	void										DrawBackground();
	void										DrawEnviroment();
};


