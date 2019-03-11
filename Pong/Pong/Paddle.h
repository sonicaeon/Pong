#pragma once

#include "DrawableGameComponent.h"
#include "Rectangle.h"
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <wrl.h>

namespace BouncingBall
{
	class Paddle final : public Library::DrawableGameComponent
	{
	public:
		Paddle(Library::Game& game);
		
		const Library::Rectangle& Bounds() const;
		
		virtual void Initialize() override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;
		
		void Reset();

	private:
		static const int PaddleSpeed;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTexture;
		Library::Point mTextureHalfSize;
		Library::Rectangle mBounds, nBounds;
		DirectX::XMFLOAT2 mVelocity;
		Library::KeyboardComponent* mKeyboard;
	};
}