#include "stdafx.h"
#include "GooeyGame.h"
#include <algorithm>
#include <format>

CGooeyGame::CGooeyGame(void) :
	theBackground("back.png"),
	theMenuBack(300, 450, 32, 0xff, 0xff00, 0xff0000, 0xff000000),
	theMenuScreen("menu.png"),
	theCongratsScreen("congrats.png"),
	theMarble(20, 20, "Player.png", 0),
	//theCannon(580, 56, "cannon.png", 0),
	//theLPaddle(215, 60, "FlipperL.png", 0),
	//theRPaddle(345, 60, "FlipperR.png", 0),
	theBarrel(620, 70, "barrel.png", 0),
	thePowerSlider(CRectangle(12, 2, 200, 20), CColor(255,255,255,0), CColor::Black(), 0),
	thePowerMarker(CRectangle(12, 2, 200, 20), CColor::Blue(), 0),
	launchtrigger(CRectangle(560, 845, 5, 50), CColor::Red(), 0)

{
	m_pButtonPressed = NULL;
	m_bAimTime = 0;
	theBarrel.SetPivotFromCenter(-40, 0);
	theLPaddle.SetPivotFromCenter(-28.5, 0);
	theRPaddle.SetPivotFromCenter(28.5, 0);
	theLPaddle.SetRotation(40);
	theRPaddle.SetRotation(-40);

	m_nCurLevel = 0;
	m_nMaxLevel = 0;
	m_nUnlockedLevel = 1;
	m_bLevelCompleted = true;
	gravstr = 5.0;
}

CGooeyGame::~CGooeyGame(void)
{
}

// Tuning
#define MAX_POWER	1000
#define MIN_POWER	200
#define GRAVITY		5.f
#define RESTITUTION	0.8f

/////////////////////////////////////////////////////
// Helper Functions

// Spawns a marble inside the cannon and plays the "ready" sound
void CGooeyGame::SpawnMarble()
{
	theMarble.UnDie();
	theMarble.UnDelete();
	theMarble.SetVelocity(0, 0);
	theMarble.SetOmega(0);
	theMarble.SetPosition(theBarrel.GetPosition());
	if (IsGameMode())
	{
		m_player.Play("ready.wav"); m_player.Volume(1.f);
	}
}

// Makes the marble die and creates a new splash in its place
void CGooeyGame::KillMarble()
{
	if (theMarble.IsDying()) return;
	theSplashes.push_back(new CSplash(theMarble.GetPosition(), CColor(80, 90, 110), 1.0, 80, 100, GetTime()));
	theMarble.Die(0);
	m_player.Play("marble.wav"); m_player.Volume(1.f);
	launched = false;
	theWalls.back()->SetPos(560, 990);
}

// Starts the aiming mode
void CGooeyGame::BeginAim()
{
	m_bAimTime = GetTime();
	m_player.Play("aim.wav", -1); m_player.Volume(1.f);
}

// Returns true if in aiming mode
bool CGooeyGame::IsAiming()
{
	return m_bAimTime != 0;
}

// Returns the shot power, which depends on the time elapsed since the aiming mode was started
float CGooeyGame::GetShotPower()
{
	if (m_bAimTime == 0) return 0;
	float t = (float)(GetTime() - m_bAimTime);
	float ft = acos(1 - 2 * ((float)MIN_POWER / (float)MAX_POWER));
	float sp = (-0.5f * cos(t  * 3.1415f / 2000 + ft) + 0.5f) * MAX_POWER;
	if (sp > MIN_POWER) return sp; else return 0;
}

// Finishes the aiming mode
float CGooeyGame::Shoot()
{
	float f = GetShotPower();
	m_bAimTime = 0;
	if (f > 0) m_player.Play("cannon.wav"); else m_player.Stop(); m_player.Volume(0.4f);
	return f;
}

