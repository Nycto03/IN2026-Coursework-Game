#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include "PowerUpExtraLife.h"
#include "PowerUpInvulnerability.h"
#include "PowerUpBulletSizeBoost.h"
#include <iostream>



//namespace used to set up the power ups, the game would not compile otherwise
namespace {
	//Extra Life
	class PU_ExtraLife : public GameObject {
	public:
		PU_ExtraLife()
			: GameObject("PowerUpExtraLife")
		{

		}

			void Init() {
				//Initialises animation
				Animation* anim = AnimationManager::GetInstance()
					.GetAnimationByName("animated heart");
				auto sprite = std::make_shared<Sprite>(
					anim->GetWidth(), anim->GetHeight(), anim);


					sprite->SetLoopAnimation(true);
				SetSprite(sprite);

				//Initialises shape
				SetBoundingShape(std::make_shared<BoundingSphere>(GetThisPtr(), 5.0f));

				float speed = (rand() / (float)RAND_MAX) * 2.0f + 1.0f;
				float angle = (rand() / (float)RAND_MAX) * 2.0f * 3.14159f;
				SetVelocity(GLVector3f(cos(angle) * speed, sin(angle) * speed, 0));

			}
			
		
			// Only test collision against the ship
		bool CollisionTest(std::shared_ptr<GameObject> o) override {
			return o->GetType() == GameObjectType("Spaceship")
				&& mBoundingShape
				&& o->GetBoundingShape()
				&& mBoundingShape->CollisionTest(o->GetBoundingShape());
		}

		void OnCollision(const GameObjectList& objs) override {
			// remove self; let Spaceship::OnCollision handle the life increment
			mWorld->FlagForRemoval(GetThisPtr());
		}
	};

	//Invulnerability
	class PU_Invulnerability : public GameObject {
	public:
		PU_Invulnerability()
			: GameObject("PowerUpInvulnerability")
		{

		}

		void Init() {
			Animation* anim = AnimationManager::GetInstance()
				.GetAnimationByName("animated star");
			auto sprite = std::make_shared<Sprite>(
				anim->GetWidth(), anim->GetHeight(), anim);


				sprite->SetLoopAnimation(true);
			SetSprite(sprite);

			SetBoundingShape(std::make_shared<BoundingSphere>(GetThisPtr(), 5.0f));

			float speed = (rand() / (float)RAND_MAX) * 2.0f + 1.0f;
			float angle = (rand() / (float)RAND_MAX) * 2.0f * 3.14159f;
			SetVelocity(GLVector3f(cos(angle) * speed, sin(angle) * speed, 0));

		}

		bool CollisionTest(std::shared_ptr<GameObject> o) override {
			return o->GetType() == GameObjectType("Spaceship")
				&& mBoundingShape
				&& o->GetBoundingShape()
				&& mBoundingShape->CollisionTest(o->GetBoundingShape());
		}

		void OnCollision(const GameObjectList& objs) override {
			mWorld->FlagForRemoval(GetThisPtr());
		}
	};

	//Bullet Size Boost
	class PU_BulletBoost : public GameObject {
	public:
		PU_BulletBoost()
			: GameObject("PowerUpBulletSizeBoost")
		{

		}

		void Init() {
			Animation* anim = AnimationManager::GetInstance()
				.GetAnimationByName("animated bullet");
			auto sprite = std::make_shared<Sprite>(
				anim->GetWidth(), anim->GetHeight(), anim);


				sprite->SetLoopAnimation(true);
			SetSprite(sprite);

			SetBoundingShape(std::make_shared<BoundingSphere>(GetThisPtr(), 5.0f));

			float speed = (rand() / (float)RAND_MAX) * 2.0f + 1.0f;
			float angle = (rand() / (float)RAND_MAX) * 2.0f * 3.14159f;
			SetVelocity(GLVector3f(cos(angle) * speed, sin(angle) * speed, 0));

		}



		bool CollisionTest(std::shared_ptr<GameObject> o) override {
			return o->GetType() == GameObjectType("Spaceship")
				&& mBoundingShape
				&& o->GetBoundingShape()
				&& mBoundingShape->CollisionTest(o->GetBoundingShape());
		}

		void OnCollision(const GameObjectList& objs) override {
			mWorld->FlagForRemoval(GetThisPtr());
		}
	};
} // end anonymous namespace


// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char *argv[])
	: GameSession(argc, argv)
{
	mLevel = 0;
	mAsteroidCount = 0;
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	Animation *explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation *asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	Animation *spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");



	Animation *bulletAnim = AnimationManager::GetInstance().CreateAnimationFromFile(
		"animated bullet", 64, 16, 8, 16, "animated_bullet.png"
	);
	Animation *heartAnim = AnimationManager::GetInstance().CreateAnimationFromFile(
		"animated heart", 192, 32, 32, 32, "animated_heart.png" 
	);
	Animation * tarAnim = AnimationManager::GetInstance().CreateAnimationFromFile(
		"animated star", 256, 32, 32, 32, "animated_star.png"
	);

	// Create a spaceship and add it to the world
	mGameWorld->AddObject(CreateSpaceship());
	// Create some asteroids and add them to the world
	CreateAsteroids(10);

	//Create the GUI
	CreateGUI();

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);

	//Add a powerup listener
	mSpaceship->AddPowerUpListener(thisPtr);





	// schedule the first power-up spawn in 5–15 seconds:
	int delayMs = (rand() % 11 + 5) * 1000;   // 5–15s
	SetTimer(delayMs, SPAWN_POWERUP);




	// Start the game
	GameSession::Start();

}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	switch (key)
	{
	case ' ':
		mSpaceship->Shoot();
		break;
	default:
		break;
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is pressed start applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
	// If left arrow key is pressed start rotating anti-clockwise
	case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
	// If right arrow key is pressed start rotating clockwise
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
	// Default case - do nothing
	default: break;
	}
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is released stop applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
	// If left arrow key is released stop rotating
	case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
	// If right arrow key is released stop rotating
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
	// Default case - do nothing
	default: break;
	} 
}


// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	if (object->GetType() == GameObjectType("Asteroid"))
	{
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;
		if (mAsteroidCount <= 0) 
		{ 
			SetTimer(500, START_NEXT_LEVEL); 
		}
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (value == CREATE_NEW_PLAYER)
	{
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}

	if (value == START_NEXT_LEVEL)
	{
		mLevel++;
		int num_asteroids = 10 + 2 * mLevel;
		CreateAsteroids(num_asteroids);
	}

	if (value == SHOW_GAME_OVER)
	{
		mGameOverLabel->SetVisible(true);
	}

	else if (value == UPDATE_INVULNERABILITY)
	{
		int timeLeftSec = mSpaceship->GetInvulnerabilityTimer() / 1000;
		// Update the on-screen countdown
		std::ostringstream ss;
		ss << "Invulnerability: " << timeLeftSec << "s";
		mInvulnerabilityLabel->SetText(ss.str());

		if (timeLeftSec > 0) {
			// schedule next tick in 1s
			SetTimer(1000, UPDATE_INVULNERABILITY);
		}
		else {
			// hide countdown when time is up
			mInvulnerabilityLabel->SetVisible(false);
		}
	}



	if (value == SPAWN_POWERUP) {
		// pick random location
		float x = (rand() / (float)RAND_MAX) * 200.f - 100.f;
		float y = (rand() / (float)RAND_MAX) * 200.f - 100.f;
		GLVector3f pos(x, y, 0);

		// pick a random power up
		shared_ptr<GameObject> pu;
		switch (rand() % 3) {
		case 0: {
			auto p = make_shared<PU_ExtraLife>();
			p->Init();        // call Init() on the ExtraLife object
			pu = p;
			break;
		}
		case 1: {
			auto p = make_shared<PU_Invulnerability>();
			p->Init();        // call Init() on the Invulnerability object
			pu = p;
			break;
		}
		default: {
			auto p = make_shared<PU_BulletBoost>();
			p->Init();        // call Init() on the BulletBoost object
			pu = p;
			break;
			}
		}


		pu->SetPosition(pos);
		pu->SetScale(0.1f);
		mGameWorld->AddObject(pu);

		// reschedule…
		int next = (rand() % 11 + 5) * 1000;
		SetTimer(next, SPAWN_POWERUP);
	}
}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);
	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	// Return the spaceship so it can be added to the world
	return mSpaceship;

}


//Updated CreateAsteroids method so they do not spawn too close to player
void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++) {
		shared_ptr<Asteroid> asteroid = make_shared<Asteroid>(true);
		GLVector3f pos;
		// Generate a position at least 30 units away from the spaceship
		do {
			pos.x = (rand() / (float)RAND_MAX) * 200.0f - 100.0f; // World width: 200 units (-100 to 100)
			pos.y = (rand() / (float)RAND_MAX) * 200.0f - 100.0f; // World height: 200 units (-100 to 100)
			pos.z = 0.0f;
		} while ((pos - mSpaceship->GetPosition()).length() < 30.0f); // Minimum distance of 30 units
		asteroid->SetPosition(pos);

		// Set up asteroid sprite and properties
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.2f);
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));

		mGameWorld->AddObject(asteroid);
	}
}

void Asteroids::CreateGUI()
{
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> score_component
		= static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> game_over_component
		= static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));




	mInvulnerabilityLabel = make_shared<GUILabel>("Invulnerability: 0s");
	mInvulnerabilityLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mInvulnerabilityLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_RIGHT);
	mInvulnerabilityLabel->SetVisible(false);
	shared_ptr<GUIComponent> invuln_component = static_pointer_cast<GUIComponent>(mInvulnerabilityLabel);
	mGameDisplay->GetContainer()->AddComponent(invuln_component, GLVector2f(1.0f, 1.0f));

}

void Asteroids::OnScoreChanged(int score)
{
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

	if (lives_left > 0) 
	{ 
		SetTimer(1000, CREATE_NEW_PLAYER); 
	}
	else
	{
		SetTimer(500, SHOW_GAME_OVER);
	}
}

shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}



void Asteroids::OnPowerUpActivated(const std::string& powerUp, int duration)
{
	std::cout << "Power-up activated: " << powerUp << " for " << duration / 1000 << "s" << std::endl;
	if (powerUp == "Invulnerability") {
		mInvulnerabilityLabel->SetVisible(true);
		SetTimer(1000, UPDATE_INVULNERABILITY);
	}
	if (powerUp == "ExtraLife") {
		//Gives the player an extra life
		mPlayer.GrantExtraLife();

		//Pull back the new total
		int newLives = mPlayer.GetLives();

		//updates the on-screen label
		std::ostringstream ss;
		ss << "Lives: " << newLives;
		mLivesLabel->SetText(ss.str());
	}
}

void Asteroids::OnPowerUpDeactivated(const std::string& powerUp)
{
	if (powerUp == "Invulnerability") {
		//When not active the power up timer is not visible
		mInvulnerabilityLabel->SetVisible(false);
	}
	
}








