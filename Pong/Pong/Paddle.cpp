#include "pch.h"
#include "Paddle.h"

using namespace DirectX;
using namespace Library;
using namespace std;
using namespace Microsoft::WRL;

namespace BouncingBall
{
	const int Paddle::PaddleSpeed = 10;
	extern XMFLOAT2 ballDirection;
	
	Library::Rectangle playerPosition(Rectangle::Empty), cpuPosition(Rectangle::Empty);
	
	// Initializer list
	Paddle::Paddle(Game& game) :
		DrawableGameComponent(game), mBounds(Rectangle::Empty), nBounds(Rectangle::Empty)
	{
	}

	const Library::Rectangle& Paddle::Bounds() const
	{
		return mBounds;
	}

	void Paddle::Initialize()
	{
		// Load a texture
		ComPtr<ID3D11Resource> textureResource;
		wstring textureName = L"Content\\Textures\\Paddle.png";

		ThrowIfFailed(CreateWICTextureFromFile(mGame->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mTexture.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");

		ComPtr<ID3D11Texture2D> texture;
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");

		// Calculate texture bounds for left/right paddle
		mBounds = TextureHelper::GetTextureBounds(texture.Get());
		mTextureHalfSize.X = mBounds.Width / 2;
		mTextureHalfSize.Y = mBounds.Height / 2;

		nBounds = TextureHelper::GetTextureBounds(texture.Get());

		// Get Keyboard service
		mKeyboard = reinterpret_cast<KeyboardComponent*>(mGame->Services().GetService(KeyboardComponent::TypeIdClass()));
		
		Reset();
	}

	void Paddle::Update(const Library::GameTime& gameTime)
	{
		int randMultiplier, randBool;
		auto& viewport = mGame->Viewport();

		// Generate random seed
		float currentTime = gameTime.ElapsedGameTimeSeconds().count();
		srand((int)currentTime);
		// Generate # 1 - 10 to create a random paddleSpeed multiplier, and a rand bool (0 or 1)
		randMultiplier = (rand() % 10) + 1;
		randBool = rand() % 2;

		// Set the Player's Paddle controls for keyboard
		if (mKeyboard->WasKeyDown(Keys::Up) | mKeyboard->WasKeyDown(Keys::Left))
		{
			// Set the upper Bounds for Paddle for the viewport
			if (!(mBounds.Y <= 0))
			{
				mBounds.Y -= PaddleSpeed; // move the paddle up
			}
			randBool = rand() % 2;
		}
		if (mKeyboard->WasKeyDown(Keys::Down) | mKeyboard->WasKeyDown(Keys::Right))
		{
			// Set the lower Bounds for Paddle for the viewport
			if (!(mBounds.Y + mBounds.Height >= viewport.Height))
			{
				mBounds.Y += PaddleSpeed; // move the paddle down
			}
			randBool = rand() % 2;
		}
		
		// Pseudo paddle ai for CPU
		if (!(nBounds.Y + nBounds.Height >= viewport.Height) && ballDirection.y > 0.0f && ballDirection.x > 0.0f)
		{
			if (randBool)
			{
				// if ball moves downward, move paddle down randomly
				nBounds.Y += randMultiplier;
			}
			else if (!(nBounds.Y <= 0))
			{
				// otherwise, move up randomly
				nBounds.Y -= randMultiplier;
			}
			else
				nBounds.Y += (randMultiplier / 2);
		}
		if (!(nBounds.Y <= 0) && ballDirection.y < 0.0f && ballDirection.x > 0.0f)
		{
			if (randBool)
			{
				// if ball moves upwards, move paddle up randomly
				nBounds.Y -= randMultiplier;
			}
			else if (!(nBounds.Y + nBounds.Height >= viewport.Height))
			{
				// otherwise, move down randomly
				nBounds.Y += randMultiplier;
			}
			else
				nBounds.Y -= (randMultiplier / 2);
		}
		// Keep reference for paddle's position
		playerPosition = mBounds;
		cpuPosition = nBounds;
	}

	void Paddle::Draw(const Library::GameTime& gameTime)
	{
		UNREFERENCED_PARAMETER(gameTime);

		// Draw left and right paddle using bounds calculated
		XMFLOAT2 position(static_cast<float>(mBounds.X), static_cast<float>(mBounds.Y));
		SpriteManager::DrawTexture2D(mTexture.Get(), position);
		XMFLOAT2 positionCPU(static_cast<float>(nBounds.X), static_cast<float>(nBounds.Y));
		SpriteManager::DrawTexture2D(mTexture.Get(), positionCPU);
	}

	void Paddle::Reset()
	{
		// find viewport Size and center
		Library::Rectangle viewportSize(static_cast<int>(mGame->Viewport().TopLeftX), static_cast<int>(mGame->Viewport().TopLeftY), static_cast<int>(mGame->Viewport().Width), static_cast<int>(mGame->Viewport().Height));
		Point center = viewportSize.Center();

		// Spawn the paddle to the left of the screen
		mBounds.X = 8;
		mBounds.Y = center.Y - mTextureHalfSize.Y;

		// Spawn the right paddle (computer)
		nBounds.X = viewportSize.Width - 16;
		nBounds.Y = center.Y - mTextureHalfSize.Y;
	}
}