void CGooeyGame::PaddleControl()
{
	for (CSprite* paddles : theFlippers)
	{
		if (IsKeyDown(SDLK_a) && theFlippers.back()->GetRotation() > -40)
		{
			theFlippers.back()->Rotate(-4);



		}
		if (IsKeyDown(SDLK_d) && theFlippers.front()->GetRotation() < 40)
		{
			theFlippers.front()->Rotate(4);

		}
		
		if (!IsKeyDown(SDLK_a) && theFlippers.back()->GetRotation() < 40)
		{
				theFlippers.back()->Rotate(4);
		}
		if (!IsKeyDown(SDLK_d) && theFlippers.front()->GetRotation() > -40)
		{
				theFlippers.front()->Rotate(-4);

		}
		
	}
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CGooeyGame::OnUpdate()
{
	if (!IsGameMode()) return;

	Uint32 t = GetTime();
	Uint32 dt = GetDeltaTime();	// time since the last frame (in milliseconds)
	timeFrame = float(dt) / 1000.f;
	float R = (theMarble.GetWidth() / 2);

	launchtrigger.Update(t);
	PaddleControl();
	
	if (!theMarble.IsDead() && theMarble.GetSpeed() > 0)
	{
		// Apply accelerations
		if (theMarble.IsDying())
			// rapid linear deceleration if trapped inside a Goo
			theMarble.SetSpeed(theMarble.GetSpeed() * 0.9f);
		else
			// gravity!
			theMarble.Accelerate(0, -GRAVITY);

		//// TO DO: Test collisions with the walls
		// Hint: When collision detected, apply reflection. Note that you have the RESTITUTION defined as 0.8 (see line 36)
		// Also, play sound:  m_player.Play("hit.wav");


		//planets and blackholes
		if (launched == true)
		{
			for each(CSprite * planet in planets)
			{
				CVector planetpos = planet->GetPos();
				CVector n = theMarble.GetPos() - planetpos;
				CVector grv = n;

				if (n.Length() < 100)
				{
					n.Normalise();
					theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), n) * 1.5);
				}
			}



		}
		
		if (theMarble.HitTest(&launchtrigger))
		{
			launched = true;
			theWalls.back()->SetPos(560, 840);
		}

		//bumpers
		for each(CSprite * bumper in bumpers)
		{
			CVector bumperpos = bumper->GetPos();
			CVector n = theMarble.GetPos() - bumperpos;

			if (n.Length() < 40)
			{
				n.Normalise();
				theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), n) *1.1);
			}
		}

		// Black holes
		for each(CSprite * hole in blackHoles)
		{
			CVector holepos = hole->GetPos();
			CVector n = theMarble.GetPos() - holepos;

			if (n.Length() < 35)
			{
				KillMarble();
			}
		}
		// paddles
		for each(CSprite * paddles in theFlippers) {
			X = (paddles->GetWidth() / 2);
			Y = (paddles->GetHeight() / 2);
			a = paddles->GetRotation();
			alpha = DEG2RAD(a);
			CVector v = theMarble.GetVelocity() * timeFrame;
			CVector t = paddles->GetPos() - theMarble.GetPos();
			if (paddles->GetRotation() != 0 && paddles->GetRotation() != 180) {
				for (int i = 0; i <= 180; i += 180) {
					alpha = DEG2RAD(a + i);
					CVector n = CVector(sin(alpha), cos(alpha));
					if (Dot(v, n) < 0) {
						float vy = Dot(v, n);
						CVector d = t + (Y + R) * n;
						float dy = Dot(d, n);
						float f1 = dy / vy;

						float vx = Cross(v, n);
						float tx = Cross(t, n);
						float f2 = (tx - vx * f1) / (X + R);
						if (f1 >= 0 && f1 <= 1 && f2 > -1 && f2 <= 1) {
							theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), n));
							theMarble.SetYVelocity(theMarble.GetYVelocity() * 0.5);
							m_player.Play("hit.wav");
						}
					}
				}
			}
			else
			{


				if (v.m_y < 0) {
					f1 = (t.m_y + Y + R) / v.m_y;
					f2 = (t.m_x - v.m_x * f1) / (X + R);
					if (f1 >= 0 && f1 <= 1 && f2 > -1 && f2 <= 1) {
						theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), CVector(0, 1)));
						theMarble.SetYVelocity(theMarble.GetYVelocity() * 0.5);
						m_player.Play("hit.wav");
					}
				}

				if (v.m_y > 0) {
					f1 = (t.m_y - Y - R) / v.m_y;
					f2 = (t.m_x + v.m_x * f1) / (X + R);
					if (f1 >= 0 && f1 <= 1 && f2 > -1 && f2 <= 1) {
						theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), CVector(0, 1)));
						theMarble.SetYVelocity(theMarble.GetYVelocity() * 0.5);
						m_player.Play("hit.wav");
					}
				}

				if (v.m_x < 0) {
					f1 = (t.m_x + X + R) / v.m_x;
					f2 = (t.m_y - v.m_y * f1) / (Y + R);
					if (f1 >= 0 && f1 <= 1 && f2 > -1 && f2 <= 1) {
						theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), CVector(1, 0)));
						theMarble.SetXVelocity(theMarble.GetXVelocity() * 0.5);
						m_player.Play("hit.wav");
					}
				}

				if (v.m_x > 0) {
					f1 = (t.m_x - X - R) / v.m_x;
					f2 = (t.m_y + v.m_y * f1) / (Y + R);
					if (f1 >= 0 && f1 <= 1 && f2 > -1 && f2 <= 1) {
						theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), CVector(1, 0)));
						theMarble.SetXVelocity(theMarble.GetXVelocity() * 0.5);
						m_player.Play("hit.wav");
					}

				}

			}
		}

		//wall collision
		for each(CSprite * pWall in theWalls) {
			X = (pWall->GetWidth() / 2);
			Y = (pWall->GetHeight() / 2);
			a = pWall->GetRotation();
			alpha = DEG2RAD(a);
			CVector v = theMarble.GetVelocity() * timeFrame;
			CVector t = pWall->GetPos() - theMarble.GetPos();
			if (pWall->GetRotation() != 0 && pWall->GetRotation() != 180) {
				for (int i = 0; i <= 180; i += 180) {
					alpha = DEG2RAD(a + i);
					CVector n = CVector(sin(alpha), cos(alpha));
					if (Dot(v, n) < 0) {
						float vy = Dot(v, n);
						CVector d = t + (Y + R) * n;
						float dy = Dot(d, n);
						float f1 = dy / vy;

						float vx = Cross(v, n);
						float tx = Cross(t, n);
						float f2 = (tx - vx * f1) / (X + R);
						if (f1 >= 0 && f1 <= 1 && f2 > -1 && f2 <= 1) {
							theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), n)*0.8);
							if (!theMarble.IsDying() && theMarble.GetSpeed() > 30 )
							{
								m_player.Play("hit.wav");
							}
						}
					}
				}
			}
			else
			{


				if (v.m_y < 0) {
					f1 = (t.m_y + Y + R) / v.m_y;
					f2 = (t.m_x - v.m_x * f1) / (X + R);
					if (f1 >= 0 && f1 <= 1 && f2 > -1 && f2 <= 1) {
						theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), CVector(0, 1)));
						theMarble.SetYVelocity(theMarble.GetYVelocity() * 0.5);
						if (!theMarble.IsDying() && theMarble.GetSpeed() > 30)
						{
							m_player.Play("hit.wav");
						}
					}
				}

				if (v.m_y > 0) {
					f1 = (t.m_y - Y - R) / v.m_y;
					f2 = (t.m_x + v.m_x * f1) / (X + R);
					if (f1 >= 0 && f1 <= 1 && f2 > -1 && f2 <= 1) {
						theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), CVector(0, 1)));
						theMarble.SetYVelocity(theMarble.GetYVelocity() * 0.5);
						if (!theMarble.IsDying() && theMarble.GetSpeed() > 30)
						{
							m_player.Play("hit.wav");
						}
					}
				}

				if (v.m_x < 0) {
					f1 = (t.m_x + X + R) / v.m_x;
					f2 = (t.m_y - v.m_y * f1) / (Y + R);
					if (f1 >= 0 && f1 <= 1 && f2 > -1 && f2 <= 1) {
						theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), CVector(1, 0)));
						theMarble.SetXVelocity(theMarble.GetXVelocity() * 0.5);
						if (!theMarble.IsDying() && theMarble.GetSpeed() > 30)
						{
							m_player.Play("hit.wav");
						}
					}
				}

				if (v.m_x > 0) {
					f1 = (t.m_x - X - R) / v.m_x;
					f2 = (t.m_y + v.m_y * f1) / (Y + R);
					if (f1 >= 0 && f1 <= 1 && f2 > -1 && f2 <= 1) {
						theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), CVector(1, 0)));
						theMarble.SetXVelocity(theMarble.GetXVelocity() * 0.5);
						if (!theMarble.IsDying() && theMarble.GetSpeed() > 30)
						{
							m_player.Play("hit.wav");
						}
					}

				}

			}
		}
	}



	// Marble Update Call
	theMarble.Update(t);

	for (CSprite* paddles : theFlippers)
	{
		paddles->Update(t);
	}
	for (CSprite* pCollectibles : collectibles)
		pCollectibles->Update(t);
	for (CSprite* pBumpers : bumpers)
		pBumpers->Update(t);
	for (CSprite* pBlackHoles : blackHoles)
		pBlackHoles->Update(t);
	for (CSprite* pPlanet : planets)
		pPlanet->Update(t);
	for (CSprite* pBluePortal : bluePortal)
		pBluePortal->Update(t);
	for (CSprite* pOrangePortal : orangePortal)
		pOrangePortal->Update(t);


	// Kill very slow moving marbles
	if (!theMarble.IsDying() && theMarble.GetSpeed() > 0 && theMarble.GetSpeed() < 0.1)
		KillMarble();	// kill very slow moving marble

	// Kill the marble if lost of sight
	if (theMarble.GetRight() < 0 || theMarble.GetLeft() > GetWidth() || theMarble.GetTop() < 0)
		KillMarble();

	// Test for hitting Goos
	CSprite *pGooHit = NULL;
	for (CSprite *pGoo : theGoos)
		if (theMarble.HitTest(pGoo))
			pGooHit = pGoo;

	// point get
	for (CSprite* point : collectibles)
	{
		if (theMarble.HitTest(point))
		{
			point->Delete();
			pts++;
		}
	}
	collectibles.delete_if(deleted);


	// Update the Splashes (if needed)
	for (CSplash *pSplash : theSplashes)
		pSplash->Update(t);
	theSplashes.remove_if(deleted<CSplash*>);

	// Respawn the Marble - when all splashes are gone
	if (theMarble.IsDead() && theSplashes.size() == 0)
		SpawnMarble();

	// Success Test - if no more nuclear fuel rods and no more splashes, then the level is complete!
	if (collectibles.size() == 0 && theSplashes.size() == 0)
	{
		m_bLevelCompleted = true;
		NewGame();
	}
}

