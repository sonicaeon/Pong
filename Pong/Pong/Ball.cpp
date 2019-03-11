#include "pch.h"
#include "Ball.h"

using namespace DirectX;
using namespace Library;
using namespace std;
using namespace Microsoft::WRL;

namespace BouncingBall
{
	const int Ball::MinBallSpeed = 200;
	const int Ball::MaxBallSpeed = 400;
	XMFLOAT2 ballDirection;
	extern int playerScore, cpuScore;
	extern bool newGameFlag;
	extern Library::Rectangle playerPosition, cpuPosition;

	random_device Ball::sDevice;
	default_random_engine Ball::sGenerator(sDevice());
	uniform_int_distribution<int> Ball::sBoolDistribution(0, 1);
	uniform_int_distribution<int> Ball::sSpeedDistribution(MinBallSpeed, MaxBallSpeed);
	std::unique_ptr<SoundEffect> blip, destruction, ring;

	Ball::Ball(Game& game) :
		DrawableGameComponent(game), mBounds(Rectangle::Empty)
	{
	}
	
	const Library::Rectangle& Ball::Bounds() const
	{
		return mBounds;
	}

	DirectX::XMFLOAT2& Ball::Velocity()
	{
		return mVelocity;
	}

	void Ball::Initialize()
	{
		// Load a texture
		ComPtr<ID3D11Resource> textureResource;
		wstring textureName = L"Content\\Textures\\Ball.png";

		// Init audio engine
		CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		// Load AudioEngine debug mode
		AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
		#ifdef _DEBUG
		eflags = eflags | AudioEngine_Debug;
		#endif
		audEngine = std::make_unique<AudioEngine>(eflags);

		// Load SFX's
		blip = std::make_unique<SoundEffect>(audEngine.get(), L"Content\\Sounds\\pongblip.wav");
		destruction = std::make_unique<SoundEffect>(audEngine.get(), L"Content\\Sounds\\destruction.wav");
		ring = std::make_unique<SoundEffect>(audEngine.get(), L"Content\\Sounds\\menu-ok.wav");

		// Create the texture from the file; textureResource.ReleaseAndGetAddressOf() can be nullptr if don't need width/height
		ThrowIfFailed(CreateWICTextureFromFile(mGame->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mTexture.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");

		ComPtr<ID3D11Texture2D> texture;
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");

		// Calculates the bounds of the texture, which will be used for collision detection
		mBounds = TextureHelper::GetTextureBounds(texture.Get());
		mTextureHalfSize.X = mBounds.Width / 2;
		mTextureHalfSize.Y = mBounds.Height / 2;

		Reset();
	}

	void Ball::Update(const Library::GameTime& gameTime)
	{
		float elapsedTime = gameTime.ElapsedGameTimeSeconds().count();

		// Updates the position of the Ball
		XMFLOAT2 positionDelta(mVelocity.x * elapsedTime, mVelocity.y * elapsedTime);
		mBounds.X += static_cast<int>(std::round(positionDelta.x));
		mBounds.Y += static_cast<int>(std::round(positionDelta.y));

		// Collision Detection b/w the Ball and the viewport(screen); +X is Right, -X is Left, +Y is Down, -Y is Up
		auto& viewport = mGame->Viewport();
		// Handle score, and resetting the ball's position
		if (mBounds.X + mBounds.Width > viewport.Width && mVelocity.x > 0.0f)
		{
			// Ball scores off right side of screen
			playerScore++;
			ring->Play();

			if (playerScore == 3)
			{
				destruction->Play(); // Play game over sfx
			}

			// Ensure that the scores are reset and not accessed upon starting a new game
			if (newGameFlag)
			{
				playerScore = 0;
				newGameFlag = false;
			}
			// Reset Ball to center of the screen
			Ball::Reset();
		}
		if (mBounds.X < 0 && mVelocity.x < 0.0f)
		{
			// Ball scores off left side of screen
			cpuScore++;
			ring->Play();

			if (cpuScore == 3)
			{
				destruction->Play(); // Play game over sfx
			}

			// Ensure that the scores are reset and not accessed upon starting a new game
			if (newGameFlag)
			{
				cpuScore = 0;
				newGameFlag = false;
			}
			// Reset Ball to center of the screen
			Ball::Reset();
		}

		// Handles collisions w/ top and bottom screen
		if (mBounds.Y + mBounds.Height >= viewport.Height && mVelocity.y > 0.0f)
		{
			blip->Play();
			mVelocity.y *= -1; // Rebounds off the Bottom of the screen
		}
		if (mBounds.Y <= 0 && mVelocity.y < 0.0f)
		{
			blip->Play();
			mVelocity.y *= -1; // Rebounds off the Top of the screen
		}
		
		// Handles collision w/ paddles
		if (mBounds.Intersects(playerPosition))
		{
			blip->Play();
			mVelocity.x *= -1; // Rebounds off the player's paddle
		}
		if (mBounds.Intersects(cpuPosition))
		{
			blip->Play();
			mVelocity.x *= -1; // Rebounds off the cpu's paddle
		}

		// pass the ball's velocity
		ballDirection = mVelocity;
	}

	void Ball::Draw(const Library::GameTime& gameTime)
	{
		UNREFERENCED_PARAMETER(gameTime);

		XMFLOAT2 position(static_cast<float>(mBounds.X), static_cast<float>(mBounds.Y));
		SpriteManager::DrawTexture2D(mTexture.Get(), position);
	}

	void Ball::Reset()
	{
		// Grab the viewportSize of the screen, calculate the bounds of the center and Spawns the ball in the middle of the screen
		Library::Rectangle viewportSize(static_cast<int>(mGame->Viewport().TopLeftX), static_cast<int>(mGame->Viewport().TopLeftY), static_cast<int>(mGame->Viewport().Width), static_cast<int>(mGame->Viewport().Height));
		Point center = viewportSize.Center();
		mBounds.X = center.X - mTextureHalfSize.X;
		mBounds.Y = center.Y - mTextureHalfSize.Y;

		// randomize the velocity
		mVelocity.x = static_cast<float>(sSpeedDistribution(sGenerator) * (sBoolDistribution(sGenerator) ? 1 : -1));
		mVelocity.y = static_cast<float>(sSpeedDistribution(sGenerator) * (sBoolDistribution(sGenerator) ? 1 : -1));
	}
}