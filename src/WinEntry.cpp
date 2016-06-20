#include "WinEntry.h"

using namespace aa;

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
bool quit = false;
static HANDLE renderThreadHandle = NULL;

//--------------------------------------------------------------------------------------
// aa::window and aa::input
//--------------------------------------------------------------------------------------
HINSTANCE window::g_hInst = nullptr;
HWND window::g_hWnd = nullptr;
HGLRC window::g_hRC = nullptr;
HDC window::g_hDC = nullptr;

void(*input::mouseMoveF)(int, int, unsigned int, unsigned int) = NULL;
void(*input::mouseDownF)(int) = NULL;
void(*input::mouseUpF)(int) = NULL;
void(*input::mouseDoubleF)(int) = NULL;
void(*input::keyDownF)(unsigned int) = NULL;
void(*input::keyUpF)(unsigned int) = NULL;
void(*input::resizeF)(unsigned int, unsigned int) = NULL;
bool input::keys[256];
unsigned int input::mx = 0;
unsigned int input::my = 0;
void input::Process()
{
    MSG msg = { 0 };
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if (msg.message == WM_QUIT) { ::quit = true; }
}

bool window::windowShouldClose()
{
    return ::quit;
}

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef USE_CONSOLE_BACKEND
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif

    // init
    if (FAILED(window::InitWindow(hInstance, nCmdShow)))
        return 0;

    // launch thread
    //renderThreadHandle = CreateThread(0, 0, &BackendProc, 0, 0, 0);

    ShowWindow(window::g_hWnd, SW_SHOW);
    SetForegroundWindow(window::g_hWnd);
    SetFocus(window::g_hWnd);

    // main loop
    window::InitBackend();
    main();
    window::CleanupBackend();

    // cleanup
    /*WaitForSingleObject(renderThreadHandle, INFINITE);
    SuspendThread(renderThreadHandle);
    CloseHandle(renderThreadHandle);*/

    window::CleanupWindow();

    return 0;
}

