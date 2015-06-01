//-----------------------------------------------------------------------------
// File: CMap.h
//
// Desc: Map loading and colisions
//
//-----------------------------------------------------------------------------

#ifndef _CMAP_H_
#define _CMAP_H_

//-----------------------------------------------------------------------------
// CBomb Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "Sprite.h"
#include "ResizeEngine.h"

//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CMap (Class)
// Desc : Map class handles all objects on map
//-----------------------------------------------------------------------------
class CMap;

class Object
{
	friend class CMap;

public:
	Object(int i, int j, char code);
	//~Object();

	void					Update();
	void					Draw();
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
	//-------------------------------------------------------------------------
	// Enumerators
	//-------------------------------------------------------------------------
	enum ELEMENT
	{
		DIRT = '0',
		WALL = '1',
		D_BOX = 'd',
		I_BOX = 'i'
	};

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	CMap(const char*, BackBuffer *); 
	~CMap(void);


	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	int											Width(){ return m_BgImage.Width(); }
	int											Height(){ return m_BgImage.Height(); }

	void										Draw(double x, double y);   //x si y sunt coordonatele de la care incepe desenarea
	void										Change(char val, int i, int j);  //schimba in harta elementul de la poz. i, j la codul val
	void										LoadElement(char val, int x, int y);

	Object*										GetElement(Vec2 pos);
	
	vector<vector<Object*>>*					GetMap() { return &m_MapMatrix; }
	BackBuffer*									GetBgBuffer(){ return m_BgBuffer; }
	double										GetXOffset() { return xOffset; }
	double										GetYOffset() { return yOffset; }
	Sprite*										GetWall(int i) { return m_Wall[i]; }

	Vec2&										WallPosition(int i) { return m_Wall[i]->mPosition; }
	Vec2&										IndesctructPosition(int i) { return m_indestructable_box[i]->mPosition; }
	Vec2&										DesctructPosition(int i) { return m_destructable_box[i]->mPosition; }

	bool&										isDesctructVisible(int i) { return m_destructable_box[i]->isSpriteVisible; }

	void										GeneratePowerups();

	vector<int>									OpenMap(const char* FileName);	// Deschidere harta .txt

private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	vector<Sprite*>								m_Wall;
	vector<Sprite*>								m_indestructable_box;
	vector<Sprite*>								m_destructable_box;

	double										xOffset;
	double										yOffset;

	CResizableImage								m_BgImage;
	BackBuffer*									m_BgBuffer;

	void										DrawBackground();
	void										DrawEnviroment();

public:
	//-------------------------------------------------------------------------
	// Keep these public because they need to be modified externally.
	//-------------------------------------------------------------------------
	vector<vector<Object*>>						m_MapMatrix;	// Matrice pentru incarcare harta

	vector<int>									NrOfWalls;	// Numarul de block-uri de fiecare tip
};

#endif // _CMAP_H_