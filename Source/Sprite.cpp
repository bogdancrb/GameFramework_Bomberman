#include "Sprite.h"

extern HINSTANCE g_hInst;
Vec2 PlayerPos;
Vec2 PlayerVel;

Sprite::Sprite(int imageID, int maskID)
{
	// Load the bitmap resources.
	mhImage = LoadBitmap(g_hInst, MAKEINTRESOURCE(imageID));
	mhMask = LoadBitmap(g_hInst, MAKEINTRESOURCE(maskID));

	// Get the BITMAP structure for each of the bitmaps.
	GetObject(mhImage, sizeof(BITMAP), &mImageBM);
	GetObject(mhMask, sizeof(BITMAP), &mMaskBM);

	// Image and Mask should be the same dimensions.
	assert(mImageBM.bmWidth == mMaskBM.bmWidth);
	assert(mImageBM.bmHeight == mMaskBM.bmHeight);	

	mcTransparentColor = 0;
	mhSpriteDC = 0;

	fileName[0] = '\0';
}

Sprite::Sprite(const char *szImageFile, const char *szMaskFile)
{
	mhImage = (HBITMAP)LoadImage(g_hInst, szImageFile, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	mhMask = (HBITMAP)LoadImage(g_hInst, szMaskFile, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	// Get the BITMAP structure for each of the bitmaps.
	GetObject(mhImage, sizeof(BITMAP), &mImageBM);
	GetObject(mhMask, sizeof(BITMAP), &mMaskBM);

	// Image and Mask should be the same dimensions.
	assert(mImageBM.bmWidth == mMaskBM.bmWidth);
	assert(mImageBM.bmHeight == mMaskBM.bmHeight);

	mcTransparentColor = 0;
	mhSpriteDC = 0;

	strcpy(fileName,szImageFile);
}

Sprite::Sprite(const char *szImageFile, COLORREF crTransparentColor)
{
	mhImage = (HBITMAP)LoadImage(g_hInst, szImageFile, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	mhMask = 0;
	mhSpriteDC = 0;
	mcTransparentColor = crTransparentColor;

	// Get the BITMAP structure for the bitmap.
	GetObject(mhImage, sizeof(BITMAP), &mImageBM);

	strcpy(fileName,szImageFile);
}

Sprite::~Sprite()
{
	// Free the resources we created in the constructor.
	DeleteObject(mhImage);
	DeleteObject(mhMask);

	DeleteDC(mhSpriteDC);
}

void Sprite::update(float dt, int ID)
{
	// Update the sprites position.

	mPosition += mVelocity * dt; // Update bounding rectangle/circle

	if (ID == 1) // avion
	{
		PlayerPos = Vec2(mPosition.x,mPosition.y);
		PlayerVel = Vec2(mVelocity.x,mVelocity.y);

		if (mPosition.x <= 0) // limita stanga
		{
			mPosition.x = 0;
			mVelocity.x = 0;
		}
		if (mPosition.x >= GetSystemMetrics(SM_CXSCREEN)) // limita dreapta
		{
			mPosition.x = GetSystemMetrics(SM_CXSCREEN);
			mVelocity.x = 0;
		}
		if (mPosition.y <= GetSystemMetrics(SM_CYSCREEN)-750 /*sau cu 60?*/) // limita sus
		{
			mPosition.y = GetSystemMetrics(SM_CYSCREEN)-750; //sau cu 60?
			mVelocity.y = 0;
		}
		if (mPosition.y >= GetSystemMetrics(SM_CYSCREEN)-100) // limita jos
		{
			mPosition.y = GetSystemMetrics(SM_CYSCREEN)-100;
			mVelocity.y = 0;
		}
	}

	if (ID == 2) // bullet
	{
		if (mPosition.x <= 0 || mPosition.x >= GetSystemMetrics(SM_CXSCREEN)) // limita bullet stanga/dreapta
		{
			mPosition.x = PlayerPos.x;
			mVelocity.x = PlayerVel.x;
		}
		if (mPosition.y <= 0 || mPosition.y >= GetSystemMetrics(SM_CYSCREEN)) // limita bullet sus/jos
		{
			mPosition.y = PlayerPos.y;
			mVelocity.y = PlayerVel.y;
		}
		
	}

	if (ID == 3) // crate
	{
		if (mPosition.y >= GetSystemMetrics(SM_CYSCREEN)) // limita jos
		{
			mPosition.y = -(rand() % 400 + 100);
			mPosition.x = rand() % GetSystemMetrics(SM_CXSCREEN) + 1;
			mVelocity.y = rand() % 50 + 30;

			int chose = rand() % 3 + 1;

			if (chose == 2)
				isSpriteVisible = true;
			else
				isSpriteVisible = false;
		}
	}
}

void Sprite::setBackBuffer(const BackBuffer *pBackBuffer)
{
	mpBackBuffer = pBackBuffer;
	if(mpBackBuffer)
	{
		DeleteDC(mhSpriteDC);
		mhSpriteDC = CreateCompatibleDC(mpBackBuffer->getDC());
	}
}

void Sprite::draw()
{
	if( mhMask != 0 )
		drawMask();
	else
		drawTransparent();
}

void Sprite::drawMask()
{
	if( mpBackBuffer == NULL )
		return;

	HDC hBackBufferDC = mpBackBuffer->getDC();

	// The position BitBlt wants is not the sprite's center
	// position; rather, it wants the upper-left position,
	// so compute that.
	int w = width();
	int h = height();

	// Upper-left corner.
	int x = (int)mPosition.x - (w / 2);
	int y = (int)mPosition.y - (h / 2);

	// Note: For this masking technique to work, it is assumed
	// the backbuffer bitmap has been cleared to some
	// non-zero value.
	// Select the mask bitmap.
	HGDIOBJ oldObj = SelectObject(mhSpriteDC, mhMask);

	// Draw the mask to the backbuffer with SRCAND. This
	// only draws the black pixels in the mask to the backbuffer,
	// thereby marking the pixels we want to draw the sprite
	// image onto.
	BitBlt(hBackBufferDC, x, y, w, h, mhSpriteDC, 0, 0, SRCAND);

	// Now select the image bitmap.
	SelectObject(mhSpriteDC, mhImage);

	// Draw the image to the backbuffer with SRCPAINT. This
	// will only draw the image onto the pixels that where previously
	// marked black by the mask.
	BitBlt(hBackBufferDC, x, y, w, h, mhSpriteDC, 0, 0, SRCPAINT);

	// Restore the original bitmap object.
	SelectObject(mhSpriteDC, oldObj);
}

void Sprite::drawTransparent()
{
	if( mpBackBuffer == NULL )
		return;

	HDC hBackBuffer = mpBackBuffer->getDC();

	int w = width();
	int h = height();

	// Upper-left corner.
	int x = (int)mPosition.x - (w / 2);
	int y = (int)mPosition.y - (h / 2);

	COLORREF crOldBack = SetBkColor(hBackBuffer, RGB(255, 255, 255));
	COLORREF crOldText = SetTextColor(hBackBuffer, RGB(0, 0, 0));
	HDC dcImage, dcTrans;

	// Create two memory dcs for the image and the mask
	dcImage=CreateCompatibleDC(hBackBuffer);
	dcTrans=CreateCompatibleDC(hBackBuffer);

	// Select the image into the appropriate dc
	SelectObject(dcImage, mhImage);

	// Create the mask bitmap
	BITMAP bitmap;
	GetObject(mhImage, sizeof(BITMAP), &bitmap);
	HBITMAP bitmapTrans = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, NULL);

	// Select the mask bitmap into the appropriate dc
	SelectObject(dcTrans, bitmapTrans);

	// Build mask based on transparent color
	SetBkColor(dcImage, mcTransparentColor);
	BitBlt(dcTrans, 0, 0, bitmap.bmWidth, bitmap.bmHeight, dcImage, 0, 0, SRCCOPY);

	// Do the work - True Mask method - cool if not actual display
	BitBlt(hBackBuffer, x, y, bitmap.bmWidth, bitmap.bmHeight, dcImage, 0, 0, SRCINVERT);
	BitBlt(hBackBuffer, x, y, bitmap.bmWidth, bitmap.bmHeight, dcTrans, 0, 0, SRCAND);
	BitBlt(hBackBuffer, x, y, bitmap.bmWidth, bitmap.bmHeight, dcImage, 0, 0, SRCINVERT);

	// free memory	
	DeleteDC(dcImage);
	DeleteDC(dcTrans);
	DeleteObject(bitmapTrans);

	// Restore settings
	SetBkColor(hBackBuffer, crOldBack);
	SetTextColor(hBackBuffer, crOldText);
}

void Sprite::LoadSprite(const char *szImageFile, COLORREF crTransparentColor)
{
	mhImage = (HBITMAP)LoadImage(g_hInst, szImageFile, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	mhMask = 0;
	mhSpriteDC = 0;
	mcTransparentColor = crTransparentColor;

	// Get the BITMAP structure for the bitmap.
	GetObject(mhImage, sizeof(BITMAP), &mImageBM);

	strcpy(fileName,szImageFile);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AnimatedSprite::AnimatedSprite(const char *szImageFile, const char *szMaskFile, const RECT& rcFirstFrame, int iFrameCount) 
			: Sprite (szImageFile, szMaskFile)
{
	mptFrameCrop.x = rcFirstFrame.left;
	mptFrameCrop.y = rcFirstFrame.top;
	mptFrameStartCrop = mptFrameCrop;
	miFrameWidth = rcFirstFrame.right - rcFirstFrame.left;
	miFrameHeight = rcFirstFrame.bottom - rcFirstFrame.top;
	miFrameCount = iFrameCount;
}

AnimatedSprite::AnimatedSprite(const char *szImageFile, COLORREF crTransparentColor, const RECT& rcFirstFrame, int iFrameCount) 
			: Sprite (szImageFile, crTransparentColor)
{
	mptFrameCrop.x = rcFirstFrame.left;
	mptFrameCrop.y = rcFirstFrame.top;
	mptFrameStartCrop = mptFrameCrop;
	miFrameWidth = rcFirstFrame.right - rcFirstFrame.left;
	miFrameHeight = rcFirstFrame.bottom - rcFirstFrame.top;
	miFrameCount = iFrameCount;
}

void AnimatedSprite::SetFrame(int iIndex)
{
	// index must be in range
	assert(iIndex >= 0 && iIndex < miFrameCount && "AnimatedSprite frame Index must be in range!");

	mptFrameCrop.x = mptFrameStartCrop.x + iIndex%5*miFrameWidth;
	mptFrameCrop.y = mptFrameStartCrop.y + iIndex/5*miFrameHeight;
}

void AnimatedSprite::draw()
{
	if( mpBackBuffer == NULL )
		return;

	// The position BitBlt wants is not the sprite's center
	// position; rather, it wants the upper-left position,
	// so compute that.
	int w = miFrameWidth;
	int h = miFrameHeight;

	HDC hBackBufferDC = mpBackBuffer->getDC();

	// Upper-left corner.
	int x = (int)mPosition.x - (w / 2);
	int y = (int)mPosition.y - (h / 2);

	// Note: For this masking technique to work, it is assumed
	// the backbuffer bitmap has been cleared to some
	// non-zero value.
	// Select the mask bitmap.
	HGDIOBJ oldObj = SelectObject(mhSpriteDC, mhMask);

	// Draw the mask to the backbuffer with SRCAND. This
	// only draws the black pixels in the mask to the backbuffer,
	// thereby marking the pixels we want to draw the sprite
	// image onto.
	BitBlt(hBackBufferDC, x, y, w, h, mhSpriteDC, mptFrameCrop.x, mptFrameCrop.y, SRCAND);

	// Now select the image bitmap.
	SelectObject(mhSpriteDC, mhImage);

	// Draw the image to the backbuffer with SRCPAINT. This
	// will only draw the image onto the pixels that where previously
	// marked black by the mask.
	BitBlt(hBackBufferDC, x, y, w, h, mhSpriteDC, mptFrameCrop.x, mptFrameCrop.y, SRCPAINT);

	// Restore the original bitmap object.
	SelectObject(mhSpriteDC, oldObj);
}