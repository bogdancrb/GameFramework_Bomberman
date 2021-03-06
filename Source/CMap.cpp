//-----------------------------------------------------------------------------
// File: CMap.cpp
//
// Desc: Map loading and colisions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CMap Specific Includes
//-----------------------------------------------------------------------------
#include "CMap.h"

CMap::CMap(const char* FileName, BackBuffer * Buffer)
{
	NrOfWalls = OpenMap(FileName);	// Deschidere harta si returnare numar de block-uri

	m_BgBuffer = Buffer;

	// Pentru fiecare block setam un sprite

	for (int index = 0; index < NrOfWalls[0]; index++)
	{
		m_Wall.resize(index+1);
		m_Wall[index] = new Sprite("data/walls/stone.bmp", RGB(0xff, 0x00, 0xff));
		m_Wall[index]->setBackBuffer(m_BgBuffer);
	}

	for (int index = 0; index < NrOfWalls[1]; index++)
	{
		m_indestructable_box.resize(index+1);
		m_indestructable_box[index] = new Sprite("data/walls/stone.bmp", RGB(0xff, 0x00, 0xff));
		m_indestructable_box[index]->setBackBuffer(m_BgBuffer);
	}

	for (int index = 0; index < NrOfWalls[2]; index++)
	{
		m_destructable_box.resize(index+1);
		m_destructable_box[index] = new Sprite("data/walls/brick.bmp", RGB(0xff, 0x00, 0xff));
		m_destructable_box[index]->setBackBuffer(m_BgBuffer);
	}

	m_Portal = new Sprite("data/objects/portal.bmp", RGB(0xff, 0x00, 0xff));
	m_Portal->setBackBuffer(m_BgBuffer);

	m_Portal->isSpriteVisible = true;

	xOffset = 0;
	yOffset = 0;
}

CMap::~CMap(void)
{
	for(int i=0;i < (int)m_MapMatrix.size();i++)
		for(int j=0;j < (int)m_MapMatrix[i].size();j++)
		{
			delete m_MapMatrix[i][j];
			m_MapMatrix[i][j] = NULL;
		}

	m_Wall.clear();
	m_indestructable_box.clear();
	m_destructable_box.clear();
}

vector<int> CMap::OpenMap(const char* FileName)
{
	ifstream file(FileName, ifstream::in);
	vector<int> NrOfObj;

	if(file.is_open())
	{
		int i = 0, j = 0;
		char c;

		NrOfObj.push_back(0); NrOfObj.push_back(0); NrOfObj.push_back(0); // Initializare vectori pentru 3 tipuri de block-uri

		while(!file.eof())
		{
			c = file.get();

			if(file.eof()) // Sa nu se execute Load Element odata in plus
				break;

			if(c != '\n')
			{
				LoadElement(c,i,j);
				j++;
			}
			else
			{
				i++;
				j = 0;
			}

			// Calculam numarul total de block-uri
			// NrOfObj[0] - block-uri margine mapa (m_Wall)
			// NrOfObj[1] - block-uri indestructibile (m_indestructable_box)
			// NrOfObj[2] - block-uri destructibile (m_destructable_box)

			if (c == '1')
				NrOfObj[0]++;
			else if (c == 'i')
				NrOfObj[1]++;
			else if (c == 'd')
				NrOfObj[2]++;
		}	

		file.close();

		return NrOfObj;
	}
	else
	{
		MessageBox( 0, _T("Error code: 2\n\nFailed to initialize properly. Reinstalling the application may solve this problem.\nIf the problem persists, please contact technical support."), _T("Fatal Error"), MB_OK | MB_ICONSTOP);
		exit(0);
	}
}

void CMap::Draw(double x, double y)
{
	xOffset = x;
	yOffset = y;

	//DrawBackground();
	DrawEnviroment();
}

void CMap::DrawBackground()
{
	/*CBoxFilter* filter = new CBoxFilter();
	m_BgImage.SetFilter(filter);
	m_BgImage.Resample(1366, 768);*/
	m_BgImage.Paint(m_BgBuffer->getDC(), xOffset, yOffset);
}

void CMap::DrawEnviroment()
{
	int nWalls = 0, nDestruct = 0, nIndestruct = 0;

	for (int i = 0; i < m_MapMatrix.size(); i++)
	{
		for (int j = 0; j < m_MapMatrix[i].size(); j++)
		{
			if (m_MapMatrix[i][j]->m_Code != DIRT)	
			{
				switch (m_MapMatrix[i][j]->m_Code)
				{
					// Memorare fiecare bock in vectori

					case WALL:
						m_Wall[nWalls]->mPosition.x = j*BLOCKSIZE + BLOCKSIZE / 2 + xOffset;
						m_Wall[nWalls]->mPosition.y = i*BLOCKSIZE + BLOCKSIZE / 2 + yOffset;
						m_Wall[nWalls]->draw();
						nWalls++;
						break;

					case D_BOX:
						if (m_destructable_box[nDestruct]->isSpriteVisible)
						{
							m_destructable_box[nDestruct]->mPosition.x = j*BLOCKSIZE + BLOCKSIZE / 2 + xOffset;
							m_destructable_box[nDestruct]->mPosition.y = i*BLOCKSIZE + BLOCKSIZE / 2 + yOffset;
							m_destructable_box[nDestruct]->draw();
							nDestruct++;
						}
						else
						{
							nDestruct++;
						}

						break;

					case I_BOX:
						m_indestructable_box[nIndestruct]->mPosition.x = j*BLOCKSIZE + BLOCKSIZE / 2 + xOffset;
						m_indestructable_box[nIndestruct]->mPosition.y = i*BLOCKSIZE + BLOCKSIZE / 2 + yOffset;
						m_indestructable_box[nIndestruct]->draw();
						nIndestruct++;
						break;
				}
			}
		}
	}
}

void CMap::LoadElement(char val, int x, int y)
{
	m_MapMatrix.resize(x+1);
	m_MapMatrix[x].resize(y+1);
	m_MapMatrix[x][y] = new Object(x, y, val);
}

Object* CMap::GetElement(Vec2 pos)
{
	
	if(m_MapMatrix.size() > pos.y / BLOCKSIZE  && m_MapMatrix[pos.y / BLOCKSIZE ].size() > pos.x/ BLOCKSIZE )
		return m_MapMatrix[pos.y / BLOCKSIZE][pos.x/ BLOCKSIZE];
	else
		return NULL;
}

void CMap::Change(char val, int i, int j)   //updateaza spriteul dupa coliziuni
{
	m_MapMatrix[i][j]->m_Code = val;
}

void CMap::GeneratePortal()
{
	if (NrOfWalls[2] != NULL)
	{
		int randDestructIndex = rand() % NrOfWalls[2];

		if (m_Portal->mPosition == Vec2(0,0))
			m_Portal->mPosition = m_destructable_box[randDestructIndex]->mPosition;

		m_Portal->draw();
	}
	else
	{
		m_Portal->mPosition = Vec2(300,300);
	}
}

//----------------------------------------------[Object class] -------------------------------------------------

Object::Object(int i, int j, char code)
{ 
	m_I = i;
	m_J = j;
	m_Code = code;
}

void Object::Change(char val)   //updateaza spriteul dupa coliziuni
{
	m_Code = val;
}