#undef UNICODE
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <string>
#include <CommCtrl.h>

#include <FreeImage.h>
#include "resource.h"
#include "RenderStateMachine.h"

#define IDM_SHADOW 1
#define IDM_PHONG 2
#define IDM_MULTIPLEPHONG 3
#define IDM_SKYBOX 4
#define IDM_ADDLAMP 5
#define IDM_SIMPLE 6
#define IDM_LOADMESH 7
#define IDM_EXPORT_BMP 8
#define IDM_LOADTEXTURE 9
#define IDM_CHANGE_MESH 10
#define IDM_ADDTEXTUREDMESH 11
#define IDM_TEXTURED 12
#define IDM_CHANGE_LAMP 13 
#define IDM_REFLECTION 14
#define IDM_LIGHTS_SOURCES 15

#define MIN_SPECULAR_LEVEL 8
#define MAX_SPECULAR_LEVEL 512

int groupSize = 5;
int renderModesGroup[] = {IDM_SHADOW, IDM_PHONG, 
    IDM_MULTIPLEPHONG, IDM_REFLECTION, IDM_SIMPLE};

#define KEY_A 0x41
#define KEY_D 0x44
#define KEY_S 0x53
#define KEY_W 0x57
#define KEY_N 0x4E

#define PI 3.1415
#define WIDTH 1024
#define HEIGHT 600


HMENU hModeSubmenu;
HMENU hAddSubmenu;
HMENU hExportMenu;
HMENU hChangeMenu; 

HANDLE eventInitialized;
HANDLE initEvents[RenderStateMachine::INIT_STEPS_AMOUNT];

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void updateMovement(float time);
void processExportBmp(HWND hWnd);

RenderStateMachine* render;

bool keys[256];
Camera* camera;
float alpha;
bool isContinue;
bool isMakeScreenshot;

HDC hDC;
HGLRC hRC;
HGLRC threadRC;

void initFunctionsPointers();

