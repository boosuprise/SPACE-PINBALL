#pragma once

#include <Box2D/Box2D.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSpriteBox2DBase: Base abstract class

class CSpriteBox2DBase : public CSprite
{
	b2World *m_pWorld;
	b2Body *m_pBody;		// Box2D Body ref
	float m_fScale;			// scale (1 meter size in pixels); default = 100
public:
	CSpriteBox2DBase(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, Uint32 time);
	CSpriteBox2DBase(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, char *pFileBitmap, Uint32 time);
	CSpriteBox2DBase(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, char *pFileBitmap, CColor colorKey, Uint32 time);
	CSpriteBox2DBase(b2World *pWorld, double x, double y, char *pFileBitmap, Uint32 time);
	CSpriteBox2DBase(b2World *pWorld, double x, double y, char *pFileBitmap, CColor colorKey, Uint32 time);
	CSpriteBox2DBase(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, CGraphics *pGraphics, Uint32 time);
	CSpriteBox2DBase(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, CGraphics *pGraphics, CColor colorKey, Uint32 time);
	CSpriteBox2DBase(b2World *pWorld, double x, double y, CGraphics *pGraphics, Uint32 time);
	CSpriteBox2DBase(b2World *pWorld, double x, double y, CGraphics *pGraphics, CColor colorKey, Uint32 time);
	virtual ~CSpriteBox2DBase(void);

	void SetScale(float fScale)				{ m_fScale = fScale; }
	float GetScale()						{ return m_fScale; }

	b2World *GetWorld()						{ return m_pWorld; }

	void CreateStaticBody();
	void CreateDynamicBody(float density = 10.0f, float friction = 0.3, float restitution = 0);
	b2Body *GetBody()						{ return m_pBody; }
	void DeleteBody();

protected:
	virtual void OnUpdate(Uint32 time, Uint32 deltaTime);
	virtual b2Shape *CreateShape() = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSpriteBox2D: Usable class for rectangular bitmap sprites

class CSpriteBox2D : public CSpriteBox2DBase
{
	b2PolygonShape m_shapePoly;
public:
	CSpriteBox2D(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, Uint32 time);
	CSpriteBox2D(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, char *pFileBitmap, Uint32 time);
	CSpriteBox2D(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, char *pFileBitmap, CColor colorKey, Uint32 time);
	CSpriteBox2D(b2World *pWorld, double x, double y, char *pFileBitmap, Uint32 time);
	CSpriteBox2D(b2World *pWorld, double x, double y, char *pFileBitmap, CColor colorKey, Uint32 time);
	CSpriteBox2D(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, CGraphics *pGraphics, Uint32 time);
	CSpriteBox2D(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, CGraphics *pGraphics, CColor colorKey, Uint32 time);
	CSpriteBox2D(b2World *pWorld, double x, double y, CGraphics *pGraphics, Uint32 time);
	CSpriteBox2D(b2World *pWorld, double x, double y, CGraphics *pGraphics, CColor colorKey, Uint32 time);

protected:
	virtual b2Shape *CreateShape();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSpriteBox2DRect: Box2D based alternative for CSpriteRect

class CSpriteBox2DRect : public CSpriteBox2D
{
	CColor m_color;
	CColor m_colorOutline;
public:
	CSpriteBox2DRect(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, CColor color, long time);
	CSpriteBox2DRect(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, CColor color, CColor colorOutline, long time);

protected:
	virtual void OnDraw(CGraphics *g);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSpriteBox2DCircle: Box2D based alternative for CSpriteOval (circles only!)

class CSpriteBox2DCircle : public CSpriteBox2DBase
{
	CColor m_color;
	CColor m_colorOutline;
	b2CircleShape m_shapeCircle;
public:
	CSpriteBox2DCircle(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, CColor color, long time);
	CSpriteBox2DCircle(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, CColor color, CColor colorOutline, long time);

protected:
	virtual b2Shape *CreateShape();
	virtual void OnDraw(CGraphics *g);
};