//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT window::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"PrtWinClass";
    wcex.hIconSm = wcex.hIcon;
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    window::g_hInst = hInstance;
    RECT rc = { 0, 0, 640, 480 };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    window::g_hWnd = CreateWindow(L"PrtWinClass", L"PRT",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
        nullptr);

    if (!window::g_hWnd)
        return E_FAIL;

    //ShowWindow(g_hWnd, nCmdShow);

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Release resources, destroy window and unregister class
//--------------------------------------------------------------------------------------
void window::CleanupWindow()
{
    if (g_hWnd && !DestroyWindow(g_hWnd))
    {
        MessageBox(NULL, L"Could Not Release hWnd.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        g_hWnd = NULL;
    }

    if (!UnregisterClass(L"PrtWinClass", g_hInst))
    {
        MessageBox(NULL, L"Could Not Unregister Class.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        g_hInst = NULL;
    }
}

// --------------------------------------------------------------------------------------
// Called every time the application receives a message
//---------------------------------------------------------------------------------------
LRESULT CALLBACK window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

#ifdef USE_ANTTWBAR_BACKEND
    TwEventWin(hWnd, message, wParam, lParam);
#endif

    switch (message)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        input::keys[wParam] = true;
        if (input::keyDownF)
            (input::keyDownF)(wParam);
        return 0;
    case WM_KEYUP:
        input::keys[wParam] = false;
        if (input::keyUpF)
            (*input::keyUpF)(wParam);
        return 0;
    case WM_MOUSEMOVE:
    {
        input::mx = LOWORD(lParam);
        input::my = HIWORD(lParam);
        if (input::mouseMoveF)
            (*input::mouseMoveF)
            (
            wParam,
            ((wParam == input::Key::MOUSE_LEFT || wParam == input::Key::MOUSE_MIDDLE || wParam == input::Key::MOUSE_RIGHT) ? 1 : 0),
            input::mx,
            input::my
            );
        return 0;
    }
    case WM_LBUTTONDOWN:
        input::keys[input::Key::MOUSE_LEFT] = true;
        if (input::mouseDownF)
            (*input::mouseDownF)(input::Key::MOUSE_LEFT);
        return 0;
    case WM_MBUTTONDOWN:
        input::keys[input::Key::MOUSE_MIDDLE] = true;
        if (input::mouseDownF)
            (*input::mouseDownF)(input::Key::MOUSE_MIDDLE);
        return 0;
    case WM_RBUTTONDOWN:
        input::keys[input::Key::MOUSE_RIGHT] = true;
        if (input::mouseDownF)
            (*input::mouseDownF)(input::Key::MOUSE_RIGHT);
        return 0;
    case WM_LBUTTONUP:
        input::keys[input::Key::MOUSE_LEFT] = false;
        if (input::mouseUpF)
            (*input::mouseUpF)(input::Key::MOUSE_LEFT);
        return 0;
    case WM_MBUTTONUP:
        input::keys[input::Key::MOUSE_MIDDLE] = false;
        if (input::mouseUpF)
            (*input::mouseUpF)(input::Key::MOUSE_MIDDLE);
        return 0;
    case WM_RBUTTONUP:
        input::keys[input::Key::MOUSE_RIGHT] = false;
        if (input::mouseUpF)
            (*input::mouseUpF)(input::Key::MOUSE_RIGHT);
        return 0;
    case WM_LBUTTONDBLCLK:
        if (input::mouseDoubleF)
            (*input::mouseDoubleF)(input::Key::MOUSE_LEFT);
        return 0;
    case WM_MBUTTONDBLCLK:
        if (input::mouseDoubleF)
            (*input::mouseDoubleF)(input::Key::MOUSE_MIDDLE);
        return 0;
    case WM_RBUTTONDBLCLK:
        if (input::mouseDoubleF)
            (*input::mouseDoubleF)(input::Key::MOUSE_RIGHT);
        return 0;
    case WM_SIZE:
        if (input::resizeF)
            (*input::resizeF)(LOWORD(lParam), HIWORD(lParam));
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

//--------------------------------------------------------------------------------------
// Initialize drawing API
//--------------------------------------------------------------------------------------
HRESULT window::InitBackend()
{
    if (!(g_hDC = GetDC(g_hWnd)))
    {
        printf("Error: Device fail.\n");
        return E_FAIL;
    }

    int format;
    static PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR), // Size Of This Pixel Format Descriptor
        1, // Version Number
        PFD_DRAW_TO_WINDOW | // Format Must Support Window
        PFD_SUPPORT_OPENGL | // Format Must Support OpenGL
        PFD_DOUBLEBUFFER, // Must Support Double Buffering
        PFD_TYPE_RGBA, // Request An RGBA Format
        32, // Select Our Color Depth
        0, 0, 0, 0, 0, 0, // Color Bits Ignored
        0, // No Alpha Buffer
        0, // Shift Bit Ignored
        0, // No Accumulation Buffer
        0, 0, 0, 0, // Accumulation Bits Ignored
        16, // 16Bit Z-Buffer (Depth Buffer)
        0, // No Stencil Buffer
        0, // No Auxiliary Buffer
        PFD_MAIN_PLANE, // Main Drawing Layer
        0, // Reserved
        0, 0, 0
    };

    if (!(format = ChoosePixelFormat(g_hDC, &pfd)))
    {
        printf("Error: PixelFormat fail.\n");
        return E_FAIL;
    }
    if (!SetPixelFormat(g_hDC, format, &pfd))
    {
        printf("Error: PixelFormat not set.\n");
        return E_FAIL;
    }
    if (!(g_hRC = wglCreateContext(g_hDC)))
    {
        printf("Error: Context not creted.\n");
        return E_FAIL;
    }
    if (!wglMakeCurrent(g_hDC, g_hRC))
    {
        printf("Error: Context not set.\n");
        return E_FAIL;
    }

    // initialize glew
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        printf("Error: GLEW fail.\n");
        return E_FAIL;
    }

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Release drawing API resources
//--------------------------------------------------------------------------------------
void window::CleanupBackend()
{
    if (g_hRC)
    {
        if (!wglMakeCurrent(NULL, NULL))
            MessageBox(NULL, L"Release Of DC And RC Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        if (!wglDeleteContext(g_hRC))
            MessageBox(NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        g_hRC = NULL;
    }

    if (g_hDC && !ReleaseDC(g_hWnd, g_hDC))
    {
        MessageBox(NULL, L"Release Device Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        g_hDC = NULL;
    }
}

//--------------------------------------------------------------------------------------
// Swap buffers and display
//--------------------------------------------------------------------------------------
void window::SwapBuffersBackend()
{
    SwapBuffers(g_hDC);
}