void threadProc()
{
    if (!wglMakeCurrent(hDC, threadRC))
    {
        SetEvent(eventInitialized);
        return;
    }
    initFunctionsPointers();
    render = new RenderStateMachine(WIDTH, HEIGHT, initEvents);
    SetEvent(eventInitialized);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc;
    HWND hWnd;
    MSG msg;
    BOOL bQuit = FALSE;
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "classsdandart";
    RegisterClass(&wc);

    hWnd = CreateWindow("classsdandart", "RenderGPU",
        WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
        0, 0,
        WIDTH, HEIGHT,
        NULL, NULL, hInstance, NULL);

    HMENU hMenu = CreateMenu();

    hModeSubmenu = CreatePopupMenu();
    hAddSubmenu = CreatePopupMenu();
    hExportMenu = CreatePopupMenu();
    hChangeMenu = CreatePopupMenu();

    AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hModeSubmenu, "Change mode");
    AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hAddSubmenu, "Add objects");
    AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hChangeMenu, "Change objects");
    AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hExportMenu, "Export");

    AppendMenuA(hModeSubmenu, MF_STRING, IDM_SHADOW, "Shadow");
    AppendMenuA(hModeSubmenu, MF_STRING, IDM_PHONG, "Phong");
    AppendMenuA(hModeSubmenu, MF_STRING, IDM_MULTIPLEPHONG, "MultiplePhong");
    AppendMenuA(hModeSubmenu, MF_BYCOMMAND, IDM_SKYBOX, "Skybox");
    AppendMenuA(hModeSubmenu, MF_STRING, IDM_SIMPLE, "Simple");
    AppendMenuA(hModeSubmenu, MF_STRING, IDM_TEXTURED, "Textured");
    AppendMenuA(hModeSubmenu, MF_STRING, IDM_REFLECTION, "Reflections");
    AppendMenuA(hModeSubmenu, MF_STRING, IDM_LIGHTS_SOURCES, "Lamps");

    AppendMenuA(hAddSubmenu, MF_STRING, IDM_ADDLAMP, "Add lamp");
    AppendMenuA(hAddSubmenu, MF_STRING, IDM_LOADMESH, "Add mesh");
    AppendMenuA(hAddSubmenu, MF_STRING, IDM_LOADTEXTURE, "Add texture");
    AppendMenuA(hAddSubmenu, MF_STRING, IDM_ADDTEXTUREDMESH, "Add textured mesh");

    AppendMenuA(hChangeMenu, MF_STRING, IDM_CHANGE_MESH, "Change mesh");
    AppendMenuA(hChangeMenu, MF_STRING, IDM_CHANGE_LAMP, "Change lamp");

    AppendMenuA(hExportMenu, MF_STRING, IDM_EXPORT_BMP, "Export BMP");
    SetMenu(hWnd, hMenu);


    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    hDC = GetDC(hWnd);

    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, iFormat, &pfd);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    hRC = wglCreateContext(hDC);
    threadRC = wglCreateContext(hDC);

    if (!wglShareLists(threadRC, hRC))
    {
        MessageBox(hWnd, "Can not share context", "Error", MB_OK);
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -1.0, 1.0);
    
    initFunctionsPointers();
    HWND hwndPB;
    int progressWidth = 100, progressHeight = 20;

    hwndPB = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE, WIDTH / 2 - progressWidth / 2,
        HEIGHT / 2 - progressHeight / 2, progressWidth, progressHeight, hWnd, 0, hInstance, NULL);
    SendMessage(hwndPB, PBM_SETRANGE, 0, MAKELPARAM(0, RenderStateMachine::INIT_STEPS_AMOUNT));
    SendMessage(hwndPB, PBM_SETSTEP, 1, 0);

    eventInitialized = CreateEvent(NULL, TRUE, FALSE, "Initialized");
    for (int i = 0; i < RenderStateMachine::INIT_STEPS_AMOUNT; i++)
    {
        initEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
    }

    HANDLE threadInit = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProc, NULL, 0, NULL);
    SetWindowText(hWnd, "Init");
    for (int i = 0; i < RenderStateMachine::INIT_STEPS_AMOUNT; i++)
    {
        WaitForSingleObject(initEvents[i], INFINITE);
        SendMessage(hwndPB, PBM_STEPIT, 0, 0);
    }

    for (int i = 0; i < RenderStateMachine::INIT_STEPS_AMOUNT; i++)
    {
        CloseHandle(initEvents[i]);
    }

    WaitForSingleObject(eventInitialized, INFINITE);
    CloseHandle(eventInitialized);
    CloseHandle(threadInit);
    SetWindowText(hWnd, "Initialized");
    if (!wglMakeCurrent(hDC, hRC))
    {
        MessageBox(hWnd, "Can not make current", "Error", MB_OK);
        return -1;
    }
    render->initSkybox();
    render->initDepthFBO();
    DestroyWindow(hwndPB);
    glEnable(GL_DEPTH_TEST);

    int tm1 = clock();
    int tm2 = 0;
    int time;
    isContinue = true;
    while (isContinue)
    {
        tm2 = clock();
        time = tm2 - tm1;
        tm1 = tm2;

        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
       
        updateMovement(time);

        render->Render();

        SwapBuffers(hDC);

    //    Sleep(1);  // Pause (milliseconds)

    }

    delete render;
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);

    DestroyWindow(hWnd);
    return 0;
}

