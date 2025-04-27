	#ifndef __SCOREKEEPER_H__
	#define __SCOREKEEPER_H__

	#include "GameUtil.h"

	#include "GameObject.h"
	#include "GameObjectType.h"
	#include "IScoreListener.h"
	#include "IGameWorldListener.h"
	#include "Asteroid.h"
	#include "IPlayerListener.h"

	class ScoreKeeper : public IGameWorldListener
	{
	public:
		ScoreKeeper() { mScore = 0; mLives = 3;}
		virtual ~ScoreKeeper() {}

		void OnWorldUpdated(GameWorld* world) {}
		void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}

		void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
		{
			if (object->GetType() == GameObjectType("Asteroid")) {
				auto asteroid = std::static_pointer_cast<Asteroid>(object);
				if (!asteroid->DestroyedBySpaceship()) { // Skip scoring if destroyed by spaceship
					if (asteroid->IsLarge()) {
						mScore += 20; // More points for large asteroids
					}
					else {
						mScore += 10; // Fewer points for small asteroids
					}
					FireScoreChanged();
				}
			}
			else if (object->GetType() == GameObjectType("PowerUpExtraLife")) {
				// Extra life collected
				mLives++;
				FirePlayerKilled(mLives);
			}
			else if (object->GetType() == GameObjectType("Spaceship")) {
				//Spaceship damaged, remove it and deduct life
				mLives--;
				FirePlayerKilled(mLives);
			}
		}

		void AddListener(shared_ptr<IScoreListener> listener)
		{
			mListeners.push_back(listener);
		}

		void FireScoreChanged()
		{
			// Send message to all listeners
			for (ScoreListenerList::iterator lit = mListeners.begin(); lit != mListeners.end(); ++lit) {
				(*lit)->OnScoreChanged(mScore);
			}
		}




		void OnPlayerKilled(int lives_left)
		{
			FirePlayerKilled(lives_left);
		}

		void AddPlayerListener(shared_ptr<IPlayerListener> listener)
		{
			mPlayerListeners.push_back(listener);
		}

		void FirePlayerKilled(int lives_left)
		{
			for (PlayerListenerList::iterator lit = mPlayerListeners.begin(); lit != mPlayerListeners.end(); ++lit) {
				(*lit)->OnPlayerKilled(lives_left);
			}
		}

	private:
		int mScore;

		typedef std::list< shared_ptr<IScoreListener> > ScoreListenerList;

		ScoreListenerList mListeners;


		int mLives;
		typedef std::list< shared_ptr<IPlayerListener> > PlayerListenerList;
		PlayerListenerList mPlayerListeners;
	};

	#endif
