#pragma once

#include "Game.h"
#include "Ball.h"
#include "Paddle.h"

namespace Library
{
	class KeyboardComponent;
}

namespace BouncingBall
{
	class BouncingBallGame : public Library::Game
	{
	public:
		BouncingBallGame(std::function<void*()> getWindowCallback, std::function<void(SIZE&)> getRenderTargetSizeCallback);

		virtual void Initialize() override;
		virtual void Shutdown() override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;
		
	private:
		void Exit();
		void GameOver();

		static const DirectX::XMVECTORF32 BackgroundColor;

		std::shared_ptr<Library::KeyboardComponent> mKeyboard;
		std::shared_ptr<Ball> mBall;
		std::shared_ptr<Paddle> mPaddle;
		std::shared_ptr<DirectX::SpriteFont> mFont;
		std::wstring mMessage;
		DirectX::XMFLOAT2 mMessagePosition;
	};
}