void initFunctionsPointers()
{

    glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
    glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
    glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
    glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
    glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
    glUniform3f = (PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f");
    glUniform4f = (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f");
    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
    glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
    glCreateFramebuffers = (PFNGLCREATEFRAMEBUFFERSPROC)wglGetProcAddress("glCreateFramebuffers");
    glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
    glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
    glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
    glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
    glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)wglGetProcAddress("glFramebufferTexture");
}

INT_PTR CALLBACK DlgProcAddLamp(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR strX[10];
    TCHAR strY[10];
    TCHAR strZ[10];
    static DWORD rgbCurrent = 0x000000FF;
    WORD length;
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HDC hdc = GetDC(hDlg);
        LOGBRUSH logBrush;
        logBrush.lbColor = rgbCurrent;
        logBrush.lbStyle = BS_SOLID;
        HBRUSH brush = CreateBrushIndirect(&logBrush);
        SelectObject(hdc, brush);
        RECT rect{ 250, 70, 270, 90 };
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);
        DeleteDC(hdc);
    }
    break;
    case WM_COMMAND:
    {
        switch (wParam)
        {
        case IDOK:
            {

                length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_X, EM_LINELENGTH, 0, 0);
                length = length > 1 ? length : 2;
                *((LPWORD)strX) = length;
                SendDlgItemMessageA(hDlg, IDC_EDIT_X, EM_GETLINE, 0, (LPARAM)strX);
                strX[length] = 0;

                length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_Y, EM_LINELENGTH, 0, 0);
                length = length > 1 ? length : 2;
                *((LPWORD)strY) = length;
                SendDlgItemMessageA(hDlg, IDC_EDIT_Y, EM_GETLINE, 0, (LPARAM)strY);
                strY[length] = 0;

                length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_Z, EM_LINELENGTH, 0, 0);
                length = length > 1 ? length : 2;
                *((LPWORD)strZ) = length;
                SendDlgItemMessageA(hDlg, IDC_EDIT_Z, EM_GETLINE, 0, (LPARAM)strZ);
                strZ[length] = 0;

                int x = atoi(strX);
                int y = atoi(strY);
                int z = atoi(strZ);

                float red = (rgbCurrent % 256) / 255;
                float green = ((rgbCurrent >> 8) % 256) / 255;
                float blue = ((rgbCurrent >> 16) % 256) / 255;

                render->lamps.push_back(new Ball(glm::vec3(x, y, z), glm::vec3(0, 1, 0), 0.5, 0, glm::vec3(red, green, blue)));
                EndDialog(hDlg, 0);
                return TRUE;
            }
        break;
        case IDC_BUTTON_CHANGECOLOR:
            {
                CHOOSECOLOR cc;
                static COLORREF customColors[] = { 0x00FF0000, 0x0000FF00, 0x000000FF };
                ZeroMemory(&cc, sizeof(cc));
                cc.lStructSize = sizeof(cc);
                cc.hwndOwner = hDlg;
                cc.Flags = CC_FULLOPEN;
                cc.lpCustColors = (LPDWORD)customColors;
                cc.rgbResult = rgbCurrent;
                cc.Flags = CC_FULLOPEN | CC_RGBINIT;

                if (ChooseColor(&cc) == TRUE)
                {
                    rgbCurrent = cc.rgbResult;
                    char strColor[20];
                    _itoa(rgbCurrent, strColor, 16);
                    SendDlgItemMessageA(hDlg, IDC_EDIT_COLOR, WM_SETTEXT, 0, (LPARAM)strColor);
                    HDC hdc = GetDC(hDlg);
                    LOGBRUSH logBrush;
                    logBrush.lbColor = cc.rgbResult;
                    logBrush.lbStyle = BS_SOLID;
                    HBRUSH brush = CreateBrushIndirect(&logBrush);
                    SelectObject(hdc, brush);
                    RECT rect{250, 70, 270, 90};
                    FillRect(hdc, &rect, brush);
                    DeleteObject(brush);
                    DeleteDC(hdc);
                }
                return TRUE;
            }
        break;
        }
        break;
    }
    case WM_CLOSE:
        EndDialog(hDlg, 0);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK DlgProcAddMesh(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR strX[10];
    TCHAR strY[10];
    TCHAR strZ[10];
    TCHAR strName[20];
    TCHAR strPath[100];
    TCHAR strSpec[10];
    WORD length;
    char szFile[260];
    MeshNormals* mesh;

    OPENFILENAMEA ofn;
    switch (message)
    {
    case WM_INITDIALOG:
    {
        int defaultSpecLevel = 32;
        TCHAR strDefaultSpec[10];
        _itoa(defaultSpecLevel, strDefaultSpec, 10);
        SendDlgItemMessage(hDlg, IDC_EDIT_SPEC, WM_SETTEXT, 0, (LPARAM)strDefaultSpec);
        return TRUE;
    }

    case WM_COMMAND:
    {
        switch (wParam)
        {
        case IDC_BUTTON_SELECTPATH:
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hDlg;
            ofn.lpstrFile = szFile;
            // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
            // use the contents of szFile to initialize itself.
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = "OBJ\0*.obj\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileNameA(&ofn) == TRUE)
            {
                SendDlgItemMessageA(hDlg, IDC_EDIT_PATH, WM_SETTEXT, 0, (LPARAM)ofn.lpstrFile);
            }
            return TRUE;

        case IDOK:
            length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_X, EM_LINELENGTH, 0, 0);
            length = length > 1 ? length : 2;
            *((LPWORD)strX) = length;
            SendDlgItemMessageA(hDlg, IDC_EDIT_X, EM_GETLINE, 0, (LPARAM)strX);
            strX[length] = 0;

            length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_Y, EM_LINELENGTH, 0, 0);
            length = length > 1 ? length : 2;
            *((LPWORD)strY) = length;
            SendDlgItemMessageA(hDlg, IDC_EDIT_Y, EM_GETLINE, 0, (LPARAM)strY);
            strY[length] = 0;

            length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_Z, EM_LINELENGTH, 0, 0);
            length = length > 1 ? length : 2;
            *((LPWORD)strZ) = length;
            SendDlgItemMessageA(hDlg, IDC_EDIT_Z, EM_GETLINE, 0, (LPARAM)strZ);
            strZ[length] = 0;


            length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_NAME, EM_LINELENGTH, 0, 0);
            *((LPWORD)strName) = length;
            SendDlgItemMessageA(hDlg, IDC_EDIT_NAME, EM_GETLINE, 0, (LPARAM)strName);
            strName[length] = 0;

            length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_PATH, EM_LINELENGTH, 0, 0);
            *((LPWORD)strPath) = length;
            SendDlgItemMessageA(hDlg, IDC_EDIT_PATH, EM_GETLINE, 0, (LPARAM)strPath);
            strPath[length] = 0;

            length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_SPEC, EM_LINELENGTH, 0, 0);
            *((LPWORD)strSpec) = length;
            SendDlgItemMessageA(hDlg, IDC_EDIT_SPEC, EM_GETLINE, 0, (LPARAM)strSpec);
            strSpec[length] = 0;

            int x = atoi(strX);
            int y = atoi(strY);
            int z = atoi(strZ);
            int specLevel = atoi(strSpec);
            
            if (specLevel < MIN_SPECULAR_LEVEL || specLevel > MAX_SPECULAR_LEVEL)
            {
                TCHAR message[100];
                sprintf(message, "The specular level should be from %d to %d", 
                    MIN_SPECULAR_LEVEL, MAX_SPECULAR_LEVEL);
                MessageBox(hDlg, message, "Waring", MB_OK);
                return TRUE;
            }

            mesh = new MeshNormals();
            try {
                mesh->loadFromFile(strPath);
                mesh->setup();
                mesh->specLevel = specLevel;
            }
            catch (const char* message)
            {
                delete mesh;
                MessageBox(hDlg, "Please, check the file", "Error", MB_OK);
                return TRUE;
            }

            mesh->position = glm::vec3(x, y, z);
            render->addMesh(mesh, strName);

            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;

    }
    case WM_CLOSE:
        EndDialog(hDlg, 0);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK DlgProcAddTexture(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR strName[20];
    TCHAR strPath[100];
    char szFile[100];
    DWORD length;
    Texture* texture;

    OPENFILENAMEA ofn;
    switch (message)
    {
    case WM_COMMAND:
    {
        switch (wParam)
        {
        case IDC_BUTTON_SELECTPATH:
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hDlg;
            ofn.lpstrFile = szFile;
            // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
            // use the contents of szFile to initialize itself.
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileNameA(&ofn) == TRUE)
            {
                SendDlgItemMessageA(hDlg, IDC_EDIT_PATH, WM_SETTEXT, 0, (LPARAM)ofn.lpstrFile);
            }
            break;
        case IDOK:
            length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_NAME, EM_LINELENGTH, 0, 0);
            *((LPWORD)strName) = length;
            SendDlgItemMessageA(hDlg, IDC_EDIT_NAME, EM_GETLINE, 0, (LPARAM)strName);
            strName[length] = 0;

            length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_PATH, EM_LINELENGTH, 0, 0);
            *((LPWORD)strPath) = length;
            SendDlgItemMessageA(hDlg, IDC_EDIT_PATH, EM_GETLINE, 0, (LPARAM)strPath);
            strPath[length] = 0;

            texture = new Texture(strPath);
            if (!render->addTexture(*texture, strName))
            {
                MessageBoxA(hDlg, "Error", "Please, select another name", MB_OK);
            }

            EndDialog(hDlg, 0);
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;

    }
    case WM_CLOSE:
        EndDialog(hDlg, 0);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK DlgProcChangeMesh(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR strX[10];
    TCHAR strY[10];
    TCHAR strZ[10];
    TCHAR strName[20];

    WORD length;
    char szFile[260];
    int meshIndex;

    switch (message)
    {
    case WM_INITDIALOG:
    {
        std::vector<std::string> names = render->getAllMeshesNames();
        for (int i = 0; i < names.size(); i++)
        {
            SendDlgItemMessageA(hDlg, IDC_COMBO_NAMES, CB_ADDSTRING, 0, (LPARAM)names[i].c_str());
        }

        return TRUE;
    }
    case WM_COMMAND:
    {
        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            int index = SendDlgItemMessageA(hDlg, IDC_COMBO_NAMES, CB_GETCURSEL, 0, 0);

            SendDlgItemMessageA(hDlg, IDC_COMBO_NAMES, CB_GETLBTEXT, index, (LPARAM)strName);
            meshIndex = render->getMeshIndexByName(strName);
            Mesh mesh = render->getMeshInfo(meshIndex);
            
            _itoa(mesh.position.x, strX, 10);
            _itoa(mesh.position.y, strY, 10);
            _itoa(mesh.position.z, strZ, 10);

            SendDlgItemMessageA(hDlg, IDC_EDIT_X, WM_SETTEXT, 0, (LPARAM)strX);
            SendDlgItemMessageA(hDlg, IDC_EDIT_Y, WM_SETTEXT, 0, (LPARAM)strY);
            SendDlgItemMessageA(hDlg, IDC_EDIT_Z, WM_SETTEXT, 0, (LPARAM)strZ);
            
        }
        switch (wParam)
        {

        case IDOK:
            length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_X, EM_LINELENGTH, 0, 0);
            length = length > 1 ? length : 2;
            *((LPWORD)strX) = length;
            SendDlgItemMessageA(hDlg, IDC_EDIT_X, EM_GETLINE, 0, (LPARAM)strX);
            strX[length] = 0;

            length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_Y, EM_LINELENGTH, 0, 0);
            length = length > 1 ? length : 2;
            *((LPWORD)strY) = length;
            SendDlgItemMessageA(hDlg, IDC_EDIT_Y, EM_GETLINE, 0, (LPARAM)strY);
            strY[length] = 0;

            length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_Z, EM_LINELENGTH, 0, 0);
            length = length > 1 ? length : 2;
            *((LPWORD)strZ) = length;
            SendDlgItemMessageA(hDlg, IDC_EDIT_Z, EM_GETLINE, 0, (LPARAM)strZ);
            strZ[length] = 0;

            int index = SendDlgItemMessageA(hDlg, IDC_COMBO_NAMES, CB_GETCURSEL, 0, 0);

            SendDlgItemMessageA(hDlg, IDC_COMBO_NAMES, CB_GETLBTEXT, index, (LPARAM)strName);

            int x = atoi(strX);
            int y = atoi(strY); 
            int z = atoi(strZ);

            meshIndex = render->getMeshIndexByName(strName);
            if (meshIndex != -1)
            {
                render->changeMeshPosition(meshIndex, glm::vec3(x, y, z));
            }
            else
            {
                MessageBoxA(hDlg, "Mesh not loaded", "Info", MB_OK);
            }

            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;

    }
    case WM_CLOSE:
        EndDialog(hDlg, 0);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK DlgProcAddTexturedMesh(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR strName[20];
    TCHAR strPath[100];
    TCHAR strTexture[20];

    TCHAR strX[10];
    TCHAR strY[10];
    TCHAR strZ[10];

    char szFile[100];
    DWORD length;
    Mesh* mesh;
    int textureIndex;
    bool isIncludeNormals;
    int index;

    OPENFILENAMEA ofn;
    switch (message)
    {
    case WM_INITDIALOG:
    {
        std::vector<std::string> names = render->getAllTexturesNames();
        for (int i = 0; i < names.size(); i++)
        {
            SendDlgItemMessageA(hDlg, IDC_COMBO_TEXTURES, CB_ADDSTRING, 0, (LPARAM)names[i].c_str());
        }

        return TRUE;
    }
    case WM_COMMAND:
    {
        switch (wParam)
        {
        case IDC_BUTTON_SELECTPATH:
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hDlg;
            ofn.lpstrFile = szFile;
            // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
            // use the contents of szFile to initialize itself.
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileNameA(&ofn) == TRUE)
            {
                SendDlgItemMessageA(hDlg, IDC_EDIT_PATH, WM_SETTEXT, 0, (LPARAM)ofn.lpstrFile);
            }
            break;
        case IDOK:
            length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_NAME, EM_LINELENGTH, 0, 0);
            *((LPWORD)strName) = length;
            SendDlgItemMessageA(hDlg, IDC_EDIT_NAME, EM_GETLINE, 0, (LPARAM)strName);
            strName[length] = 0;

            length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_PATH, EM_LINELENGTH, 0, 0);
            *((LPWORD)strPath) = length;
            SendDlgItemMessageA(hDlg, IDC_EDIT_PATH, EM_GETLINE, 0, (LPARAM)strPath);
            strPath[length] = 0;

            index = SendDlgItemMessageA(hDlg, IDC_COMBO_TEXTURES, CB_GETCURSEL, 0, 0);
            SendDlgItemMessageA(hDlg, IDC_COMBO_TEXTURES, CB_GETLBTEXT, index, (LPARAM)strTexture);

            DWORD includeNormals;
            includeNormals = SendDlgItemMessageA(hDlg, IDC_CHECK_NORMALS, BM_GETCHECK, 0, 0);
            if (includeNormals == BST_CHECKED)
            {
                isIncludeNormals = true;
            }
            else
            {
                isIncludeNormals = false;
            }
            
            if (isIncludeNormals)
            {
                mesh = new MeshNormalsTextures();
            }
            else
            {
                mesh = new MeshTextured();
            }

            try
            {
                mesh->loadFromFile(strPath);
                mesh->setup();
            }
            catch (const char* message)
            {
                delete mesh;
                MessageBox(hDlg, "Please, check the file", "Error", MB_OK);
                return TRUE;
            }
            textureIndex = render->getTextureIndexByName(strTexture);
            ((MeshTextured*)mesh)->textureId = render->getTextureIdByIndex(textureIndex);
            if (render->addMesh(mesh, strName) != TRUE)
            {
                MessageBoxA(hDlg, "Error", "Please, select another name", MB_OK);
            }

            EndDialog(hDlg, 0);
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;

    }
    case WM_CLOSE:
        EndDialog(hDlg, 0);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK DlgProcChangeLamp(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR strX[10];
    TCHAR strY[10];
    TCHAR strZ[10];
    TCHAR strName[20];

    static  DWORD rgbCurrent = 0xFF0000;

    WORD length;
    char szFile[260];
    int lampIndex;

    switch (message)
    {
    case WM_INITDIALOG:
    {
        
        for (int i = 0; i < render->lamps.size(); i++)
        {
            _itoa(i, strName, 10);
            SendDlgItemMessageA(hDlg, IDC_COMBO_NAMES, CB_ADDSTRING, 0, (LPARAM)strName);
        }

        return TRUE;
    }
    case WM_COMMAND:
    {
        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            int index = SendDlgItemMessageA(hDlg, IDC_COMBO_NAMES, CB_GETCURSEL, 0, 0);

            SendDlgItemMessageA(hDlg, IDC_COMBO_NAMES, CB_GETLBTEXT, index, (LPARAM)strName);
            lampIndex = atoi(strName);
            Ball* lamp = render->lamps[lampIndex];

            _itoa(lamp->position.x, strX, 10);
            _itoa(lamp->position.y, strY, 10);
            _itoa(lamp->position.z, strZ, 10);

            glm::vec3 color = lamp->getColor();
            rgbCurrent = color.r * 255 + color.g * 255 * 256 + color.b * 255 * 256 * 256;
            HDC hdc = GetDC(hDlg);
            LOGBRUSH logBrush;
            logBrush.lbColor = rgbCurrent;
            logBrush.lbStyle = BS_SOLID;
            HBRUSH brush = CreateBrushIndirect(&logBrush);
            SelectObject(hdc, brush);
            RECT rect{ 250, 70, 270, 90 };
            FillRect(hdc, &rect, brush);
            DeleteObject(brush);
            DeleteDC(hdc);

            SendDlgItemMessageA(hDlg, IDC_EDIT_X, WM_SETTEXT, 0, (LPARAM)strX);
            SendDlgItemMessageA(hDlg, IDC_EDIT_Y, WM_SETTEXT, 0, (LPARAM)strY);
            SendDlgItemMessageA(hDlg, IDC_EDIT_Z, WM_SETTEXT, 0, (LPARAM)strZ);

        }
        switch (wParam)
        {

        case IDOK:
        {
            length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_X, EM_LINELENGTH, 0, 0);
            length = length > 1 ? length : 2;
            *((LPWORD)strX) = length;
            SendDlgItemMessageA(hDlg, IDC_EDIT_X, EM_GETLINE, 0, (LPARAM)strX);
            strX[length] = 0;

            length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_Y, EM_LINELENGTH, 0, 0);
            length = length > 1 ? length : 2;
            *((LPWORD)strY) = length;
            SendDlgItemMessageA(hDlg, IDC_EDIT_Y, EM_GETLINE, 0, (LPARAM)strY);
            strY[length] = 0;

            length = (WORD)SendDlgItemMessageA(hDlg, IDC_EDIT_Z, EM_LINELENGTH, 0, 0);
            length = length > 1 ? length : 2;
            *((LPWORD)strZ) = length;
            SendDlgItemMessageA(hDlg, IDC_EDIT_Z, EM_GETLINE, 0, (LPARAM)strZ);
            strZ[length] = 0;

            int index = SendDlgItemMessageA(hDlg, IDC_COMBO_NAMES, CB_GETCURSEL, 0, 0);

            SendDlgItemMessageA(hDlg, IDC_COMBO_NAMES, CB_GETLBTEXT, index, (LPARAM)strName);

            int x = atoi(strX);
            int y = atoi(strY);
            int z = atoi(strZ);

            lampIndex = atoi(strName);
            Ball* lamp = render->lamps[lampIndex];
            lamp->position = glm::vec3(x, y, z);
            float red = rgbCurrent % 256 / 255;
            float green = (rgbCurrent >> 8) % 256 / 255;
            float blue = (rgbCurrent >> 16) % 256 / 255;
            lamp->setColor(glm::vec3(red, green, blue));

            EndDialog(hDlg, 0);
        }
        return TRUE;
        case IDC_BUTTON_CHANGECOLOR:
            {
            CHOOSECOLOR cc;
            static COLORREF customColors[16];
            ZeroMemory(&cc, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hDlg;
            cc.Flags = CC_FULLOPEN;
            cc.lpCustColors = (LPDWORD)customColors;
            cc.rgbResult = rgbCurrent;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;

            if (ChooseColor(&cc) == TRUE)
            {
                rgbCurrent = cc.rgbResult;
                char strColor[20];
                _itoa(rgbCurrent, strColor, 16);
                SendDlgItemMessageA(hDlg, IDC_EDIT_COLOR, WM_SETTEXT, 0, (LPARAM)strColor);
                HDC hdc = GetDC(hDlg);
                LOGBRUSH logBrush;
                logBrush.lbColor = cc.rgbResult;
                logBrush.lbStyle = BS_SOLID;
                HBRUSH brush = CreateBrushIndirect(&logBrush);
                SelectObject(hdc, brush);
                RECT rect{ 250, 70, 270, 90 };
                FillRect(hdc, &rect, brush);
                DeleteObject(brush);
                DeleteDC(hdc);
            }
            return TRUE;
                return TRUE;
            }
        }
        break;

    }
    case WM_CLOSE:
        EndDialog(hDlg, 0);
        break;
    }
    return FALSE;
}

