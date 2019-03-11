#include "pch.h"
#include "BouncingBallGame.h"

using namespace std;
using namespace DirectX;
using namespace Library;
using namespace Microsoft::WRL;

namespace BouncingBall
{
	const XMVECTORF32 BouncingBallGame::BackgroundColor = Colors::Black;
	extern int playerScore, cpuScore;
	bool newGameFlag = false;
	const SIZE RenderTargetSize = { 200, 400 };

	HWND windowHandle;
	WNDCLASSEX window;
	static const wstring windowTitle = L"Game Over";
	static const wstring windowContent1 = L"YOU WIN!\nWOULD YOU LIKE TO PLAY AGAIN?";
	static const wstring windowContent2 = L"YOU LOSE!\nWOULD YOU LIKE TO PLAY AGAIN?";

	BouncingBallGame::BouncingBallGame(function<void*()> getWindowCallback, function<void(SIZE&)> getRenderTargetSizeCallback) :
		Game(getWindowCallback, getRenderTargetSizeCallback)
	{
	}

	void BouncingBallGame::Initialize() // init sprites and comps
	{
		SpriteManager::Initialize(*this);
		BlendStates::Initialize(mDirect3DDevice.Get());

		// instantiate the keyboard, and other components
		mKeyboard = make_shared<KeyboardComponent>(*this);
		mComponents.push_back(mKeyboard);
		mServices.AddService(KeyboardComponent::TypeIdClass(), mKeyboard.get());

		mBall = make_shared<Ball>(*this);
		mComponents.push_back(mBall);

		mPaddle = make_shared<Paddle>(*this);
		mComponents.push_back(mPaddle);
		mServices.AddService(KeyboardComponent::TypeIdClass(), mPaddle.get());

		mFont = make_shared<SpriteFont>(mDirect3DDevice.Get(), L"Content\\Fonts\\Arial_36_Regular.spritefont");

		Game::Initialize();
	}

	void BouncingBallGame::Shutdown()
	{
		BlendStates::Shutdown();
		SpriteManager::Shutdown();
	}

	void BouncingBallGame::Update(const GameTime &gameTime)
	{
		if (mKeyboard->WasKeyPressedThisFrame(Keys::Escape))
		{
			Exit();
		}

		// Get the viewport size
		XMFLOAT2 tempViewportSize(mViewport.Width, mViewport.Height);
		XMVECTOR viewportSize = XMLoadFloat2(&tempViewportSize);

		// Get the message size
		XMVECTOR messageSize = mFont->MeasureString(mMessage.c_str());
		// Store and set the message position for drawing the text on screen
		XMStoreFloat2(&mMessagePosition, (viewportSize - messageSize) / 2);
		mMessagePosition.y = 0;

		Game::Update(gameTime);
	}

	void BouncingBallGame::Draw(const GameTime &gameTime)
	{
		// Double bufferings, clears and renders into the back buffer
		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		
		// Display the current score
		wostringstream messageStream;
		messageStream << playerScore << " : " << cpuScore;
		mMessage = messageStream.str();

		// Draw the string at the position to the screen
		SpriteManager::DrawString(mFont, mMessage.c_str(), mMessagePosition);

		Game::Draw(gameTime);
		// Swaps the front buffer and back buffer; Sync w/ refresh rate (Present(1,0)) ~60 FPS to avoid screen tearing, (Present(0,0)) for uncapped FPS
		HRESULT hr = mSwapChain->Present(1, 0);

		// If the device was removed either by a disconnection or a driver upgrade, we must recreate all device resources.
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			HandleDeviceLost();
		}
		else
		{
			ThrowIfFailed(hr, "IDXGISwapChain::Present() failed.");
		}
		if (playerScore == 3 || cpuScore == 3)
		{
			Sleep(250);
			GameOver();
		}
	}

	void BouncingBallGame::GameOver()
	{
		HWND mainWindow = ::FindWindow(NULL, L"Pong");

		// Handle game win-state conditions
		if (playerScore == 3)
		{
			if (MessageBox(windowHandle, windowContent1.c_str(), windowTitle.c_str(), MB_YESNO) == IDYES)
			{
				// If Yes, reset the score and reset into a new game
				playerScore = 0;
				cpuScore = 0;
				newGameFlag = true;
				
				if (mainWindow)
				{
					// move to foreground
					::SetForegroundWindow(mainWindow);
				}
				Sleep(500); // delay upon starting new game
			}
			else // If No, exit
			{
				Exit();
			}
		}
		else if (cpuScore == 3)
		{
			if (MessageBox(windowHandle, windowContent2.c_str(), windowTitle.c_str(), MB_YESNO) == IDYES)
			{
				// If Yes, reset the score and reset into a new game
				playerScore = 0;
				cpuScore = 0;
				newGameFlag = true;

				if (mainWindow)
				{
					// move to foreground
					::SetForegroundWindow(mainWindow);
				}
				Sleep(500); // delay upon starting new game
			}
			else // If No, exit
			{
				Exit();
			}
		}
	}

	void BouncingBallGame::Exit()
	{
		PostQuitMessage(0);
	}
}