void CGooeyGame::OnDraw(CGraphics* g)
{
	// Draw Sprites
	g->Blit(CVector(0, 0), theBackground);
	for (CSprite *pWall : theWalls)
		pWall->Draw(g);
	for (CSprite* paddles : theFlippers)
	{
		paddles->Draw(g);
	}
	for (CSprite* pCollectibles : collectibles)
		pCollectibles->Draw(g);
	for (CSprite* pBumpers : bumpers)
		pBumpers->Draw(g);
	for (CSprite *pGoo : theGoos)
		pGoo->Draw(g);
	for (CSprite* pBlackHole : blackHoles)
		pBlackHole->Draw(g);
	for (CSprite* pPlanet : planets)
		pPlanet->Draw(g);
	for (CSprite* pBluePortal : bluePortal)
		pBluePortal->Draw(g);
	for (CSprite* pOrangePortal : orangePortal)
		pOrangePortal->Draw(g);
	if (IsGameMode())
	{
		theButtons[1]->Enable(!theMarble.IsDying() && theMarble.GetSpeed() != 0);
		for (CSpriteButton* pButton : theButtons)
			if (pButton->IsVisible())
				pButton->Draw(g);
	}
	for (CSplash *pSplash : theSplashes)
		pSplash->Draw(g);

	theMarble.Draw(g);
	theBarrel.Draw(g);
	theCannon.Draw(g);
	//theLPaddle.Draw(g);
	//theRPaddle.Draw(g);

	launchtrigger.Draw(g);

	// Draw Power Meter
	float x = (GetShotPower() - MIN_POWER) * thePowerSlider.GetWidth() / (MAX_POWER - MIN_POWER);
	if (x < 0) x = 0;
	if (theMarble.GetSpeed() == 0)
	{
		thePowerMarker.SetSize(x, thePowerSlider.GetHeight());
		thePowerMarker.SetPosition(thePowerSlider.GetPosition() + CVector((x - thePowerSlider.GetWidth()) / 2, 0));
		thePowerMarker.Invalidate();
		thePowerMarker.Draw(g);
		thePowerSlider.Draw(g);
	}

	if (IsGameMode())
		*g << bottom << right << "LEVEL " << theMarble.GetSpeed();
	
	// Draw Menu Items
	if (IsMenuMode())
	{
		g->Blit(CVector(0, 0), theMenuBack);
		g->Blit(CVector((float)GetWidth() - theMenuScreen.GetWidth(), (float)GetHeight() - theMenuScreen.GetHeight()) / 2, theMenuScreen);
		for (CSpriteButton *pButton : theButtonsLevel)
			if (pButton->IsVisible())
				pButton->Draw(g);
		if (m_pCancelButton->IsVisible())
			m_pCancelButton->Draw(g);
	}

	// Draw the Congratulations screen
	if (IsGameOver())
	{
		g->Blit(CVector(0, 0), theMenuBack);
		g->Blit(CVector((float)GetWidth() - theCongratsScreen.GetWidth(), (float)GetHeight() - theCongratsScreen.GetHeight()) / 2, theCongratsScreen);
		m_pCancelButton->Draw(g);
	}
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CGooeyGame::OnInitialize()
{
	m_nMaxLevel = 5;
		
	// Prepare menu background: dark grey, semi-transparent
	Uint32 col = SDL_MapRGBA(theMenuBack.GetSurface()->format, 64, 64, 64, 192);
	SDL_FillRect(theMenuBack.GetSurface(), NULL, col);

	// setup buttons
	theButtons.push_back(new CSpriteButton(50, 695, 80, 30, CMD_MENU, CColor::Black(), CColor(192, 192, 192), "menu", "arial.ttf", 16, GetTime()));
	theButtons.push_back(new CSpriteButton(140, 695, 80, 30, CMD_EXPLODE, CColor::Black(), CColor::LightGray(), "explode", "arial.ttf", 16, GetTime()));

	// setup level buttons
	float x = (GetWidth() - (m_nMaxLevel - 1) * 50) / 2;
	for (int i = 1; i <= m_nMaxLevel; i++)
	{
		CSpriteButton *p = new CSpriteButton(x, 320, 40, 40, CMD_LEVEL + i, CColor::Black(), CColor(192, 192, 192), to_string(i), "arial.ttf", 14, GetTime());
		p->SetSelectedBackColor(CColor(99, 234, 1));
		p->SetSelectedTextColor(CColor(1, 128, 1));
		p->SetSelectedFrameColor(CColor(1, 128, 1));
		theButtonsLevel.push_back(p);
		x += 50;
	}

	// setup cancel button
	m_pCancelButton = new CSpriteButton(1072, 569, 16, 16, CMD_CANCEL, CColor::Black(), CColor(192, 192, 192), "", "arial.ttf", 14, GetTime());
	m_pCancelButton->LoadImages("xcross.png", "xsel.png", "xsel.png");
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CGooeyGame::OnDisplayMenu()
{
	// Spawn the marble and prepare the level buttons for display
	SpawnMarble();

	// Progress the level
	if (m_bLevelCompleted)
	{
		m_nCurLevel++;
		if (m_nCurLevel > m_nMaxLevel)
		{
			GameOver();	// Game Over means the game won!
			m_player.Play("success2.wav"); m_player.Volume(1.f);
			m_pCancelButton->Show(true);
			return;
		}
		else
		{
			m_nUnlockedLevel = max(m_nUnlockedLevel, m_nCurLevel);
			m_player.Play("success1.wav"); m_player.Volume(1.f);
		}
	}

	// Update button states
	for (int nLevel = 1; nLevel <= m_nMaxLevel; nLevel++)
	{
		CSpriteButton* pButton = theButtonsLevel[nLevel-1];
		pButton->Select(nLevel == max(m_nCurLevel, 1));
		pButton->Enable(nLevel <= m_nUnlockedLevel);
		pButton->LoadImages("", "", "", "padlock.png");
	}
	m_pCancelButton->Show(!m_bLevelCompleted);
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CGooeyGame::OnStartGame()
{
}

void CGooeyGame::OnStartLevel(Sint16 nLevel)
{
	if (nLevel == 0) return;	// menu mode...

	// if level not completed, it should be continued (this is when cancel pressed in level selection menu)
	if (!m_bLevelCompleted)
		return;

	// spawn the marble
	SpawnMarble();

	// destroy the old playfield
	for (CSprite *pWall : theWalls) delete pWall;
	theWalls.clear();
	for (CSprite* paddles : theFlippers) delete paddles;
	theFlippers.clear();
	for (CSprite *pGoo : theGoos) delete pGoo;
	theGoos.clear();
	for (CSplash *pSplash : theSplashes) delete pSplash;
	theSplashes.clear();
	for (CSprite* pCollectibles : collectibles) delete pCollectibles;
	collectibles.clear();
	for (CSprite* pBumper : bumpers) delete pBumper;
	bumpers.clear();
	for (CSprite* pBlackHole : blackHoles) delete pBlackHole;
	blackHoles.clear();
	for (CSprite* pPlanet : planets) delete pPlanet;
	planets.clear();
	for (CSprite* pBluePortal : bluePortal) delete pBluePortal;
	bluePortal.clear();
	for (CSprite* pOrangePortal : orangePortal) delete pOrangePortal;
	orangePortal.clear();

	// create the new playfield, depending on the current level
	switch (m_nCurLevel)
	{
	// Level 1
	case 1:
		theWalls.push_back(new CSprite(CRectangle(0, 900, 600, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(550, 0, 20, 840), "wallvert.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(600, 0, 20, 840), "wallvert.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(0, 0, 20, 900), "wallvert.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(560, 890, 60, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(45);
		theWalls.push_back(new CSprite(CRectangle(0, 90, 200, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(20);
		theWalls.push_back(new CSprite(CRectangle(370, 90, 200, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(-20);
		theWalls.push_back(new CSprite(CRectangle(0, 570, 100, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(20);
		theWalls.push_back(new CSprite(CRectangle(0, 550, 100, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theFlippers.push_back(new CSprite(345, 60, "FlipperR.png", GetTime()));
		theFlippers.back()->SetPivotFromCenter(28.5);
		theFlippers.back()->Rotate(-40);
		theFlippers.push_back(new CSprite(215, 60, "FlipperL.png", GetTime()));
		theFlippers.back()->SetPivotFromCenter(-28.5);
		theFlippers.back()->Rotate(40);
		collectibles.push_back(new CSprite(CRectangle(70, 600, 18, 48),  "Fuel rod.png", GetTime()));
		collectibles.push_back(new CSprite(CRectangle(50, 250, 18, 48), "Fuel rod.png", GetTime())); 
		collectibles.push_back(new CSprite(CRectangle(490, 230, 18, 48), "Fuel rod.png", GetTime()));
		bumpers.push_back(new CSprite(CRectangle(150, 700, 48, 48), "bumper.png", GetTime()));
		bumpers.push_back(new CSprite(CRectangle(100, 300, 48, 48), "bumper.png", GetTime()));
		bumpers.push_back(new CSprite(CRectangle(450, 350, 48, 48), "bumper.png", GetTime()));
		break;

	//Level 2
	case 2:
		theWalls.push_back(new CSprite(CRectangle(0, 900, 600, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(550, 0, 20, 840), "wallvert.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(0, 0, 20, 900), "wallvert.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(560, 890, 60, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(45);
		theWalls.push_back(new CSprite(CRectangle(0, 90, 200, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(20);
		theWalls.push_back(new CSprite(CRectangle(370, 90, 200, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(-20);
		theFlippers.push_back(new CSprite(345, 60, "FlipperR.png", GetTime()));
		theFlippers.back()->SetPivotFromCenter(28.5);
		theFlippers.back()->Rotate(-40);
		theFlippers.push_back(new CSprite(215, 60, "FlipperL.png", GetTime()));
		theFlippers.back()->SetPivotFromCenter(-28.5);
		theFlippers.back()->Rotate(40);
		collectibles.push_back(new CSprite(CRectangle(500, 750, 18, 48), "Fuel rod.png", GetTime()));
		collectibles.push_back(new CSprite(CRectangle(50, 650, 18, 48), "Fuel rod.png", GetTime()));
		collectibles.push_back(new CSprite(CRectangle(500, 500, 18, 48), "Fuel rod.png", GetTime()));
		collectibles.push_back(new CSprite(CRectangle(50, 150, 18, 48), "Fuel rod.png", GetTime()));
		//Planet
		planets.push_back(new CSprite(CRectangle(20, 450, 100, 200), "PlanetL.png", GetTime()));
		planets.push_back(new CSprite(CRectangle(450, 300, 100, 200), "PlanetR.png", GetTime()));
		break;

	//Level 3
	case 3:
		//walls
		theWalls.push_back(new CSprite(CRectangle(0, 900, 600, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(550, 0, 20, 840), "wallvert.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(0, 0, 20, 900), "wallvert.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(560, 890, 60, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(45);
		theWalls.push_back(new CSprite(CRectangle(0, 90, 200, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(20);
		theWalls.push_back(new CSprite(CRectangle(0, 550, 100, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(30);
		theWalls.push_back(new CSprite(CRectangle(0, 400, 100, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(30);
		theWalls.push_back(new CSprite(CRectangle(460, 550, 100, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(-30);
		theWalls.push_back(new CSprite(CRectangle(350, 300, 70, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(-20);
		theWalls.push_back(new CSprite(CRectangle(410, 300, 70, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(20);
		theWalls.push_back(new CSprite(CRectangle(370, 90, 200, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(-20);
		//Flippers
		theFlippers.push_back(new CSprite(345, 60, "FlipperR.png", GetTime()));
		theFlippers.back()->SetPivotFromCenter(28.5);
		theFlippers.back()->Rotate(-40);
		theFlippers.push_back(new CSprite(215, 60, "FlipperL.png", GetTime()));
		theFlippers.back()->SetPivotFromCenter(-28.5);
		theFlippers.back()->Rotate(40);
		//collectibles
		collectibles.push_back(new CSprite(CRectangle(520, 600, 18, 48), "Fuel rod.png", GetTime()));
		collectibles.push_back(new CSprite(CRectangle(30, 450, 18, 48), "Fuel rod.png", GetTime()));
		collectibles.push_back(new CSprite(CRectangle(405, 250, 18, 48), "Fuel rod.png", GetTime()));
		//bumpers
		bumpers.push_back(new CSprite(CRectangle(30, 150, 48, 48), "bumper.png", GetTime()));
		bumpers.push_back(new CSprite(CRectangle(500, 750, 48, 48), "bumper.png", GetTime()));
		//Black hole
		blackHoles.push_back(new CSprite(CRectangle(30, 580, 24*3, 24*3), "Bhole.png", GetTime()));
		blackHoles.push_back(new CSprite(CRectangle(480, 300, 24 * 3, 24 * 3), "Bhole.png", GetTime()));
		break;

	//Level 4
	case 4:
		theWalls.push_back(new CSprite(CRectangle(0, 900, 600, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(550, 0, 20, 840), "wallvert.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(0, 0, 20, 900), "wallvert.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(560, 890, 60, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(45);
		theWalls.push_back(new CSprite(CRectangle(0, 90, 200, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(20);
		theWalls.push_back(new CSprite(CRectangle(370, 90, 200, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(-20);
		theWalls.push_back(new CSprite(CRectangle(0, 450, 100, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(0, 300, 100, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(30);
		theWalls.push_back(new CSprite(CRectangle(450, 300, 100, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		collectibles.push_back(new CSprite(CRectangle(30, 360, 18, 48), "Fuel rod.png", GetTime()));
		collectibles.push_back(new CSprite(CRectangle(520, 700, 18, 48), "Fuel rod.png", GetTime()));
		//Flippers
		theFlippers.push_back(new CSprite(345, 60, "FlipperR.png", GetTime()));
		theFlippers.back()->SetPivotFromCenter(28.5);
		theFlippers.back()->Rotate(-40);
		theFlippers.push_back(new CSprite(215, 60, "FlipperL.png", GetTime()));
		theFlippers.back()->SetPivotFromCenter(-28.5);
		theFlippers.back()->Rotate(40);
		//Planet
		planets.push_back(new CSprite(CRectangle(450, 500, 100, 200), "PlanetR.png", GetTime()));
		//Portal
		bluePortal.push_back(new CSprite(CRectangle(20, 470, 64, 64), "Portal_blue.png", GetTime()));
		orangePortal.push_back(new CSprite(CRectangle(480, 320, 64, 64), "Portal_red.png", GetTime()));
		break;
	case 5: //second part to level 4
		theWalls.push_back(new CSprite(CRectangle(0, 900, 600, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(550, 0, 20, 840), "wallvert.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(0, 0, 20, 900), "wallvert.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(560, 890, 60, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(45);
		theWalls.push_back(new CSprite(CRectangle(0, 90, 200, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(20);
		theWalls.push_back(new CSprite(CRectangle(370, 90, 200, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(-20);
		theWalls.push_back(new CSprite(CRectangle(0, 450, 100, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(450, 300, 100, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		collectibles.push_back(new CSprite(CRectangle(150, 250, 18, 48), "Fuel rod.png", GetTime()));
		collectibles.push_back(new CSprite(CRectangle(350, 200, 18, 48), "Fuel rod.png", GetTime()));
		collectibles.push_back(new CSprite(CRectangle(450, 330, 18, 48), "Fuel rod.png", GetTime()));
		collectibles.push_back(new CSprite(CRectangle(320, 400, 18, 48), "Fuel rod.png", GetTime()));
		collectibles.push_back(new CSprite(CRectangle(120, 480, 18, 48), "Fuel rod.png", GetTime()));
		collectibles.push_back(new CSprite(CRectangle(430, 500, 18, 48), "Fuel rod.png", GetTime()));
		collectibles.push_back(new CSprite(CRectangle(130, 700, 18, 48), "Fuel rod.png", GetTime()));
		collectibles.push_back(new CSprite(CRectangle(330, 730, 18, 48), "Fuel rod.png", GetTime()));
		blackHoles.push_back(new CSprite(CRectangle(30, 750, 24 * 3, 24 * 3), "Bhole.png", GetTime()));
		blackHoles.push_back(new CSprite(CRectangle(480, 750, 24 * 3, 24 * 3), "Bhole.png", GetTime()));
		//Flippers
		theFlippers.push_back(new CSprite(345, 60, "FlipperR.png", GetTime()));
		theFlippers.push_back(new CSprite(215, 60, "FlipperL.png", GetTime()));
		
		//portals
		bluePortal.push_back(new CSprite(CRectangle(20, 470, 64, 64), "Portal_blue.png", GetTime()));
		orangePortal.push_back(new CSprite(CRectangle(480, 320, 64, 64), "Portal_red.png", GetTime()));
		break;
	case 6:
		
		theWalls.push_back(new CSprite(CRectangle(0, 90, 500, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(20);
		theWalls.push_back(new CSprite(CRectangle(370, 90, 500, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(-20);
		collectibles.push_back(new CSprite(CRectangle(150, 250, 18, 48), "Fuel rod.png", GetTime()));
	}

	// kinda important dont delete LUCAS
	theWalls.push_back(new CSprite(CRectangle(550, 990, 20, 100), "wallvert.bmp", CColor::Blue(), GetTime()));
	// xoxo love u

	m_bLevelCompleted = false;
}

// called when the game is over
void CGooeyGame::OnGameOver()
{
}

// one time termination code
void CGooeyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CGooeyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
	
	
}

void CGooeyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	
}


/////////////////////////////////////////////////////
// On-Screen Buttons Handler

void CGooeyGame::OnButton(Uint16 nCmdId)
{
	if (IsGameOver())
	{
		// Cancel button clicked in the final screen
		if (nCmdId == CMD_CANCEL)
		{
			m_nCurLevel = 0;
			NewGame();
		}
	}
	else if (nCmdId >= CMD_LEVEL)
	{
		// New level selected from the menu
		m_bLevelCompleted = true;	// pretend previous level completed even if not
		m_nCurLevel = nCmdId - CMD_LEVEL;
		StartGame();
	}
	else
		switch (nCmdId)
		{
		case CMD_CANCEL: StartGame(); break; // cancel button in level selection - will go back to the game
		case CMD_MENU: NewGame(); break;	// proceed to menu without completing the level
		case CMD_EXPLODE: KillMarble(); break;
		}
}

/////////////////////////////////////////////////////
// Mouse Events Handlers

void CGooeyGame::OnLButtonDown(Uint16 x, Uint16 y)
{
	// Find out if any button is pressed and which one
	m_pButtonPressed = NULL;
	if (IsGameMode())
	{
		for (CSpriteButton* pButton : theButtons)
			if (pButton->IsVisible() && pButton->IsEnabled() && pButton->HitTest(x, y))
				m_pButtonPressed = pButton;
	}
	else
	{
		for (CSpriteButton* pButton : theButtonsLevel)
			if (pButton->IsVisible() && pButton->IsEnabled() && pButton->HitTest(x, y))
				m_pButtonPressed = pButton;
		if (m_pCancelButton->IsVisible() && m_pCancelButton->IsEnabled() && m_pCancelButton->HitTest(x, y))
			m_pButtonPressed = m_pCancelButton;
	}

	// In game mode, if the marble isn't moving yet, the mouse click will start aiming mode
	if (IsGameMode() && !m_pButtonPressed)
	{
		theBarrel.SetRotation(theBarrel.GetY() - y, x - theBarrel.GetX());
		if (theMarble.GetSpeed() == 0)
			BeginAim();
	}
}

void CGooeyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
	// Control hovering over on-screen buttons
	if (IsGameMode())
	{
		for (CSpriteButton* pButton : theButtons)
			pButton->Hover(pButton->IsVisible() && pButton->IsEnabled() && pButton->HitTest(x, y));
	}
	else
	{
		for (CSpriteButton* pButton : theButtonsLevel)
			pButton->Hover(pButton->IsVisible() && pButton->IsEnabled() && pButton->HitTest(x, y));
		m_pCancelButton->Hover(m_pCancelButton->IsVisible() && m_pCancelButton->IsEnabled() && m_pCancelButton->HitTest(x, y));
	}

	// In game mode, rotate the cannon's barrel
	if (IsGameMode() && !m_pButtonPressed)
		theBarrel.SetRotation(theBarrel.GetY() - y, x - theBarrel.GetX());
}

void CGooeyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
	// If on-screen button was pressed, handle this button (OnButton function above)
	if (m_pButtonPressed)
	{
		if (m_pButtonPressed->IsEnabled() && m_pButtonPressed->HitTest(x, y))
			OnButton(m_pButtonPressed->GetCmd());
		m_pButtonPressed = NULL;
	}
	// In game mode, rotate the cannon and, if additionally in aiming mode, shoot!
	else if (IsGameMode())
	{
		theBarrel.SetRotation(theBarrel.GetY() - y, x - theBarrel.GetX());

		if (IsAiming())
		{
			float P = Shoot();	// read the shooting power
			if (P > 0)
			{
				// create the nozzle-rotated vector and shoot the marble!
				CVector nozzle(95, 0);
				theBarrel.LtoG(nozzle, true);
				theMarble.SetPosition(nozzle);
				theMarble.Accelerate(P * Normalize(CVector(x, y) - theBarrel.GetPosition()));
			}
		}
	}
}

void CGooeyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CGooeyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CGooeyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CGooeyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