void processExportBmp(HWND hWnd)
{
    int width = WIDTH, height = HEIGHT - 100;
    BYTE* pixels = new BYTE[3 * WIDTH * HEIGHT];

    glReadPixels(0, 0, WIDTH, HEIGHT, GL_BGR, GL_UNSIGNED_BYTE, pixels);

    FILE* f;
    OPENFILENAMEA ofn;
    TCHAR szFile[100];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "BMP\0*.bmp\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetSaveFileName(&ofn) == TRUE)
    {
        FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, width, height, 3 * width, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);
        FreeImage_Save(FIF_BMP, image, ofn.lpstrFile, 0);
        FreeImage_Unload(image);

    }

    delete[] pixels;
}

void changeModeFromGroup(int newRenderMode)
{
    int mask = render->RENDER_MULTIPLEPHONG | render->RENDER_PHONG 
        | render->RENDER_SHADOW | render->RENDER_REFLECTION | render->RENDER_SIMPLE;
    mask = ~mask;  
    render->RenderMode &=  mask;
    render->RenderMode |= newRenderMode;
}

void changeCheckboxMenu(int IDM)
{
    for (int i = 0; i < groupSize; i++)
    {
        CheckMenuItem(hModeSubmenu, renderModesGroup[i], MF_UNCHECKED);
    }

    CheckMenuItem(hModeSubmenu, IDM, MF_CHECKED);
}

