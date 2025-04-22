#include "stdafx.h"
#include "SpriteBox2D.h"

#pragma warning (disable:4244)

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSpriteBox2DBase: Base abstract class

CSpriteBox2DBase::CSpriteBox2DBase(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, Uint32 time)
	: CSprite(x, y, w, h, time), m_pWorld(pWorld), m_pBody(NULL), m_fScale(100) { }
CSpriteBox2DBase::CSpriteBox2DBase(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, char *pFileBitmap, Uint32 time)
	: CSprite(x, y, w, h, pFileBitmap, time), m_pWorld(pWorld), m_pBody(NULL), m_fScale(100) { }
CSpriteBox2DBase::CSpriteBox2DBase(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, char *pFileBitmap, CColor colorKey, Uint32 time)
	: CSprite(x, y, w, h, pFileBitmap, colorKey, time), m_pWorld(pWorld), m_pBody(NULL), m_fScale(100) { }
CSpriteBox2DBase::CSpriteBox2DBase(b2World *pWorld, double x, double y, char *pFileBitmap, Uint32 time)
	: CSprite(x, y, pFileBitmap, time), m_pWorld(pWorld), m_pBody(NULL), m_fScale(100) { }
CSpriteBox2DBase::CSpriteBox2DBase(b2World *pWorld, double x, double y, char *pFileBitmap, CColor colorKey, Uint32 time)
	: CSprite(x, y, pFileBitmap, colorKey, time), m_pWorld(pWorld), m_pBody(NULL), m_fScale(100) { }
CSpriteBox2DBase::CSpriteBox2DBase(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, CGraphics *pGraphics, Uint32 time)
	: CSprite(x, y, w, h, pGraphics, time), m_pWorld(pWorld), m_pBody(NULL), m_fScale(100) { }
CSpriteBox2DBase::CSpriteBox2DBase(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, CGraphics *pGraphics, CColor colorKey, Uint32 time)
	: CSprite(x, y, w, h, pGraphics, colorKey, time), m_pWorld(pWorld), m_pBody(NULL), m_fScale(100) { }
CSpriteBox2DBase::CSpriteBox2DBase(b2World *pWorld, double x, double y, CGraphics *pGraphics, Uint32 time)
	: CSprite(x, y, pGraphics, time), m_pWorld(pWorld), m_pBody(NULL), m_fScale(100) { }
CSpriteBox2DBase::CSpriteBox2DBase(b2World *pWorld, double x, double y, CGraphics *pGraphics, CColor colorKey, Uint32 time)
	: CSprite(x, y, pGraphics, colorKey, time), m_pWorld(pWorld), m_pBody(NULL), m_fScale(100) { }

CSpriteBox2DBase::~CSpriteBox2DBase(void)
{
	if (m_pBody)
		m_pWorld->DestroyBody(m_pBody);
}

void CSpriteBox2DBase::CreateStaticBody()
{
	// destroy any existing body
	if (m_pBody)
		m_pWorld->DestroyBody(m_pBody);

	// create body definition providing the sprite's position, converted into Box2D coordinate system
	// body type is static, by default
	b2BodyDef bodyDef;
	bodyDef.position.Set((GetX() - 400.0f) / GetScale(), GetY() / GetScale());
	
	// world, create the body
	m_pBody = m_pWorld->CreateBody(&bodyDef);
	
	// create the simplest possible, static fixture
	// using CreateShape virtual function to define the shape
	m_pBody->CreateFixture(CreateShape(), 0.0f);
}

void CSpriteBox2DBase::CreateDynamicBody(float density, float friction, float restitution)
{
	// destroy any existing body
	if (m_pBody)
		m_pWorld->DestroyBody(m_pBody);

	// create body definition providing the sprite's position, converted into Box2D coordinate system
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;		// body type is dynamic
	bodyDef.position.Set((GetX() - 400.f) / GetScale(), (GetY()) / GetScale());
	
	// world, create the body
	m_pBody = m_pWorld->CreateBody(&bodyDef);
	
	// craete a fixture
	// using CreateShape virtual function to define the shape
	// and parameters provided (density, friction, restitution) to determine the physical properties
	b2FixtureDef fixtureDef;
	fixtureDef.shape = CreateShape();
	fixtureDef.density = density;
	fixtureDef.friction = friction;
	fixtureDef.restitution = restitution;
	fixtureDef.userData.pointer = (uintptr_t)this;
	m_pBody->CreateFixture(&fixtureDef);

	// setup the initial velocity (if any)
	m_pBody->SetLinearVelocity(b2Vec2(GetVelocity().m_x / GetScale(), GetVelocity().m_y / GetScale()));
}

void CSpriteBox2DBase::DeleteBody()
{
	if (m_pBody)
		m_pWorld->DestroyBody(m_pBody);
	m_pBody = NULL;
}

