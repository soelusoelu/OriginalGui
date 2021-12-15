#include "Game.h"
#include "AssetsManager.h"
#include "GlobalFunction.h"
#include "SceneManager.h"
#include "Window.h"
#include "FpsCounter/FPSCounter.h"
#include "Json/JsonObject.h"
#include "Json/JsonValue.h"
#include "Shader/Shader.h"
#include "Texture/Texture.h"
#include "../DirectX/DirectX.h"
#include "../Engine/DebugManager/DebugUtility/LineRenderer/LineRenderer.h"
#include "../GameObject/GameObjectFactory.h"
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_impl_dx11.h"
#include "../Imgui/imgui_impl_win32.h"
#include "../Input/InputManager.h"
#include "../Input/Mouse.h"
#include "../Sound/XAudio2/SoundEngine.h"
#include "../Utility/LevelLoader.h"
#include "../Utility/Random.h"
#include "../System/Gui/Gui.h"
#include "../System/Gui/GuiContext.h"
#include "../System/Gui/GuiWindow.h"

Game::Game()
    : mWindow(nullptr)
    , mFPSCounter(nullptr)
    , mRootObject(nullptr)
    , mGuiContext(nullptr)
    , mGuiWindow(nullptr)
    , mSceneManager(nullptr)
    , mInstance(nullptr)
{
}

Game::~Game() {
    saveGlobalFile();
    LevelLoader::saveJson(*mRootObject, GLOBAL_DATA_FILE_NAME);

    safeDelete(mSceneManager);

    //imguiの終了処理
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    LineRenderer::finalize();
    Shader::finalize();
    Texture::finalize();
    GameObjectCreater::finalize();
    InputManager::finalize();
    SoundEngine::instance().finalize();
    AssetsManager::instance().finalize();
    MyDirectX::DirectX::instance().finalize();
}

void Game::run(HINSTANCE hInstance) {
    mInstance = hInstance;
    initialize();

    MSG msg = { 0 };
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            mainLoop();
        }
    }
}

void Game::quit() {
    PostQuitMessage(0);
}

void Game::initialize() {
    mWindow = std::make_unique<Window>();

    mFPSCounter = std::make_unique<FPSCounter>();
    InputManager::create();
    mSceneManager = new SceneManager();

    //ファイルから値を読み込む
    mRootObject = std::make_shared<JsonObject>();
    LevelLoader::loadJson(*mRootObject, GLOBAL_DATA_FILE_NAME);
    loadGlobalFile();

    mWindow->initialize(&InputManager::mouse());
    mWindow->createWindow(mInstance);
    const auto& hwnd = mWindow->gethWnd();

    MyDirectX::DirectX::instance().initialize(hwnd);

    //imguiの初期化
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(MyDirectX::DirectX::instance().device(), MyDirectX::DirectX::instance().deviceContext());

    Random::initialize();
    InputManager::initialize(hwnd);
    GameObjectCreater::initialize();
    mSceneManager->initialize();

    mGuiContext = std::make_unique<GuiContext>();
    mGuiWindow = Gui::createWindow("");
    static int test = 25;
    mGuiWindow->sliderInt("sliderInt", test, 0, 100);
    static float test2 = 0.f;
    mGuiWindow->sliderFloat("sliderFloat", test2, -100.f, 100.f);
    static Vector2 test4 = Vector2::zero;
    mGuiWindow->sliderVector2("sliderVector2", test4, -Vector2::one * 100.f, Vector2::one * 100.f);
    static Vector4 test3 = Vector4(1.f, 1.f, 1.f, 1.f);
    mGuiWindow->colorPicker4("colorPicker", test3);
    mGuiWindow->text("text");
}

void Game::mainLoop() {
    //imguiの描画前準備
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    auto& dx = MyDirectX::DirectX::instance();
    dx.clearRenderTarget(0.1f, 0.1f, 0.1f);
    dx.clearDepthStencilView();

    InputManager::update();
    mWindow->update();

    {
        //ウィンドウ位置
        ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Once);
        //ウィンドウサイズ
        ImGui::SetNextWindowSize(ImVec2(256.f, 512.f), ImGuiCond_Once);
        ////ウィンドウを不透明に
        //ImGui::SetNextWindowBgAlpha(1.f);
        const auto& mousePos = InputManager::mouse().getMousePosition();
        ImGui::GetIO().MousePos.x = mousePos.x;
        ImGui::GetIO().MousePos.y = mousePos.y;

        bool isOpen = false;
        ImGui::Begin("Test", &isOpen);
        static int num = 0;
        ImGui::DragInt("drag", &num);
        static int num2 = 0;
        ImGui::SliderInt("slide", &num2, 0, 100);
        static float num3[3] = { 0 };
        ImGui::SliderFloat3("slide", num3, 0.f, 100.f);
        //static ImVec4 col(1.f, 1.f, 1.f, 1.f);
        static float col[4] = { 1.f, 1.f, 1.f, 1.f };
        //ImGui::ColorEdit4("colorEdit", col);
        ImGui::ColorPicker4("colorPicker", col);
        static float col3[4] = { 1.f, 1.f, 1.f };
        ImGui::ColorPicker3("colorPicker3", col3);
        //ImVec4 c(col[0], col[1], col[2], col[3]);
        //ImGui::ColorButton("colorButton", c);
        ImGui::End();
    }

    mSceneManager->update();
    mSceneManager->draw();

    //mGuiWindow->setPosition(InputManager::mouse().getMousePosition());
    //mGuiWindow->setTitleBarColor(Vector4(1.f, 0.f, 0.f, 1.f));
    mGuiContext->update();

    InputManager::lateUpdate();

    //imguiの描画
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    mGuiContext->draw();

    mFPSCounter->fixedFrame();
    dx.present();

    SoundEngine::instance().update();
}

void Game::loadGlobalFile() {
    auto& globalPropertiesValue = mRootObject->getValue("globalProperties");
    auto& globalPropertiesObj = globalPropertiesValue.getObject();
    mWindow->writeAndRead(globalPropertiesObj, FileMode::LOAD);
    mFPSCounter->writeAndRead(globalPropertiesObj, FileMode::LOAD);
    InputManager::saveAndLoad(globalPropertiesObj, FileMode::LOAD);
    mSceneManager->writeAndRead(globalPropertiesObj, FileMode::LOAD);
}

void Game::saveGlobalFile() {
    auto& globalPropertiesValue = mRootObject->getValue("globalProperties");
    auto& globalPropertiesObj = globalPropertiesValue.getObject();
    mWindow->writeAndRead(globalPropertiesObj, FileMode::SAVE);
    mFPSCounter->writeAndRead(globalPropertiesObj, FileMode::SAVE);
    InputManager::saveAndLoad(globalPropertiesObj, FileMode::SAVE);
    mSceneManager->writeAndRead(globalPropertiesObj, FileMode::SAVE);
}