void changeSingleCheckboxMenu(int IDM)
{
    DWORD dwModeMenu = GetMenuState(hModeSubmenu, IDM, MF_BYCOMMAND);
    if (dwModeMenu & MF_CHECKED)
    {
        CheckMenuItem(hModeSubmenu, IDM, MF_UNCHECKED);
    }
    else
    {
        CheckMenuItem(hModeSubmenu, IDM, MF_CHECKED);
    }
}

void onCommand(HWND hWnd, WPARAM wParam)
{
    DWORD dwModeMenu;
    switch (wParam)
    {
    case IDM_SHADOW:
        changeModeFromGroup(render->RENDER_SHADOW);
        changeCheckboxMenu(IDM_SHADOW);
        break;
    case IDM_PHONG:
        changeModeFromGroup(render->RENDER_PHONG);
        changeCheckboxMenu(IDM_PHONG);
        break;
    case IDM_MULTIPLEPHONG:
        changeModeFromGroup(render->RENDER_MULTIPLEPHONG);
        changeCheckboxMenu(IDM_MULTIPLEPHONG);
        break;
    case IDM_SIMPLE:
        changeModeFromGroup(render->RENDER_SIMPLE);
        changeCheckboxMenu(IDM_SIMPLE);
        break;
    case IDM_SKYBOX:
        changeSingleCheckboxMenu(IDM_SKYBOX);
        render->RenderMode = render->RenderMode ^ render->RENDER_SKYBOX;
        break;
    case IDM_LIGHTS_SOURCES:
        changeSingleCheckboxMenu(IDM_LIGHTS_SOURCES);
        render->RenderMode = render->RenderMode ^ render->RENDER_LIGHTS;
        break;
    case IDM_TEXTURED:
        changeSingleCheckboxMenu(IDM_TEXTURED);
        render->RenderMode ^= render->RENDER_TEXTURED;
        break;
    case IDM_REFLECTION:
        changeModeFromGroup(render->RENDER_REFLECTION);
        changeCheckboxMenu(IDM_REFLECTION);
        break;
    case IDM_ADDLAMP:
        DialogBoxParamA(NULL, MAKEINTRESOURCEA(IDD_DIALOG_ADDLAMP), hWnd, DlgProcAddLamp, 0);
        break;
    case IDM_LOADMESH:
        DialogBoxParamA(NULL, MAKEINTRESOURCEA(IDD_DIALOG_ADDMESH), hWnd, DlgProcAddMesh, 0);
        break;
    case IDM_EXPORT_BMP:
        processExportBmp(hWnd);
        break;
    case IDM_LOADTEXTURE:
        DialogBoxParamA(NULL, MAKEINTRESOURCEA(IDD_DIALOG_ADDTEXTURE), hWnd, DlgProcAddTexture, 0);
        break;
    case IDM_CHANGE_MESH:
        DialogBoxParamA(NULL, MAKEINTRESOURCEA(IDD_DIALOG_CHANGEMESH), hWnd, DlgProcChangeMesh, 0);
        break;
    case IDM_ADDTEXTUREDMESH:
        DialogBoxParamA(NULL, MAKEINTRESOURCEA(IDD_DIALOG_ADDTEXTUREDMESH), hWnd, DlgProcAddTexturedMesh, 0);
        break;
    case IDM_CHANGE_LAMP:
        DialogBoxParamA(NULL, MAKEINTRESOURCEA(IDD_DIALOG_CHENGELAMP), hWnd, DlgProcChangeLamp, 0);
        break;
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    switch (message)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    case WM_SIZE:
    {
        RECT rect;
        GetClientRect(hWnd, &rect);
        InvalidateRect(hWnd, &rect, TRUE);
        break;
    }
    case WM_KEYDOWN:
    {
        keys[wParam] = true;
        break;
    }
    case WM_KEYUP:
    {
        keys[wParam] = false;
        break;
    }
    case WM_COMMAND:
    {
        onCommand(hWnd, wParam);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    return 0;
}

void updateMovement(float time)
{
    time *= 0.01;
    if (keys[KEY_A])
    {
        alpha += 0.001;
        render->camera->moveLeft(time);
    }
    if (keys[KEY_D])
    {
        alpha -= 0.001;
        render->camera->moveRight(time);
    }
    if (keys[KEY_W])
    {
        render->camera->moveFront(time);
    }
    if (keys[KEY_S])
    {
        render->camera->moveBack(time);
    }
    if (keys[VK_ESCAPE])
    {
        isContinue = false;
    }
    time *= 10;
    if (keys[VK_UP])
    {
        render->camera->rotate(0, time);
    }
    if (keys[VK_DOWN])
    {
        render->camera->rotate(0, -time);
    }
    if (keys[VK_LEFT])
    {
        render->camera->rotate(-time, 0);
    }
    if (keys[VK_RIGHT])
    {
        render->camera->rotate(time, 0);
    }
}