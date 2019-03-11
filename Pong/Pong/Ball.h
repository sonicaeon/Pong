#pragma once

#include "DrawableGameComponent.h"
#include "Rectangle.h"
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <Audio.h>

namespace BouncingBall
{
	class Ball final : public Library::DrawableGameComponent
	{
	public:
		Ball(Library::Game& game);

		const Library::Rectangle& Bounds() const;
		DirectX::XMFLOAT2& Velocity();

		virtual void Initialize() override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;

		void Reset();

	private:
		static const int MinBallSpeed;
		static const int MaxBallSpeed;

		// adds randomness to the ball on spawn
		static std::random_device sDevice;
		static std::default_random_engine sGenerator;
		static std::uniform_int_distribution<int> sBoolDistribution;
		static std::uniform_int_distribution<int> sSpeedDistribution;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTexture;
		Library::Point mTextureHalfSize;
		Library::Rectangle mBounds;
		DirectX::XMFLOAT2 mVelocity;
		std::unique_ptr<DirectX::AudioEngine> audEngine;
	};
}