void CSpriteBox2DBase::OnUpdate(Uint32 time, Uint32 deltaTime)
{
	if (m_pBody)
	{
		// get the position and angle calculated by Box2D body and transfer them to the sprite
		// we bypass entire OnUpdate route typically used in GFC
		b2Vec2 position = m_pBody->GetPosition();
		float angle = m_pBody->GetAngle();

		SetPosition(position.x * GetScale() + 400, position.y * GetScale());
		SetRotation(-RAD2DEG(angle));
	}
	else
		// normal update if no Box2D body created
		CSprite::OnUpdate(time, deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSpriteBox2D: Usable class for rectangular bitmap sprites

CSpriteBox2D::CSpriteBox2D(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, Uint32 time)
	: CSpriteBox2DBase(pWorld, x, y, w, h, time) { }
CSpriteBox2D::CSpriteBox2D(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, char *pFileBitmap, Uint32 time)
	: CSpriteBox2DBase(pWorld, x, y, w, h, pFileBitmap, time) { }
CSpriteBox2D::CSpriteBox2D(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, char *pFileBitmap, CColor colorKey, Uint32 time)
	: CSpriteBox2DBase(pWorld, x, y, w, h, pFileBitmap, colorKey, time) { }
CSpriteBox2D::CSpriteBox2D(b2World *pWorld, double x, double y, char *pFileBitmap, Uint32 time)
	: CSpriteBox2DBase(pWorld, x, y, pFileBitmap, time) { }
CSpriteBox2D::CSpriteBox2D(b2World *pWorld, double x, double y, char *pFileBitmap, CColor colorKey, Uint32 time)
	: CSpriteBox2DBase(pWorld, x, y, pFileBitmap, colorKey, time) { }
CSpriteBox2D::CSpriteBox2D(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, CGraphics *pGraphics, Uint32 time)
	: CSpriteBox2DBase(pWorld, x, y, w, h, pGraphics, time) { }
CSpriteBox2D::CSpriteBox2D(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, CGraphics *pGraphics, CColor colorKey, Uint32 time)
	: CSpriteBox2DBase(pWorld, x, y, w, h, pGraphics, colorKey, time) { }
CSpriteBox2D::CSpriteBox2D(b2World *pWorld, double x, double y, CGraphics *pGraphics, Uint32 time)
	: CSpriteBox2DBase(pWorld, x, y, pGraphics, time) { }
CSpriteBox2D::CSpriteBox2D(b2World *pWorld, double x, double y, CGraphics *pGraphics, CColor colorKey, Uint32 time)
	: CSpriteBox2DBase(pWorld, x, y, pGraphics, colorKey, time) { }

b2Shape *CSpriteBox2D::CreateShape()
{
	// create a rectangular shape
	m_shapePoly.SetAsBox(GetWidth() / GetScale() / 2, GetHeight() / GetScale() / 2);
	return &m_shapePoly;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSpriteBox2DRect: Box2D based alternative for CSpriteRect

CSpriteBox2DRect::CSpriteBox2DRect(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, CColor color, long time)
	: CSpriteBox2D(pWorld, x, y, w, h, time), m_color(color), m_colorOutline(color) { SetColorKey(CColor::AnyBut(color)); }
CSpriteBox2DRect::CSpriteBox2DRect(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, CColor color, CColor colorOutline, long time)	
	: CSpriteBox2D(pWorld, x, y, w, h, time), m_color(color), m_colorOutline(colorOutline) { SetColorKey(CColor::AnyBut(colorOutline)); }

void CSpriteBox2DRect::OnDraw(CGraphics *g)
{
	// draw a rectangle (copy of standard GFC CSpriteRect::OnDraw function)
	if (IsValid()) return;
	CRectangle rect;
	GetClientRect(rect);
	g->FillRect(rect, m_color);
	rect.Grow(0, 0, -1, -1);
	g->DrawRect(rect, m_colorOutline);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSpriteBox2DCircle: Box2D based alternative for CSpriteOval (circles only!)

CSpriteBox2DCircle::CSpriteBox2DCircle(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, CColor color, long time)
	: CSpriteBox2DBase(pWorld, x, y, w, h, time), m_color(color), m_colorOutline(color) { SetColorKey(CColor::AnyBut(color)); }
CSpriteBox2DCircle::CSpriteBox2DCircle(b2World *pWorld, double x, double y, Sint16 w, Sint16 h, CColor color, CColor colorOutline, long time)	
	: CSpriteBox2DBase(pWorld, x, y, w, h, time), m_color(color), m_colorOutline(colorOutline) { SetColorKey(CColor::AnyBut(colorOutline)); }

b2Shape *CSpriteBox2DCircle::CreateShape()
{
	// create a circular shape
	m_shapeCircle.m_radius = GetWidth() / GetScale() / 2;
	return &m_shapeCircle;
}

void CSpriteBox2DCircle::OnDraw(CGraphics *g)
{
	// draw a circle (copy of standard GFC CSpriteOval::OnDraw function)
	if (IsValid()) return;
	CRectangle rect;
	GetClientRect(rect);
	g->FillRect(rect, GetColorKey());
	rect.Grow(0, 0, -1, -1);
	g->FillOval(rect, m_color);
	g->DrawOval(rect, m_colorOutline);
}

