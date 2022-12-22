// #define EMBED
#define JNI
// #define CONSOLE
#define DOWNLOAD

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")

#ifdef DOWNLOAD
#pragma comment(lib, "urlmon.lib")
#endif

#include "framework.h"
#include "launcher.h"

#ifdef JNI
typedef /*_JNI_IMPORT_OR_EXPORT_*/ jint(JNICALL* JNI_CreateJavaVM_func)(JavaVM** pvm, void** penv, void* args);
#endif

#define MAX_LOADSTRING 100

#define width 768
#define height 432

#define BTN_CLOSE  100
#define BTN_PLAY 101
#ifdef DOWNLOAD
#define BTN_INSTALL 102
#define BTN_BROWSEFOLDER 103
#endif
#define INPUT_USERNAME 104
#ifdef CONSOLE
#define CHECKBOX_CONSOLE 105
#endif
#define TRAY_ICON_ID 106
#define STATIC_TITLE 107

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

HWND hwndUsernameInput;
#ifdef CONSOLE
HWND hwndConsoleCheckbox;
#endif
HWND hwndPlayButton;
HWND hwndLabel;
HWND hwndMain;
HWND hwndPB;
HWND hwndMinecraft = NULL;
HWND hwndCloseButton;

#ifdef DOWNLOAD
HWND hwndFolderInput = NULL;
HWND hwndTitle;
HWND hWndVersionComboBox;
HWND hWndModComboBox;
HWND hwndBrowseFolderButton;

std::wstring oldVersion = L"";
#endif

WNDPROC oldEditProc;

BOOL schoolNetwork = false;

#define BASE_URL L"https://github.com/chtibizoux/minecraft-portable/releases/download/"
#define SCHOOLNETWORKDIRECTORY  L"P:\\Diffusion\\Travail\\à ranger merci (by poubelle)\\Jeux\\Minecraft fabric 1.19.2\\"
// #define SCHOOLNETWORKDIRECTORY = L"D:\\Documents\\.minecraft\\"

std::wstring minecraftDir = L"";
std::wstring dataDir = L"";

HCURSOR buttonCursor = LoadCursor(NULL, IDC_HAND);

#define WMAPP_NOTIFYCALLBACK (WM_APP + 1)
#ifdef DOWNLOAD
#define WMAPP_DOWNLOADEND (WM_APP + 2)
#endif

ATOM                 MyRegisterClass(HINSTANCE);
BOOL                 InitInstance(HINSTANCE, int);
BOOL                 AddNotificationIcon(HWND);
void                 ShowContextMenu(HWND, POINT);
#if defined(JNI) || defined(EMBED)
void                 _append_exception_trace_messages(JNIEnv&, std::string&, jthrowable, jmethodID, jmethodID, jmethodID, jmethodID);
#endif
void CALLBACK        HandleWinEvent(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD);
DWORD WINAPI         WaitMinecraft(LPVOID);
void                 StartMinecraft(HWND);
void                 InitPlayUI(HWND);
#ifdef DOWNLOAD
DWORD WINAPI         WaitDownload(LPVOID);
void                 InitInstallUI(HWND);
static int CALLBACK  BrowseFolderCallback(HWND, UINT, LPARAM, LPARAM);
std::wstring         BrowseFolder(std::wstring);
void                 InitConboBox(HWND, TCHAR[][MAX_PATH], int);
#endif
void                 ChooseInstall(HWND, BOOL);
LRESULT CALLBACK     subEditProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK     WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL                 IsFile(LPCWSTR);
BOOL                 IsDirectory(LPCWSTR);
BOOL                 IsFile(std::wstring);
BOOL                 IsDirectory(std::wstring);
BOOL                 CanLaunchMinecraft(std::wstring = L"");

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    TCHAR directory[MAX_PATH] = L"";
    GetCurrentDirectory(MAX_PATH, directory);
    schoolNetwork = std::wstring(directory).rfind(L"P:\\Diffusion\\Travail", 0) == 0;
    // schoolNetwork = true;

    if (schoolNetwork)
    {
        TCHAR my_documents[MAX_PATH];
        HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

        if (result == S_OK)
        {
            dataDir = std::wstring(my_documents) + L"\\.minecraft";
        }
        else
        {
            // std::cout << "Error: " << result << "\n";
        }
    }

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_LAUNCHER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAUNCHER));

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = CreatePatternBrush(LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BACKGROUND)));
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_LAUNCHER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   int screenWidth = GetSystemMetrics(SM_CXSCREEN);
   int screenHeight = GetSystemMetrics(SM_CYSCREEN);

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_MAXIMIZE | WS_POPUP,
       (screenWidth - width) / 2, (screenHeight - height) / 2, width, height, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

BOOL AddNotificationIcon(HWND hWnd)
{
    NOTIFYICONDATA nid = { sizeof(nid) };

    nid.hWnd = hWnd;
    // add the icon, setting the icon, tooltip, and callback message.
    // the icon will be identified with the GUID
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;

    nid.hIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    // LoadIconMetric(hInst, MAKEINTRESOURCE(IDI_ICON), LIM_SMALL, &nid.hIcon);

    LoadString(hInst, IDS_TOOLTIP, nid.szTip, ARRAYSIZE(nid.szTip));
    nid.uID = TRAY_ICON_ID;
    Shell_NotifyIcon(NIM_ADD, &nid);

    // NOTIFYICON_VERSION_4 is prefered
    nid.uVersion = NOTIFYICON_VERSION_4;
    return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

void ShowContextMenu(HWND hWnd, POINT pt)
{
    HMENU hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDC_CONTEXTMENU));
    if (hMenu)
    {
        HMENU hSubMenu = GetSubMenu(hMenu, 0);
        if (hSubMenu)
        {
            // our window must be foreground before calling TrackPopupMenu or the menu will not disappear when the user clicks away
            SetForegroundWindow(hWnd);

            // respect menu drop alignment
            UINT uFlags = TPM_RIGHTBUTTON;
            if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
            {
                uFlags |= TPM_RIGHTALIGN;
            }
            else
            {
                uFlags |= TPM_LEFTALIGN;
            }

            TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, hWnd, NULL);
        }
        DestroyMenu(hMenu);
    }
}

void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hWnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    switch (event)
    {
    case EVENT_OBJECT_CREATE:
        if (hwndMinecraft == NULL)
        {
            TCHAR name[MAX_PATH] = L"";
            GetWindowText(hWnd, name, MAX_PATH);
            if (std::wstring(name).rfind(L"Minecraft", 0) == 0)
            // if (std::wstring(name) == L"Minecraft* 1.19.2")
            {
                hwndMinecraft = hWnd;
                ShowWindow(hwndMain, SW_HIDE);
                AddNotificationIcon(hwndMain);
            }
        }
        break;
    case EVENT_SYSTEM_MINIMIZESTART:
#ifdef CheckMinimizedWindow
        if (hWnd == hwndMinecraft)
        {
            ShowWindow(hwndMinecraft, SW_HIDE);
        }
#else
        ShowWindow(hwndMinecraft, SW_HIDE);
#endif
        break;
    }
}

#if defined(JNI)
typedef struct MyData {
    HWND hWnd;
    std::string username;
    std::wstring mod;
#ifdef CONSOLE
    BOOL console;
#endif
} MYDATA, * PMYDATA;

JNI_CreateJavaVM_func JNI_CreateJavaVM_ptr;
DWORD WINAPI WaitMinecraft(LPVOID lpParam)
{
    PMYDATA pDataArray = (PMYDATA)lpParam;
    JavaVM* jvm;
    JNIEnv* env;
    JavaVMInitArgs vm_args;

    // std::string currentDir(minecraftDir.begin(), minecraftDir.end());

    if (pDataArray->mod == L"forge")
    {
        JavaVMOption options[9];
        options[0].optionString = (char*)"-DmergeModules=jna-5.10.0.jar,jna-platform-5.10.0.jar";
        options[1].optionString = (char*)"-DlibraryDirectory=libraries";
        options[2].optionString = (char*)"--module-path=libraries\\cpw\\mods\\bootstraplauncher\\1.0.0\\bootstraplauncher-1.0.0.jar;libraries\\cpw\\mods\\securejarhandler\\1.0.3\\securejarhandler-1.0.3.jar;libraries\\org\\ow2\\asm\\asm-commons\\9.2\\asm-commons-9.2.jar;libraries\\org\\ow2\\asm\\asm-util\\9.2\\asm-util-9.2.jar;libraries\\org\\ow2\\asm\\asm-analysis\\9.2\\asm-analysis-9.2.jar;libraries\\org\\ow2\\asm\\asm-tree\\9.2\\asm-tree-9.2.jar;libraries\\org\\ow2\\asm\\asm\\9.2\\asm-9.2.jar";
        options[3].optionString = (char*)"--add-modules=ALL-MODULE-PATH";
        options[4].optionString = (char*)"--add-opens=java.base/java.util.jar=cpw.mods.securejarhandler";
        options[5].optionString = (char*)"--add-exports=java.base/sun.security.util=cpw.mods.securejarhandler";
        options[6].optionString = (char*)"--add-exports=jdk.naming.dns/com.sun.jndi.dns=java.naming";
        options[7].optionString = (char*)"-Djava.library.path=natives/1.18.2";
        options[8].optionString = (char*)"-Djava.class.path=libraries\\net\\minecraftforge\\fmlloader\\1.18.2-40.1.86\\fmlloader-1.18.2-40.1.86.jar;libraries\\io\\github\\zekerzhayard\\ForgeWrapper\\1.5.5\\ForgeWrapper-1.5.5.jar;libraries\\net\\minecraftforge\\accesstransformers\\8.0.4\\accesstransformers-8.0.4.jar;libraries\\net\\minecraftforge\\eventbus\\5.0.3\\eventbus-5.0.3.jar;libraries\\net\\minecraftforge\\forgespi\\4.0.15-4.x\\forgespi-4.0.15-4.x.jar;libraries\\net\\minecraftforge\\coremods\\5.0.1\\coremods-5.0.1.jar;libraries\\org\\antlr\\antlr4-runtime\\4.9.1\\antlr4-runtime-4.9.1.jar;libraries\\cpw\\mods\\modlauncher\\9.1.3\\modlauncher-9.1.3.jar;libraries\\net\\minecraftforge\\unsafe\\0.2.0\\unsafe-0.2.0.jar;libraries\\com\\electronwill\\night-config\\toml\\3.6.4\\toml-3.6.4.jar;libraries\\org\\apache\\maven\\maven-artifact\\3.6.3\\maven-artifact-3.6.3.jar;libraries\\com\\electronwill\\night-config\\core\\3.6.4\\core-3.6.4.jar;libraries\\net\\jodah\\typetools\\0.8.3\\typetools-0.8.3.jar;libraries\\net\\minecrell\\terminalconsoleappender\\1.2.0\\terminalconsoleappender-1.2.0.jar;libraries\\org\\jline\\jline-reader\\3.12.1\\jline-reader-3.12.1.jar;libraries\\org\\jline\\jline-terminal\\3.12.1\\jline-terminal-3.12.1.jar;libraries\\org\\spongepowered\\mixin\\0.8.5\\mixin-0.8.5.jar;libraries\\net\\minecraftforge\\JarJar\\0.3.0\\JarJar-0.3.0.jar;libraries\\org\\openjdk\\nashorn\\nashorn-core\\15.3\\nashorn-core-15.3.jar;libraries\\net\\sf\\jopt-simple\\jopt-simple\\5.0.4\\jopt-simple-5.0.4.jar;libraries\\commons-logging\\commons-logging\\1.2\\commons-logging-1.2.jar;libraries\\com\\mojang\\blocklist\\1.0.10\\blocklist-1.0.10.jar;libraries\\com\\mojang\\logging\\1.0.0\\logging-1.0.0.jar;libraries\\com\\mojang\\patchy\\2.2.10\\patchy-2.2.10.jar;libraries\\com\\github\\oshi\\oshi-core\\5.8.5\\oshi-core-5.8.5.jar;libraries\\net\\java\\dev\\jna\\jna\\5.10.0\\jna-5.10.0.jar;libraries\\org\\slf4j\\slf4j-api\\1.8.0-beta4\\slf4j-api-1.8.0-beta4.jar;libraries\\org\\apache\\logging\\log4j\\log4j-slf4j18-impl\\2.17.0\\log4j-slf4j18-impl-2.17.0.jar;libraries\\net\\java\\dev\\jna\\jna-platform\\5.10.0\\jna-platform-5.10.0.jar;libraries\\com\\mojang\\javabridge\\1.2.24\\javabridge-1.2.24.jar;libraries\\io\\netty\\netty-all\\4.1.68.Final\\netty-all-4.1.68.Final.jar;libraries\\com\\google\\guava\\failureaccess\\1.0.1\\failureaccess-1.0.1.jar;libraries\\com\\google\\guava\\guava\\31.0.1-jre\\guava-31.0.1-jre.jar;libraries\\org\\apache\\commons\\commons-lang3\\3.12.0\\commons-lang3-3.12.0.jar;libraries\\commons-io\\commons-io\\2.11.0\\commons-io-2.11.0.jar;libraries\\commons-codec\\commons-codec\\1.15\\commons-codec-1.15.jar;libraries\\com\\mojang\\brigadier\\1.0.18\\brigadier-1.0.18.jar;libraries\\com\\mojang\\datafixerupper\\4.1.27\\datafixerupper-4.1.27.jar;libraries\\com\\google\\code\\gson\\gson\\2.8.9\\gson-2.8.9.jar;libraries\\com\\mojang\\authlib\\3.3.39\\authlib-3.3.39.jar;libraries\\org\\apache\\commons\\commons-compress\\1.21\\commons-compress-1.21.jar;libraries\\org\\apache\\httpcomponents\\httpclient\\4.5.13\\httpclient-4.5.13.jar;libraries\\com\\ibm\\icu\\icu4j\\70.1\\icu4j-70.1.jar;libraries\\org\\apache\\httpcomponents\\httpcore\\4.4.14\\httpcore-4.4.14.jar;libraries\\org\\apache\\logging\\log4j\\log4j-api\\2.17.0\\log4j-api-2.17.0.jar;libraries\\org\\apache\\logging\\log4j\\log4j-core\\2.17.0\\log4j-core-2.17.0.jar;libraries\\org\\lwjgl\\lwjgl\\3.2.2\\lwjgl-3.2.2.jar;libraries\\org\\lwjgl\\lwjgl-jemalloc\\3.2.2\\lwjgl-jemalloc-3.2.2.jar;libraries\\org\\lwjgl\\lwjgl-openal\\3.2.2\\lwjgl-openal-3.2.2.jar;libraries\\org\\lwjgl\\lwjgl-opengl\\3.2.2\\lwjgl-opengl-3.2.2.jar;libraries\\org\\lwjgl\\lwjgl-glfw\\3.2.2\\lwjgl-glfw-3.2.2.jar;libraries\\org\\lwjgl\\lwjgl-stb\\3.2.2\\lwjgl-stb-3.2.2.jar;libraries\\org\\lwjgl\\lwjgl-tinyfd\\3.2.2\\lwjgl-tinyfd-3.2.2.jar;libraries\\com\\mojang\\text2speech\\1.12.4\\text2speech-1.12.4.jar;libraries\\it\\unimi\\dsi\\fastutil\\8.5.6\\fastutil-8.5.6.jar";
        vm_args.nOptions = 9;
        vm_args.options = options;
    }
    else
    {
        JavaVMOption options[1];
        if (pDataArray->mod == L"fabric")
        {
            options[0].optionString = (char*)"-Djava.class.path=fabric.jar;minecraft.jar";
        }
        if (pDataArray->mod == L"remmaped")
        {
            options[0].optionString = (char*)"-Djava.class.path=fabric.jar;.fabric\\remappedJars\\minecraft-1.19.2-0.14.10\\client-intermediary.jar";
        }
        if (pDataArray->mod == L"vanilla")
        {
            options[0].optionString = (char*)"-Djava.class.path=minecraft.jar";
        }
        vm_args.nOptions = 1;
        vm_args.options = options;
    }
    //options[1].optionString = (char*)"-verbose:class,gc,jni";
    //options[2].optionString = (char*)"-Xcheck:jni";

    // options[1].optionString = (char*)"-Xms1m";
    // options[2].optionString = (char*)"-Xmx1g";
    vm_args.version = JNI_VERSION_19;
    vm_args.ignoreUnrecognized = true;
    jint errorCode = JNI_CreateJavaVM_ptr(&jvm, (void**)&env, &vm_args);
    if (errorCode == 0)
    {
        jclass cls = env->FindClass((pDataArray->mod == L"fabric" || pDataArray->mod == L"remmaped") ? "net/fabricmc/loader/impl/launch/knot/KnotClient" : (pDataArray->mod == L"forge" ? "cpw/mods/bootstraplauncher/BootstrapLauncher" : "net/minecraft/client/main/Main"));
        if (cls != 0)
        {
            jmethodID meth = env->GetStaticMethodID(cls, "main", "([Ljava/lang/String;)V");
            int length = 6;
            if (schoolNetwork) length += 2;
            if (pDataArray->mod == L"forge") length += 8;
            jobjectArray args = env->NewObjectArray(length, env->FindClass("java/lang/String"), env->NewStringUTF(""));
            env->SetObjectArrayElement(args, 0, env->NewStringUTF("--username"));
            env->SetObjectArrayElement(args, 1, env->NewStringUTF(pDataArray->username.c_str()));
            env->SetObjectArrayElement(args, 2, env->NewStringUTF("--version"));
            env->SetObjectArrayElement(args, 3, env->NewStringUTF("1.19.2"));
            env->SetObjectArrayElement(args, 4, env->NewStringUTF("--accessToken"));
            env->SetObjectArrayElement(args, 5, env->NewStringUTF("00000000-0000-0000-0000-000000000000"));
            if (schoolNetwork)
            {
                env->SetObjectArrayElement(args, 6, env->NewStringUTF("--gameDir"));
                env->SetObjectArrayElement(args, 7, env->NewStringUTF(std::string(dataDir.begin(), dataDir.end()).c_str()));
            }
            if (pDataArray->mod == L"forge")
            {
                env->SetObjectArrayElement(args, length - 8, env->NewStringUTF("--launchTarget"));
                env->SetObjectArrayElement(args, length - 7, env->NewStringUTF("forgeclient"));
                env->SetObjectArrayElement(args, length - 6, env->NewStringUTF("--fml.forgeVersion"));
                env->SetObjectArrayElement(args, length - 5, env->NewStringUTF("40.1.86"));
                env->SetObjectArrayElement(args, length - 4, env->NewStringUTF("--fml.mcVersion"));
                env->SetObjectArrayElement(args, length - 3, env->NewStringUTF("1.18.2"));
                env->SetObjectArrayElement(args, length - 2, env->NewStringUTF("--fml.mcpVersion"));
                env->SetObjectArrayElement(args, length - 1, env->NewStringUTF("20220404.173914"));
            }

            env->CallStaticVoidMethod(cls, meth, args);

            jthrowable exception = env->ExceptionOccurred();
            env->ExceptionClear();
            jclass throwable_class = env->FindClass("java/lang/Throwable");
            jmethodID mid_throwable_getCause = env->GetMethodID(throwable_class, "getCause", "()Ljava/lang/Throwable;");
            jmethodID mid_throwable_getStackTrace = env->GetMethodID(throwable_class, "getStackTrace", "()[Ljava/lang/StackTraceElement;");
            jmethodID mid_throwable_toString = env->GetMethodID(throwable_class, "toString", "()Ljava/lang/String;");
            jclass frame_class = env->FindClass("java/lang/StackTraceElement");
            jmethodID mid_frame_toString = env->GetMethodID(frame_class, "toString", "()Ljava/lang/String;");
            std::string error_msg;
            _append_exception_trace_messages(*env, error_msg, exception, mid_throwable_getCause, mid_throwable_getStackTrace, mid_throwable_toString, mid_frame_toString);
            if (!error_msg.empty())
            {
                MessageBoxA(pDataArray->hWnd, error_msg.c_str(), NULL, NULL);
            }
        }
        else
        {
            MessageBox(pDataArray->hWnd, L"ERROR: Main class not found !", NULL, NULL);
        }
    }
    else
    {
        switch (errorCode)
        {
        case JNI_ERR:
            MessageBox(pDataArray->hWnd, L"JNI_CreateJavaVM ERROR: unknown error", NULL, NULL);
            break;
        case JNI_EDETACHED:
            MessageBox(pDataArray->hWnd, L"JNI_CreateJavaVM ERROR: thread detached from the VM", NULL, NULL);
            break;
        case JNI_EVERSION:
            MessageBox(pDataArray->hWnd, L"JNI_CreateJavaVM ERROR: JNI version error", NULL, NULL);
            break;
        case JNI_ENOMEM:
            MessageBox(pDataArray->hWnd, L"JNI_CreateJavaVM ERROR: not enough memory", NULL, NULL);
            break;
        case JNI_EEXIST:
            MessageBox(pDataArray->hWnd, L"JNI_CreateJavaVM ERROR: VM already created", NULL, NULL);
            break;
        case JNI_EINVAL:
            MessageBox(pDataArray->hWnd, L"JNI_CreateJavaVM ERROR: invalid arguments", NULL, NULL);
            break;
        default:
            MessageBox(pDataArray->hWnd, (L"ERROR: JNI_CreateJavaVM Code: " + std::to_wstring(errorCode)).c_str(), NULL, NULL);
            break;
        }
    }
    return 0;
}

void _append_exception_trace_messages(
    JNIEnv& a_jni_env,
    std::string& a_error_msg,
    jthrowable   a_exception,
    jmethodID    a_mid_throwable_getCause,
    jmethodID    a_mid_throwable_getStackTrace,
    jmethodID    a_mid_throwable_toString,
    jmethodID    a_mid_frame_toString)
{
    jobjectArray frames = (jobjectArray)a_jni_env.CallObjectMethod(a_exception, a_mid_throwable_getStackTrace);
    jsize frames_length = a_jni_env.GetArrayLength(frames);

    if (0 != frames)
    {
        jstring msg_obj = (jstring)a_jni_env.CallObjectMethod(a_exception, a_mid_throwable_toString);
        const char* msg_str = a_jni_env.GetStringUTFChars(msg_obj, 0);
        if (!a_error_msg.empty())
        {
            a_error_msg += "\nCaused by: ";
            a_error_msg += msg_str;
        }
        else
        {
            a_error_msg = msg_str;
        }

        a_jni_env.ReleaseStringUTFChars(msg_obj, msg_str);
        a_jni_env.DeleteLocalRef(msg_obj);
    }

    if (frames_length > 0)
    {
        jsize i = 0;
        for (i = 0; i < frames_length; i++)
        {
            jobject frame = a_jni_env.GetObjectArrayElement(frames, i);
            jstring msg_obj = (jstring)a_jni_env.CallObjectMethod(frame, a_mid_frame_toString);

            const char* msg_str = a_jni_env.GetStringUTFChars(msg_obj, 0);

            a_error_msg += "\n    ";
            a_error_msg += msg_str;

            a_jni_env.ReleaseStringUTFChars(msg_obj, msg_str);
            a_jni_env.DeleteLocalRef(msg_obj);
            a_jni_env.DeleteLocalRef(frame);
        }
    }

    // If 'a_exception' has a cause then append the
    // stack trace messages from the cause.
    if (0 != frames)
    {
        jthrowable cause = (jthrowable)a_jni_env.CallObjectMethod(a_exception, a_mid_throwable_getCause);
        if (0 != cause)
        {
            _append_exception_trace_messages(a_jni_env, a_error_msg, cause, a_mid_throwable_getCause, a_mid_throwable_getStackTrace, a_mid_throwable_toString, a_mid_frame_toString);
        }
    }
}

void StartMinecraft(HWND hWnd)
{
    char username[MAX_PATH] = "";
    GetWindowTextA(hwndUsernameInput, username, MAX_PATH);
    if (!regex_match(username, std::regex("^[a-zA-Z0-9_]{2,16}$")))
    {
        MessageBox(hWnd, L"Nom d'utilisateur nom valide", L"Erreur Minecraft", MB_OK);
        return;
    }

    if (!minecraftDir.empty())
    {
        SetCurrentDirectory(minecraftDir.c_str());
    }

#ifdef CONSOLE
    BOOL console = IsDlgButtonChecked(hWnd, CHECKBOX_CONSOLE);
#endif

    DestroyWindow(hwndUsernameInput);
    DestroyWindow(hwndPlayButton);
#ifdef CONSOLE
    DestroyWindow(hwndConsoleCheckbox);
#endif
    DestroyWindow(hwndLabel);

    HWND hwndPB = CreateWindow(
        PROGRESS_CLASS,
        (LPTSTR)NULL,
        WS_CHILD | WS_VISIBLE | PBS_MARQUEE,
        width / 2 - 200,
        height / 2 - 15,
        400,
        30,
        hWnd,
        NULL,
        hInst,
        NULL);
    SendMessage(hwndPB, PBM_SETMARQUEE, true, 0);

    HMODULE jvm_dll = LoadLibrary(/*(minecraftDir + */L"jdk\\bin\\server\\jvm.dll"/*).c_str()*/);
    if (jvm_dll == NULL)
    {
        MessageBox(hWnd, L"Can't load jvm.dll", NULL, NULL);
        return;
    }

    JNI_CreateJavaVM_ptr = (JNI_CreateJavaVM_func)GetProcAddress(jvm_dll, "JNI_CreateJavaVM");
    if (JNI_CreateJavaVM_ptr == NULL)
    {
        MessageBox(hWnd, L"Can't load JNI_CreateJavaVM function", NULL, NULL);
        return;
    }

    SetWinEventHook(
        EVENT_SYSTEM_MINIMIZESTART, EVENT_OBJECT_DESTROY,
        NULL, HandleWinEvent,
        GetCurrentProcessId(), 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNTHREAD);

    PMYDATA pData = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(MYDATA));

    pData->hWnd = hWnd;
    pData->username = std::string(username);

    if (IsFile(/*minecraftDir + */L"fabric.jar"))
    {
        if (IsFile(/*minecraftDir + */L".fabric\\remappedJars\\minecraft-1.19.2-0.14.10\\client-intermediary.jar"))
        {
            pData->mod = std::wstring(L"remapped");
        }
        else
        {
            pData->mod = std::wstring(L"fabric");
        }
    }
    else if (IsDirectory(/*minecraftDir + */L"libraries"))
    {
        pData->mod = std::wstring(L"forge");
    }
    else
    {
        pData->mod = std::wstring(L"vanilla");
    }
#ifdef CONSOLE
    pData->console = console;
#endif

    DWORD threadId;
    HANDLE thread = CreateThread(
        NULL,
        0,
        WaitMinecraft,
        pData,
        0,
        &threadId);
    if (thread == NULL)
    {
        MessageBox(hWnd, L"Error when starting thread.", NULL, NULL);
    }
}
#elif defined(EMBED)
typedef struct MyData {
    HWND hWnd;
    std::string username;
    std::wstring mod;
#ifdef CONSOLE
    BOOL console;
#endif
} MYDATA, * PMYDATA;

DWORD WINAPI WaitMinecraft(LPVOID lpParam)
{
    PMYDATA pDataArray = (PMYDATA)lpParam;

    JavaVM* jvm;
    JNIEnv* env;
    JavaVMInitArgs vm_args;
    JavaVMOption options[1];
    if (pDataArray->fabric) {
        if (pDataArray->remmaped) {
            options[0].optionString = (char*)"-Djava.class.path=fabric.jar;.fabric\\remappedJars\\minecraft-1.19.2-0.14.10\\client-intermediary.jar";
        }
        else {
            options[0].optionString = (char*)"-Djava.class.path=fabric.jar;minecraft.jar";
        }
    }
    else
    {
        options[0].optionString = (char*)"-Djava.class.path=minecraft.jar";
    }
    //options[1].optionString = (char*)"-verbose:class,gc,jni";
    //options[2].optionString = (char*)"-Xcheck:jni";

    // options[1].optionString = (char*)"-Xms1m";
    // options[2].optionString = (char*)"-Xmx1g";
    vm_args.version = JNI_VERSION_19;
    vm_args.nOptions = 1;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = true;
    jint errorCode = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
    if (errorCode == 0) {
        jclass cls = env->FindClass(pDataArray->fabric ? "net/fabricmc/loader/impl/launch/knot/KnotClient" : "net/minecraft/client/main/Main");
        if (cls != 0) {
            jmethodID meth = env->GetStaticMethodID(cls, "main", "([Ljava/lang/String;)V");

            jobjectArray args = env->NewObjectArray(6, env->FindClass("java/lang/String"), env->NewStringUTF(""));
            env->SetObjectArrayElement(args, 0, env->NewStringUTF("--username"));
            env->SetObjectArrayElement(args, 1, env->NewStringUTF(pDataArray->username.c_str()));
            env->SetObjectArrayElement(args, 2, env->NewStringUTF("--version"));
            env->SetObjectArrayElement(args, 3, env->NewStringUTF("1.19.2"));
            env->SetObjectArrayElement(args, 4, env->NewStringUTF("--accessToken"));
            env->SetObjectArrayElement(args, 5, env->NewStringUTF("00000000-0000-0000-0000-000000000000"));

            env->CallStaticVoidMethod(cls, meth, args);

            jthrowable exception = env->ExceptionOccurred();
            env->ExceptionClear();
            jclass throwable_class = env->FindClass("java/lang/Throwable");
            jmethodID mid_throwable_getCause = env->GetMethodID(throwable_class, "getCause", "()Ljava/lang/Throwable;");
            jmethodID mid_throwable_getStackTrace = env->GetMethodID(throwable_class, "getStackTrace", "()[Ljava/lang/StackTraceElement;");
            jmethodID mid_throwable_toString = env->GetMethodID(throwable_class, "toString", "()Ljava/lang/String;");
            jclass frame_class = env->FindClass("java/lang/StackTraceElement");
            jmethodID mid_frame_toString = env->GetMethodID(frame_class, "toString", "()Ljava/lang/String;");
            std::string error_msg;
            _append_exception_trace_messages(*env, error_msg, exception, mid_throwable_getCause, mid_throwable_getStackTrace, mid_throwable_toString, mid_frame_toString);
            if (!error_msg.empty())
            {
                MessageBoxA(pDataArray->hWnd, error_msg.c_str(), NULL, NULL);
            }
        }
        else {
            MessageBox(pDataArray->hWnd, L"ERROR: Main class not found !", NULL, NULL);
        }
    }
    else
    {
        switch (errorCode)
        {
        case JNI_ERR:
            MessageBox(pDataArray->hWnd, L"JNI_CreateJavaVM ERROR: unknown error", NULL, NULL);
            break;
        case JNI_EDETACHED:
            MessageBox(pDataArray->hWnd, L"JNI_CreateJavaVM ERROR: thread detached from the VM", NULL, NULL);
            break;
        case JNI_EVERSION:
            MessageBox(pDataArray->hWnd, L"JNI_CreateJavaVM ERROR: JNI version error", NULL, NULL);
            break;
        case JNI_ENOMEM:
            MessageBox(pDataArray->hWnd, L"JNI_CreateJavaVM ERROR: not enough memory", NULL, NULL);
            break;
        case JNI_EEXIST:
            MessageBox(pDataArray->hWnd, L"JNI_CreateJavaVM ERROR: VM already created", NULL, NULL);
            break;
        case JNI_EINVAL:
            MessageBox(pDataArray->hWnd, L"JNI_CreateJavaVM ERROR: invalid arguments", NULL, NULL);
            break;
        default:
            MessageBox(pDataArray->hWnd, (L"ERROR: JNI_CreateJavaVM Code: " + std::to_wstring(errorCode)).c_str(), NULL, NULL);
            break;
        }
    }
    return 0;
}

void _append_exception_trace_messages(
    JNIEnv& a_jni_env,
    std::string& a_error_msg,
    jthrowable   a_exception,
    jmethodID    a_mid_throwable_getCause,
    jmethodID    a_mid_throwable_getStackTrace,
    jmethodID    a_mid_throwable_toString,
    jmethodID    a_mid_frame_toString)
{
    jobjectArray frames = (jobjectArray)a_jni_env.CallObjectMethod(a_exception, a_mid_throwable_getStackTrace);
    jsize frames_length = a_jni_env.GetArrayLength(frames);

    if (0 != frames)
    {
        jstring msg_obj = (jstring)a_jni_env.CallObjectMethod(a_exception, a_mid_throwable_toString);
        const char* msg_str = a_jni_env.GetStringUTFChars(msg_obj, 0);
        if (!a_error_msg.empty())
        {
            a_error_msg += "\nCaused by: ";
            a_error_msg += msg_str;
        }
        else
        {
            a_error_msg = msg_str;
        }

        a_jni_env.ReleaseStringUTFChars(msg_obj, msg_str);
        a_jni_env.DeleteLocalRef(msg_obj);
    }

    if (frames_length > 0)
    {
        jsize i = 0;
        for (i = 0; i < frames_length; i++)
        {
            jobject frame = a_jni_env.GetObjectArrayElement(frames, i);
            jstring msg_obj = (jstring)a_jni_env.CallObjectMethod(frame, a_mid_frame_toString);

            const char* msg_str = a_jni_env.GetStringUTFChars(msg_obj, 0);

            a_error_msg += "\n    ";
            a_error_msg += msg_str;

            a_jni_env.ReleaseStringUTFChars(msg_obj, msg_str);
            a_jni_env.DeleteLocalRef(msg_obj);
            a_jni_env.DeleteLocalRef(frame);
        }
    }

    // If 'a_exception' has a cause then append the
    // stack trace messages from the cause.
    if (0 != frames)
    {
        jthrowable cause = (jthrowable)a_jni_env.CallObjectMethod(a_exception, a_mid_throwable_getCause);
        if (0 != cause)
        {
            _append_exception_trace_messages(a_jni_env, a_error_msg, cause, a_mid_throwable_getCause, a_mid_throwable_getStackTrace, a_mid_throwable_toString, a_mid_frame_toString);
        }
    }
}

void StartMinecraft(HWND hWnd)
{
    char username[MAX_PATH] = "";
    GetWindowTextA(hwndUsernameInput, username, MAX_PATH);
    if (!regex_match(username, std::regex("^[a-zA-Z0-9_]{2,16}$")))
    {
        MessageBox(hWnd, L"Nom d'utilisateur nom valide", L"Erreur Minecraft", MB_OK);
        return;
    }

#ifdef CONSOLE
    BOOL console = IsDlgButtonChecked(hWnd, CHECKBOX_CONSOLE);

    DestroyWindow(hwndConsoleCheckbox);
#endif
    DestroyWindow(hwndUsernameInput);
    DestroyWindow(hwndPlayButton);
    DestroyWindow(hwndLabel);

    HWND hwndPB = CreateWindow(
        PROGRESS_CLASS,
        (LPTSTR)NULL,
        WS_CHILD | WS_VISIBLE | PBS_MARQUEE,
        width / 2 - 200,
        height / 2 - 15,
        400,
        30,
        hWnd,
        NULL,
        hInst,
        NULL);
    SendMessage(hwndPB, PBM_SETMARQUEE, true, 0);

    SetWinEventHook(
        EVENT_SYSTEM_MINIMIZESTART, EVENT_OBJECT_DESTROY,
        NULL,
        HandleWinEvent,
        GetCurrentProcessId(), 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNTHREAD);

    PMYDATA pData = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(MYDATA));

    pData->hWnd = hWnd;
    pData->username = std::string(username);
    pData->fabric = IsFile(/*(minecraftDir + */L"fabric.jar"/*).c_str()*/);
    pData->remmaped = IsFile(/*(minecraftDir + */L".fabric\\remappedJars\\minecraft-1.19.2-0.14.10\\client-intermediary.jar"/*).c_str()*/);
#ifdef CONSOLE
    pData->console = console;
#endif

    DWORD threadId;
    HANDLE thread = CreateThread(
        NULL,
        0,
        WaitMinecraft,
        pData,
        0,
        &threadId);
    if (thread == NULL)
    {
        MessageBox(hWnd, L"Error when starting thread.", NULL, NULL);
    }
}
#else
typedef struct MyData {
    PROCESS_INFORMATION pi;
    HWND hWnd;
} MYDATA, * PMYDATA;

DWORD WINAPI WaitMinecraft(LPVOID lpParam)
{
    PMYDATA pDataArray = (PMYDATA)lpParam;

    WaitForSingleObject(pDataArray->pi.hProcess, INFINITE);

    CloseHandle(pDataArray->pi.hProcess);
    CloseHandle(pDataArray->pi.hThread);

    SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);

    return 0;
}
void StartMinecraft(HWND hWnd)
{
    TCHAR username[MAX_PATH] = L"";
    GetWindowText(hwndUsernameInput, username, MAX_PATH);
    if (!regex_match(username, std::wregex(L"^[a-zA-Z0-9_]{2,16}$")))
    {
        MessageBox(hWnd, L"Nom d'utilisateur nom valide", L"Erreur Minecraft", MB_OK);
        return;
    }
    /*if (!minecraftDir.empty())
    {
        SetCurrentDirectory(minecraftDir.c_str());
    }*/
#ifdef CONSOLE
    BOOL console = IsDlgButtonChecked(hWnd, CHECKBOX_CONSOLE);
#endif
    std::wstring args = L"";
    if (IsFile(minecraftDir + L"fabric.jar"))
    {
        if (IsFile(minecraftDir + L".fabric\\remappedJars\\minecraft-1.19.2-0.14.10\\client-intermediary.jar"))
        {
            args += L" -cp fabric.jar;.fabric\\remappedJars\\minecraft-1.19.2-0.14.10\\client-intermediary.jar net.fabricmc.loader.impl.launch.knot.KnotClient";
        }
        else
        {
            args += L" -cp fabric.jar;minecraft.jar net.fabricmc.loader.impl.launch.knot.KnotClient";
        }
    }
    else if (IsDirectory(minecraftDir + L"libraries"))
    {
        args += L" -DmergeModules=jna-5.10.0.jar,jna-platform-5.10.0.jar -DlibraryDirectory=libraries -p libraries/cpw/mods/bootstraplauncher/1.0.0/bootstraplauncher-1.0.0.jar;libraries/cpw/mods/securejarhandler/1.0.3/securejarhandler-1.0.3.jar;libraries/org/ow2/asm/asm-commons/9.2/asm-commons-9.2.jar;libraries/org/ow2/asm/asm-util/9.2/asm-util-9.2.jar;libraries/org/ow2/asm/asm-analysis/9.2/asm-analysis-9.2.jar;libraries/org/ow2/asm/asm-tree/9.2/asm-tree-9.2.jar;libraries/org/ow2/asm/asm/9.2/asm-9.2.jar --add-modules ALL-MODULE-PATH --add-opens java.base/java.util.jar=cpw.mods.securejarhandler --add-exports java.base/sun.security.util=cpw.mods.securejarhandler --add-exports jdk.naming.dns/com.sun.jndi.dns=java.naming -Djava.library.path=natives/1.18.2 -XX:HeapDumpPath=MojangTricksIntelDriversForPerformance_javaw.exe_minecraft.exe.heapdump -cp libraries/net/minecraftforge/fmlloader/1.18.2-40.1.86/fmlloader-1.18.2-40.1.86.jar;libraries/io/github/zekerzhayard/ForgeWrapper/1.5.5/ForgeWrapper-1.5.5.jar;libraries/net/minecraftforge/accesstransformers/8.0.4/accesstransformers-8.0.4.jar;libraries/net/minecraftforge/eventbus/5.0.3/eventbus-5.0.3.jar;libraries/net/minecraftforge/forgespi/4.0.15-4.x/forgespi-4.0.15-4.x.jar;libraries/net/minecraftforge/coremods/5.0.1/coremods-5.0.1.jar;libraries/org/antlr/antlr4-runtime/4.9.1/antlr4-runtime-4.9.1.jar;libraries/cpw/mods/modlauncher/9.1.3/modlauncher-9.1.3.jar;libraries/net/minecraftforge/unsafe/0.2.0/unsafe-0.2.0.jar;libraries/com/electronwill/night-config/toml/3.6.4/toml-3.6.4.jar;libraries/org/apache/maven/maven-artifact/3.6.3/maven-artifact-3.6.3.jar;libraries/com/electronwill/night-config/core/3.6.4/core-3.6.4.jar;libraries/net/jodah/typetools/0.8.3/typetools-0.8.3.jar;libraries/net/minecrell/terminalconsoleappender/1.2.0/terminalconsoleappender-1.2.0.jar;libraries/org/jline/jline-reader/3.12.1/jline-reader-3.12.1.jar;libraries/org/jline/jline-terminal/3.12.1/jline-terminal-3.12.1.jar;libraries/org/spongepowered/mixin/0.8.5/mixin-0.8.5.jar;libraries/net/minecraftforge/JarJar/0.3.0/JarJar-0.3.0.jar;libraries/org/openjdk/nashorn/nashorn-core/15.3/nashorn-core-15.3.jar;libraries/net/sf/jopt-simple/jopt-simple/5.0.4/jopt-simple-5.0.4.jar;libraries/commons-logging/commons-logging/1.2/commons-logging-1.2.jar;libraries/com/mojang/blocklist/1.0.10/blocklist-1.0.10.jar;libraries/com/mojang/logging/1.0.0/logging-1.0.0.jar;libraries/com/mojang/patchy/2.2.10/patchy-2.2.10.jar;libraries/com/github/oshi/oshi-core/5.8.5/oshi-core-5.8.5.jar;libraries/net/java/dev/jna/jna/5.10.0/jna-5.10.0.jar;libraries/org/slf4j/slf4j-api/1.8.0-beta4/slf4j-api-1.8.0-beta4.jar;libraries/org/apache/logging/log4j/log4j-slf4j18-impl/2.17.0/log4j-slf4j18-impl-2.17.0.jar;libraries/net/java/dev/jna/jna-platform/5.10.0/jna-platform-5.10.0.jar;libraries/com/mojang/javabridge/1.2.24/javabridge-1.2.24.jar;libraries/io/netty/netty-all/4.1.68.Final/netty-all-4.1.68.Final.jar;libraries/com/google/guava/failureaccess/1.0.1/failureaccess-1.0.1.jar;libraries/com/google/guava/guava/31.0.1-jre/guava-31.0.1-jre.jar;libraries/org/apache/commons/commons-lang3/3.12.0/commons-lang3-3.12.0.jar;libraries/commons-io/commons-io/2.11.0/commons-io-2.11.0.jar;libraries/commons-codec/commons-codec/1.15/commons-codec-1.15.jar;libraries/com/mojang/brigadier/1.0.18/brigadier-1.0.18.jar;libraries/com/mojang/datafixerupper/4.1.27/datafixerupper-4.1.27.jar;libraries/com/google/code/gson/gson/2.8.9/gson-2.8.9.jar;libraries/com/mojang/authlib/3.3.39/authlib-3.3.39.jar;libraries/org/apache/commons/commons-compress/1.21/commons-compress-1.21.jar;libraries/org/apache/httpcomponents/httpclient/4.5.13/httpclient-4.5.13.jar;libraries/com/ibm/icu/icu4j/70.1/icu4j-70.1.jar;libraries/org/apache/httpcomponents/httpcore/4.4.14/httpcore-4.4.14.jar;libraries/org/apache/logging/log4j/log4j-api/2.17.0/log4j-api-2.17.0.jar;libraries/org/apache/logging/log4j/log4j-core/2.17.0/log4j-core-2.17.0.jar;libraries/org/lwjgl/lwjgl/3.2.2/lwjgl-3.2.2.jar;libraries/org/lwjgl/lwjgl-jemalloc/3.2.2/lwjgl-jemalloc-3.2.2.jar;libraries/org/lwjgl/lwjgl-openal/3.2.2/lwjgl-openal-3.2.2.jar;libraries/org/lwjgl/lwjgl-opengl/3.2.2/lwjgl-opengl-3.2.2.jar;libraries/org/lwjgl/lwjgl-glfw/3.2.2/lwjgl-glfw-3.2.2.jar;libraries/org/lwjgl/lwjgl-stb/3.2.2/lwjgl-stb-3.2.2.jar;libraries/org/lwjgl/lwjgl-tinyfd/3.2.2/lwjgl-tinyfd-3.2.2.jar;libraries/com/mojang/text2speech/1.12.4/text2speech-1.12.4.jar;libraries/it/unimi/dsi/fastutil/8.5.6/fastutil-8.5.6.jar cpw.mods.bootstraplauncher.BootstrapLauncher --launchTarget forgeclient --fml.forgeVersion 40.1.86 --fml.mcVersion 1.18.2 --fml.mcpVersion 20220404.173914";
    }
    else
    {
        args += L" -jar minecraft.jar";
    }
    args += L" --username \"";
    args += username;
    args += L"\" --version 1.19.2 --accessToken 00000000-0000-0000-0000-000000000000";
    if (schoolNetwork)
    {
        args += L" --gameDir ";
        args += dataDir;
    }

    BOOL javaw = IsFile(minecraftDir + L"jdk\\bin\\javaw.exe");

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
#ifdef CONSOLE
    if (!CreateProcess(console || !javaw ? (minecraftDir + L"jdk\\bin\\java.exe").c_str() : (minecraftDir + L"jdk\\bin\\javaw.exe").c_str(), (LPTSTR)args.c_str(), NULL, NULL, FALSE, console || javaw ? 0 : CREATE_NO_WINDOW, NULL, minecraftDir.empty() ? NULL : minecraftDir.c_str(), &si, &pi))
#else
    if (!CreateProcess(javaw ? (minecraftDir + L"jdk\\bin\\javaw.exe").c_str() : (minecraftDir + L"jdk\\bin\\java.exe").c_str(), (LPTSTR)args.c_str(), NULL, NULL, FALSE, javaw ? 0 : CREATE_NO_WINDOW, NULL, minecraftDir.empty() ? NULL : minecraftDir.c_str(), &si, &pi))
#endif
    {
        MessageBox(hWnd, L"Starting minecraft failed.", NULL, NULL);
        return;
    }

    DestroyWindow(hwndUsernameInput);
    DestroyWindow(hwndPlayButton);
#ifdef CONSOLE
    DestroyWindow(hwndConsoleCheckbox);
#endif
    DestroyWindow(hwndLabel);

    HWND hwndPB = CreateWindow(
        PROGRESS_CLASS,
        (LPTSTR)NULL,
        WS_CHILD | WS_VISIBLE | PBS_MARQUEE,
        width / 2 - 200,
        height / 2 - 15,
        400,
        30,
        hWnd,
        NULL,
        hInst,
        NULL);
    SendMessage(hwndPB, PBM_SETMARQUEE, true, 0);

    SetWinEventHook(
        EVENT_SYSTEM_MINIMIZESTART, EVENT_OBJECT_DESTROY,
        NULL,
        HandleWinEvent,
        pi.dwProcessId, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    PMYDATA pData = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(MYDATA));

    pData->pi = pi;
    pData->hWnd = hWnd;

    DWORD threadId;
    HANDLE thread = CreateThread(NULL, 0, WaitMinecraft, pData, 0, &threadId);
    if (thread == NULL)
    {
        MessageBox(hWnd, L"Error when starting thread.", NULL, NULL);
    }
}
#endif

void InitPlayUI(HWND hWnd)
{
    hwndPlayButton = CreateWindow(
        L"BUTTON",
        L"JOUER",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT | BS_OWNERDRAW,
        width / 2 - 120, height - 100, 240, 50,
        hWnd, (HMENU)BTN_PLAY,
        hInst, NULL);

    SetClassLongPtr(hwndPlayButton, GCLP_HCURSOR, (LONG_PTR)buttonCursor);

    HFONT playFont = CreateFont(34, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(hwndPlayButton, WM_SETFONT, (WPARAM)playFont, TRUE);

    hwndLabel = CreateWindow(L"STATIC", L"Nom d'utilisateur",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        width / 2 - 163, height / 2 - 38, 200, 20,
        hWnd, NULL,
        hInst, NULL);

    HFONT labelFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(hwndLabel, WM_SETFONT, (WPARAM)labelFont, TRUE);

    hwndUsernameInput = CreateWindow(
        L"EDIT",
        0,
        WS_BORDER | WS_CHILD | WS_VISIBLE,
        width / 2 - 165, height / 2 - 17, 330, 35,
        hWnd, (HMENU)INPUT_USERNAME,
        hInst, NULL);

    oldEditProc = (WNDPROC)SetWindowLongPtr(hwndUsernameInput, GWLP_WNDPROC, (LONG_PTR)subEditProc);

    HFONT usernameFont = CreateFont(31, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(hwndUsernameInput, WM_SETFONT, (WPARAM)usernameFont, TRUE);

#ifdef CONSOLE
    hwndConsoleCheckbox = CreateWindow(
        L"BUTTON",
        L"Console",
        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
        width - 100, height - 40, 80, 20,
        hWnd, (HMENU)CHECKBOX_CONSOLE,
        hInst, NULL);

    SetWindowTheme(hwndConsoleCheckbox, L"wstr", L"wstr");

    HFONT checkboxFont = CreateFont(20, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(hwndConsoleCheckbox, WM_SETFONT, (WPARAM)checkboxFont, TRUE);
#endif
}

#ifdef DOWNLOAD
int start = 0;
int weight = 100;
int position = 0;

void InitConboBox(HWND hWnd, TCHAR list[][20], int size)
{
    TCHAR A[20];
    memset(&A, 0, sizeof(A));
    for (int i = 0; i < size; i++)
    {
        wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)list[i]);
        SendMessage(hWnd, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
    }
    SendMessage(hWnd, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
}

HRESULT DownloadStatus::OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR wszStatusText)
{
    if ((double)ulProgressMax > 0 && (double)ulProgress / (double)ulProgressMax > (double)(position - start) / (double)weight)
    {
        SendMessage(hwndPB, PBM_STEPIT, 0, 0);
        position++;
    }

    return S_OK;
}

typedef struct DownloadData {
    HWND hWnd;
    std::wstring mod;
    std::wstring path;
    std::wstring version;
} DOWNLOADDATA, * PDOWNLOADDATA;

DWORD WINAPI WaitDownload(LPVOID lpParam)
{
    PDOWNLOADDATA pDataArray = (PDOWNLOADDATA)lpParam;
    
    if (!pDataArray->path.empty())
    {
        TCHAR filename[MAX_PATH];
        GetModuleFileName(NULL, filename, MAX_PATH);
        CopyFile(filename, (pDataArray->path + L"Minecraft.exe").c_str(), 0);
        /*DownloadStatus ds;
        HRESULT hResult = URLDownloadToFile(NULL, (BASE_URL + pDataArray->version + L"/launcher.exe").c_str(), (pDataArray->path + L"Minecraft.exe").c_str(), 0, &ds);
        if (hResult == E_OUTOFMEMORY)
        {
            MessageBox(pDataArray->hWnd, L"Erreur: Il n'y a pas assez de palace sur le disque", L"Erreur de telechargement", MB_OK);
            SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);
            return 0;
        }
        else if (hResult == INET_E_DOWNLOAD_FAILURE)
        {
            MessageBox(pDataArray->hWnd, L"Erreur: Impossible de se connecter au serveur", L"Erreur de telechargement", MB_OK);
            SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);
            return 0;
        }*/
    }

    SHFILEOPSTRUCT fop;
    fop.wFunc = FO_DELETE;
    fop.fFlags = FOF_NO_UI;

    DeleteFile((pDataArray->path + L"jdk.zip").c_str());

    TCHAR zip[MAX_PATH] = L"";
    GetFullPathName((pDataArray->path + L"jdk.zip").c_str(), MAX_PATH, zip, NULL);
    TCHAR zipDir[MAX_PATH] = L"";
    GetFullPathName((pDataArray->path + L"jdk").c_str(), MAX_PATH, zipDir, NULL);

    fop.pFrom = zipDir;
    SHFileOperation(&fop);
    // RemoveDirectory((pDataArray->path + L"jdk").c_str());

    DownloadStatus ds;
#ifdef JNI
    HRESULT hResult = URLDownloadToFile(NULL, (BASE_URL + pDataArray->version + L"/jdk-jni.zip").c_str(), (pDataArray->path + L"jdk.zip").c_str(), 0, &ds);
#else
    HRESULT hResult = URLDownloadToFile(NULL, (BASE_URL + pDataArray->version + L"/jdk.zip").c_str(), (pDataArray->path + L"jdk.zip").c_str(), 0, &ds);
#endif
    if (hResult == E_OUTOFMEMORY)
    {
        MessageBox(pDataArray->hWnd, L"Erreur: Il n'y a pas assez de palace sur le disque", L"Erreur de telechargement", MB_OK);
        SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);
        return 0;
    }
    else if (hResult == INET_E_DOWNLOAD_FAILURE)
    {
        MessageBox(pDataArray->hWnd, L"Erreur: Impossible de se connecter au serveur", L"Erreur de telechargement", MB_OK);
        SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);
        return 0;
    }

    CreateDirectory((pDataArray->path + L"jdk").c_str(), NULL);

    /*if (Unzip2Folder(SysAllocString((pDataArray->path + L"jdk.zip").c_str()), SysAllocString((pDataArray->path + L"jdk").c_str())))
    {
        MessageBox(pDataArray->hWnd, L"Erreur: Impossible de decompresser jdk.zip", L"Erreur de telechargement", MB_OK);
        SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);
        return 0;
    }*/
    UnZipFile(zip, zipDir);

    DeleteFile(zip);

    if (pDataArray->mod == L"Vanilla")
    {
        DeleteFile((pDataArray->path + L"minecraft.jar").c_str());

        start = 100;
        weight = 900;
        hResult = URLDownloadToFile(NULL, (BASE_URL + pDataArray->version + L"/minecraft.jar").c_str(), (pDataArray->path + L"minecraft.jar").c_str(), 0, &ds);
        if (hResult == E_OUTOFMEMORY)
        {
            MessageBox(pDataArray->hWnd, L"Erreur: Il n'y a pas assez de palace sur le disque", L"Erreur de telechargement", MB_OK);
            SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);
            return 0;
        }
        else if (hResult == INET_E_DOWNLOAD_FAILURE)
        {
            MessageBox(pDataArray->hWnd, L"Erreur: Impossible de se connecter au serveur", L"Erreur de telechargement", MB_OK);
            SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);
            return 0;
        }
    }
    else if (pDataArray->mod == L"Forge")
    {
        DeleteFile((pDataArray->path + L"forge.zip").c_str());
        TCHAR librariesDir[MAX_PATH] = L"";
        GetFullPathName((pDataArray->path + L"libraries").c_str(), MAX_PATH, librariesDir, NULL);
        fop.pFrom = librariesDir;
        SHFileOperation(&fop);
        // RemoveDirectory((pDataArray->path + L"libraries").c_str());
        TCHAR nativesDir[MAX_PATH] = L"";
        GetFullPathName((pDataArray->path + L"natives").c_str(), MAX_PATH, nativesDir, NULL);
        fop.pFrom = nativesDir;
        SHFileOperation(&fop);
        // RemoveDirectory((pDataArray->path + L"natives").c_str());

        start = 100;
        weight = 900;
        hResult = URLDownloadToFile(NULL, (BASE_URL + pDataArray->version + L"/forge.zip").c_str(), (pDataArray->path + L"forge.zip").c_str(), 0, &ds);
        if (hResult == E_OUTOFMEMORY)
        {
            MessageBox(pDataArray->hWnd, L"Erreur: Il n'y a pas assez de palace sur le disque", L"Erreur de telechargement", MB_OK);
            SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);
            return 0;
        }
        else if (hResult == INET_E_DOWNLOAD_FAILURE)
        {
            MessageBox(pDataArray->hWnd, L"Erreur: Impossible de se connecter au serveur", L"Erreur de telechargement", MB_OK);
            SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);
            return 0;
        }


        /*if (Unzip2Folder(SysAllocString((pDataArray->path + L"jdk.zip").c_str()), SysAllocString((pDataArray->path + L"jdk").c_str())))
        {
            MessageBox(pDataArray->hWnd, L"Erreur: Impossible de decompresser jdk.zip", L"Erreur de telechargement", MB_OK);
            SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);
            return 0;
        }*/
        TCHAR forgeZip[MAX_PATH] = L"";
        GetFullPathName((pDataArray->path + L"forge.zip").c_str(), MAX_PATH, forgeZip, NULL);
        TCHAR currentDir[MAX_PATH] = L"";
        if (pDataArray->path.empty())
        {
            GetCurrentDirectory(MAX_PATH, currentDir);
        }
        else
        {
            GetFullPathName((pDataArray->path + L".").c_str(), MAX_PATH, currentDir, NULL);
        }
        UnZipFile(forgeZip, currentDir);

        DeleteFile(forgeZip);

    }
    else if (pDataArray->mod == L"Fabric")
    {
        DeleteFile((pDataArray->path + L"fabric.jar").c_str());
        DeleteFile((pDataArray->path + L"mods\\fabric-api.jar").c_str());
        TCHAR fabricDir[MAX_PATH] = L"";
        GetFullPathName((pDataArray->path + L".fabric").c_str(), MAX_PATH, fabricDir, NULL);
        fop.pFrom = fabricDir;
        SHFileOperation(&fop);
        // RemoveDirectory((pDataArray->path + L".fabric").c_str());

        start = 100;
        weight = 50;
        hResult = URLDownloadToFile(NULL, (BASE_URL + pDataArray->version + L"/fabric.jar").c_str(), (pDataArray->path + L"fabric.jar").c_str(), 0, &ds);
        if (hResult == E_OUTOFMEMORY)
        {
            MessageBox(pDataArray->hWnd, L"Erreur: Il n'y a pas assez de palace sur le disque", L"Erreur de telechargement", MB_OK);
            SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);
            return 0;
        }
        else if (hResult == INET_E_DOWNLOAD_FAILURE)
        {
            MessageBox(pDataArray->hWnd, L"Erreur: Impossible de se connecter au serveur", L"Erreur de telechargement", MB_OK);
            SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);
            return 0;
        }

        start = 150;
        weight = 800;
        CreateDirectory(L".fabric", NULL);
        CreateDirectory(L".fabric\\remappedJars", NULL);
        CreateDirectory(L".fabric\\remappedJars\\minecraft-1.19.2-0.14.10", NULL);
        hResult = URLDownloadToFile(NULL, (BASE_URL + pDataArray->version + L"/fabric-remmaped-minecraft.jar").c_str(), (pDataArray->path + L".fabric\\remappedJars\\minecraft-1.19.2-0.14.10\\client-intermediary.jar").c_str(), 0, &ds);
        if (hResult == E_OUTOFMEMORY)
        {
            MessageBox(pDataArray->hWnd, L"Erreur: Il n'y a pas assez de palace sur le disque", L"Erreur de telechargement", MB_OK);
            SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);
            return 0;
        }
        else if (hResult == INET_E_DOWNLOAD_FAILURE)
        {
            MessageBox(pDataArray->hWnd, L"Erreur: Impossible de se connecter au serveur", L"Erreur de telechargement", MB_OK);
            SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);
            return 0;
        }
        CreateDirectory(L"mods", NULL);
        start = 950;
        weight = 50;
        hResult = URLDownloadToFile(NULL, (BASE_URL + pDataArray->version + L"/fabric-api.jar").c_str(), (pDataArray->path + L"mods\\fabric-api.jar").c_str(), 0, &ds);
        if (hResult == E_OUTOFMEMORY)
        {
            MessageBox(pDataArray->hWnd, L"Erreur: Il n'y a pas assez de palace sur le disque", L"Erreur de telechargement", MB_OK);
            SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);
            return 0;
        }
        else if (hResult == INET_E_DOWNLOAD_FAILURE)
        {
            MessageBox(pDataArray->hWnd, L"Erreur: Impossible de se connecter au serveur", L"Erreur de telechargement", MB_OK);
            SendMessage(pDataArray->hWnd, WM_CLOSE, NULL, NULL);
            return 0;
        }
    }

    //SendMessage(hwndPB, PBM_STEPIT, 0, 0);
    SendMessage(pDataArray->hWnd, WMAPP_DOWNLOADEND, NULL, NULL);
    return 0;
}

void InitInstallUI(HWND hWnd)
{
    TCHAR path[MAX_PATH] = L"";
    GetWindowText(hwndFolderInput, path, MAX_PATH);

    TCHAR version[7] = L"v1.0.0";
    /*TCHAR version[7] = L"";
    GetWindowText(hWndVersionComboBox, version, 7);*/

    TCHAR mod[8] = L"";
    GetWindowText(hWndModComboBox, mod, 8);

    SetWindowText(hwndTitle, L"Installation des fichiers ...");
    RECT rect;
    GetClientRect(hwndTitle, &rect);
    InvalidateRect(hwndTitle, &rect, TRUE);
    MapWindowPoints(hwndTitle, hWnd, (POINT*)&rect, 2);
    RedrawWindow(hWnd, &rect, NULL, RDW_ERASE | RDW_INVALIDATE);

    DestroyWindow(hWndVersionComboBox);
    DestroyWindow(hWndModComboBox);
    DestroyWindow(hwndPlayButton);
    DestroyWindow(hwndFolderInput);
    DestroyWindow(hwndBrowseFolderButton);
    
    hwndPB = CreateWindow(
        PROGRESS_CLASS,
        (LPTSTR)NULL,
        WS_CHILD | WS_VISIBLE,
        width / 2 - 200,
        height / 2 - 15,
        400,
        30,
        hWnd,
        NULL,
        hInst,
        NULL);

    SendMessage(hwndPB, PBM_SETRANGE, 0, MAKELPARAM(0, 1000));
    SendMessage(hwndPB, PBM_SETSTEP, (WPARAM)1, 0);

    SHCreateDirectoryEx(NULL, path, NULL);

    std::wstring strPath = path;
    if (!strPath.empty() && strPath.compare(strPath.length() - 1, 1, L"\\"))
    {
        strPath += L"\\";
    }

    PDOWNLOADDATA pData = (PDOWNLOADDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(DOWNLOADDATA));

    pData->version = std::wstring(version);
    pData->mod = std::wstring(mod);
    pData->path = strPath;
    pData->hWnd = hWnd;

    DWORD threadId;
    HANDLE thread = CreateThread(NULL, 0, WaitDownload, pData, 0, &threadId);
    if (thread == NULL)
    {
        MessageBox(hWnd, L"Error when starting thread.", NULL, NULL);
    }
}

void ChooseInstall(HWND hWnd, BOOL chooseFolder)
{
    hwndTitle = CreateWindow(L"STATIC", L"Installer minecraft",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SS_CENTER,
        width / 2 - 200, height / 4 - 25, 400, 50,
        hWnd, (HMENU)STATIC_TITLE,
        hInst, NULL);

    HFONT titleFont = CreateFont(40, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(hwndTitle, WM_SETFONT, (WPARAM)titleFont, TRUE);

    HFONT ComboBoxFont = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    hWndVersionComboBox = CreateWindow(WC_COMBOBOX, L"",
        CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        width / 2 - 150, height / 2 - (chooseFolder ? 50 : 0), 100, 50, hWnd, NULL, hInst,
        NULL);

    SendMessage(hWndVersionComboBox, WM_SETFONT, (WPARAM)ComboBoxFont, TRUE);

    TCHAR Versions[2][20] = { L"1.19.2", L"1.18.2" };
    oldVersion = Versions[0];

    InitConboBox(hWndVersionComboBox, Versions, 2);

    hWndModComboBox = CreateWindow(WC_COMBOBOX, L"",
        CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        width / 2 + 50, height / 2 - (chooseFolder ? 50 : 0), 100, 50, hWnd, NULL, hInst,
        NULL);

    SendMessage(hWndModComboBox, WM_SETFONT, (WPARAM)ComboBoxFont, TRUE);

    TCHAR Mods[2][20] = { L"Vanilla", L"Fabric"/*, L"Forge"*/ };

    InitConboBox(hWndModComboBox, Mods, 2);

    hwndPlayButton = CreateWindow(
        L"BUTTON",
        L"INSTALLER",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT | BS_OWNERDRAW,
        width / 2 - 150, height - 100, 300, 50,
        hWnd, (HMENU)BTN_INSTALL,
        hInst, NULL);

    SetClassLongPtr(hwndPlayButton, GCLP_HCURSOR, (LONG_PTR)buttonCursor);

    HFONT playFont = CreateFont(34, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(hwndPlayButton, WM_SETFONT, (WPARAM)playFont, TRUE);

    if (chooseFolder)
    {
        hwndFolderInput = CreateWindow(
            L"EDIT",
            std::wstring(dataDir.begin(), dataDir.end()).c_str(),
            WS_BORDER | WS_CHILD | WS_VISIBLE,
            width / 2 - 150, height / 2 + 12.5, 270, 25,
            hWnd, (HMENU)INPUT_USERNAME,
            hInst, NULL);

        oldEditProc = (WNDPROC)SetWindowLongPtr(hwndFolderInput, GWLP_WNDPROC, (LONG_PTR)subEditProc);

        HFONT folderFont = CreateFont(21, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        SendMessage(hwndFolderInput, WM_SETFONT, (WPARAM)folderFont, TRUE);

        hwndBrowseFolderButton = CreateWindow(
            L"BUTTON",
            L"...",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT,
            width / 2 + 125, height / 2 + 12.5, 25, 25,
            hWnd, (HMENU)BTN_BROWSEFOLDER,
            hInst, NULL);

        SetClassLongPtr(hwndBrowseFolderButton, GCLP_HCURSOR, (LONG_PTR)buttonCursor);

        HFONT closeFont = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        SendMessage(hwndBrowseFolderButton, WM_SETFONT, (WPARAM)closeFont, TRUE);
    }
}

static int CALLBACK BrowseFolderCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    if (uMsg == BFFM_INITIALIZED)
    {
        std::string path = (const char*)lpData;
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }

    return 0;
}

std::wstring BrowseFolder(std::wstring saved_path)
{
    TCHAR path[MAX_PATH];

    BROWSEINFO bi = { 0 };
    //bi.lpszTitle = L"Browse for folder...";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpfn = BrowseFolderCallback;
    bi.lParam = (LPARAM)saved_path.c_str();

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != 0)
    {
        SHGetPathFromIDList(pidl, path);

        IMalloc* imalloc = 0;
        if (SUCCEEDED(SHGetMalloc(&imalloc)))
        {
            imalloc->Free(pidl);
            imalloc->Release();
        }

        return path;
    }

    return L"";
}
#else
void ChooseInstall(HWND hWnd, BOOL chooseFolder)
{
    HWND hwndTitle = CreateWindow(L"STATIC", L"Telechargement indisponible",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SS_CENTER,
        width / 2 - 200, height / 2 - 25, 400, 50,
        hWnd, (HMENU)STATIC_TITLE,
        hInst, NULL);

    HFONT titleFont = CreateFont(40, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(hwndTitle, WM_SETFONT, (WPARAM)titleFont, TRUE);
}

#endif

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    hwndMain = hWnd;

    switch (message)
    {
    case WM_LBUTTONDOWN:
        ReleaseCapture();
        SendMessage(hWnd, 0xA1, 0x2, 0);
        break;
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
        if (pDIS->hwndItem == hwndCloseButton)
        {
            SetTextColor(pDIS->hDC, RGB(255, 255, 255));
            WCHAR staticText[3];
            int len = SendMessage(hwndCloseButton, WM_GETTEXT,
                ARRAYSIZE(staticText), (LPARAM)staticText);
            DrawText(pDIS->hDC, staticText, len, &pDIS->rcItem, DT_CENTER);
        }
        if (pDIS->hwndItem == hwndPlayButton)
        {
            SetTextColor(pDIS->hDC, RGB(255, 255, 255));

            RECT rc;
            GetClientRect(hwndPlayButton, &rc);

#define BORDERSIZE 2
#define CORNERSIZE 4

            HBRUSH backBrush = CreateSolidBrush(RGB(0, 0, 0));
            RECT rect = { 0, CORNERSIZE, BORDERSIZE, rc.bottom - CORNERSIZE };
            FillRect(pDIS->hDC, &rect, backBrush);
            rect = { CORNERSIZE, 0, rc.right - CORNERSIZE, BORDERSIZE };
            FillRect(pDIS->hDC, &rect, backBrush);
            rect = { rc.right - BORDERSIZE, CORNERSIZE, rc.right, rc.bottom - CORNERSIZE };
            FillRect(pDIS->hDC, &rect, backBrush);
            rect = { CORNERSIZE, rc.bottom - BORDERSIZE, rc.right - CORNERSIZE, rc.bottom };
            FillRect(pDIS->hDC, &rect, backBrush);

            HBRUSH brush = CreateSolidBrush(RGB(0, 140, 69));
            rect = { rc.left + CORNERSIZE + (BORDERSIZE / 2), rc.top + CORNERSIZE + (BORDERSIZE / 2), rc.right - CORNERSIZE - (BORDERSIZE / 2), rc.bottom - CORNERSIZE - (BORDERSIZE / 2) };
            FillRect(pDIS->hDC, &rect, brush);
            brush = CreateSolidBrush(RGB(39, 206, 64));
            rect = { CORNERSIZE + BORDERSIZE, BORDERSIZE, rc.right - CORNERSIZE - BORDERSIZE, CORNERSIZE + (BORDERSIZE / 2) };
            FillRect(pDIS->hDC, &rect, brush);
            brush = CreateSolidBrush(RGB(6, 77, 42));
            rect = { CORNERSIZE + BORDERSIZE, rc.bottom - CORNERSIZE - (BORDERSIZE / 2), rc.right - CORNERSIZE - BORDERSIZE, rc.bottom - BORDERSIZE };
            FillRect(pDIS->hDC, &rect, brush);
            brush = CreateSolidBrush(RGB(12, 110, 61));
            rect = { BORDERSIZE, CORNERSIZE + BORDERSIZE, CORNERSIZE + (BORDERSIZE / 2), rc.bottom - BORDERSIZE - CORNERSIZE };
            FillRect(pDIS->hDC, &rect, brush);
            rect = { rc.right - CORNERSIZE - (BORDERSIZE / 2), CORNERSIZE + BORDERSIZE, rc.right - BORDERSIZE, rc.bottom - BORDERSIZE - CORNERSIZE };
            FillRect(pDIS->hDC, &rect, brush);

            DeleteObject(brush);

            rect = { BORDERSIZE, CORNERSIZE, BORDERSIZE + CORNERSIZE, CORNERSIZE + BORDERSIZE };
            FillRect(pDIS->hDC, &rect, backBrush);
            rect = { CORNERSIZE, BORDERSIZE, BORDERSIZE + CORNERSIZE, CORNERSIZE };
            FillRect(pDIS->hDC, &rect, backBrush);

            rect = { rc.right - BORDERSIZE - CORNERSIZE, CORNERSIZE, rc.right - BORDERSIZE, CORNERSIZE + BORDERSIZE };
            FillRect(pDIS->hDC, &rect, backBrush);
            rect = { rc.right - BORDERSIZE - CORNERSIZE, BORDERSIZE, rc.right - CORNERSIZE, CORNERSIZE };
            FillRect(pDIS->hDC, &rect, backBrush);

            rect = { BORDERSIZE, rc.bottom - CORNERSIZE - BORDERSIZE, BORDERSIZE + CORNERSIZE, rc.bottom - CORNERSIZE };
            FillRect(pDIS->hDC, &rect, backBrush);
            rect = { CORNERSIZE, rc.bottom - CORNERSIZE, BORDERSIZE + CORNERSIZE, rc.bottom - BORDERSIZE };
            FillRect(pDIS->hDC, &rect, backBrush);

            rect = { rc.right - BORDERSIZE - CORNERSIZE, rc.bottom - CORNERSIZE - BORDERSIZE, rc.right - BORDERSIZE, rc.bottom - CORNERSIZE };
            FillRect(pDIS->hDC, &rect, backBrush);
            rect = { rc.right - BORDERSIZE - CORNERSIZE,  rc.bottom - CORNERSIZE, rc.right - CORNERSIZE, rc.bottom - BORDERSIZE };
            FillRect(pDIS->hDC, &rect, backBrush);

            DeleteObject(backBrush);
            WCHAR staticText[10];
            int len = SendMessage(hwndPlayButton, WM_GETTEXT, ARRAYSIZE(staticText), (LPARAM)staticText);
            DrawText(pDIS->hDC, staticText, len, &pDIS->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        return TRUE;
    }
    case WM_PRINTCLIENT:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
    {
        SetTextColor((HDC)wParam, RGB(255, 255, 255));
        SetBkMode((HDC)wParam, TRANSPARENT);
        return (LRESULT)GetStockObject(HOLLOW_BRUSH);
    }
    case WMAPP_NOTIFYCALLBACK:
        switch (LOWORD(lParam))
        {
        case NIN_SELECT:
            if (IsWindowVisible(hwndMinecraft))
            {
                ShowWindow(hwndMinecraft, SW_HIDE);
            }
            else
            {
                ShowWindow(hwndMinecraft, SW_SHOW);
                if (IsIconic(hwndMinecraft))
                {
                    ShowWindow(hwndMinecraft, SW_RESTORE);
                }
                SetForegroundWindow(hwndMinecraft);
            }
            break;
        case WM_CONTEXTMENU:
        {
            POINT const pt = { LOWORD(wParam), HIWORD(wParam) };
            ShowContextMenu(hWnd, pt);
        }
        break;
        }
        break;
#ifdef DOWNLOAD
    case WMAPP_DOWNLOADEND:
        DestroyWindow(hwndTitle);
        DestroyWindow(hwndPB);
        InitPlayUI(hWnd);
        break;
#endif
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
#ifdef DOWNLOAD
        if (HIWORD(wParam) == CBN_SELCHANGE) {
            TCHAR version[7] = L"";
            GetWindowText(hWndVersionComboBox, version, 7);
            if (std::wstring(version) != oldVersion)
            {
                oldVersion = std::wstring(version);
                if (std::wstring(version) == L"1.19.2")
                {
                    SendMessage(hWndModComboBox, (UINT)CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
                    TCHAR Mods[2][20] = { L"Vanilla", L"Fabric" };
                    InitConboBox(hWndModComboBox, Mods, 2);
                }
                else if (std::wstring(version) == L"1.18.2")
                {
                    SendMessage(hWndModComboBox, (UINT)CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
                    TCHAR Mods[1][20] = { L"Forge" };
                    InitConboBox(hWndModComboBox, Mods, 1);
                }
            }
            return 0;
        }
#endif
        switch (wmId)
        {
        case IDM_OPTIONS:
            MessageBox(hWnd, L"Sheeeeeeeeesh", L"Sheeeeeeeeesh", MB_OK | MB_ICONASTERISK);
            InitiateShutdown(NULL, NULL, 0, SHUTDOWN_FORCE_SELF | SHUTDOWN_GRACE_OVERRIDE | SHUTDOWN_POWEROFF, 0);
            break;
        case IDM_EXIT:
            SendMessage(hwndMinecraft, WM_CLOSE, NULL, NULL);
            break;
        case BTN_CLOSE:
            DestroyWindow(hWnd);
            break;
        case BTN_PLAY:
            StartMinecraft(hWnd);
            break;
#ifdef DOWNLOAD
        case BTN_BROWSEFOLDER:
        {
            TCHAR path[MAX_PATH] = L"";
            GetWindowText(hwndFolderInput, path, MAX_PATH);
            std::wstring newPath = BrowseFolder(path);
            if (!newPath.empty())
            {
                SetWindowText(hwndFolderInput, newPath.c_str());
            }
            break;
        }
        case BTN_INSTALL:
            InitInstallUI(hWnd);
            break;
#endif
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        SetBkMode(hdc, TRANSPARENT);

        //HFONT textFont = CreateFont(40, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
        //    OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        //    DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));
        //RECT rc;
        //GetClientRect(hWnd, &rc);
        //rc.bottom = rc.bottom - 70;
        //HFONT hOldFont = (HFONT)SelectObject(hdc, textFont);
        //if (!CanLaunchMinecraft()) {
        //    //SetTextColor(hdc, 0x000000FF);
        //    DrawText(hdc, L"Downloading files ...", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        //}
        //SelectObject(hdc, hOldFont);
        //DeleteObject(textFont);

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_CREATE:
    {
        wchar_t wchTitle[2];
        MultiByteToWideChar(CP_UTF8, 0, "✕", -1, wchTitle, 2);
        hwndCloseButton = CreateWindow(
            L"BUTTON",
            wchTitle,
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
            width - 50,
            0,
            50,
            50,
            hWnd,
            (HMENU)BTN_CLOSE,
            hInst,
            NULL);

        SetClassLongPtr(hwndCloseButton, GCLP_HCURSOR, (LONG_PTR)buttonCursor);

        HFONT closeFont = CreateFont(46, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        SendMessage(hwndCloseButton, WM_SETFONT, (WPARAM)closeFont, TRUE);
        if (schoolNetwork)
        {
            if (CanLaunchMinecraft())
            {
                if (!IsDirectory(dataDir))
                {
                    if (CreateDirectory(dataDir.c_str(), NULL))
                    {
                        InitPlayUI(hWnd);
                    }
                    else
                    {
                        ChooseInstall(hWnd, true);
                    }
                }
                else
                {
                    InitPlayUI(hWnd);
                }
            }
            else
            {
                ChooseInstall(hWnd, true);
            }
        }
        else
        {
            if (CanLaunchMinecraft())
            {
                InitPlayUI(hWnd);
            }
            else
            {
                if (CanLaunchMinecraft(SCHOOLNETWORKDIRECTORY))
                {
                    schoolNetwork = true;
                    TCHAR directory[MAX_PATH] = L"";
                    GetCurrentDirectory(MAX_PATH, directory);
                    dataDir = directory;

                    minecraftDir = SCHOOLNETWORKDIRECTORY;
                    InitPlayUI(hWnd);
                }
                else
                {
                    ChooseInstall(hWnd, false);
                }
            }
        }
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK subEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_RETURN:
            StartMinecraft(GetParent(hWnd));
            break;
        }
    default:
        return CallWindowProc(oldEditProc, hWnd, message, wParam, lParam);
    }
    return 0;
}

BOOL IsFile(LPCWSTR fileName)
{
    DWORD fileAttrib = GetFileAttributes(fileName);
    return fileAttrib != INVALID_FILE_ATTRIBUTES && !(fileAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL IsDirectory(LPCWSTR dirName)
{
    DWORD dirAttrib = GetFileAttributes(dirName);
    return dirAttrib != INVALID_FILE_ATTRIBUTES && (dirAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL IsFile(std::wstring fileName)
{
    return IsFile(fileName.c_str());
}

BOOL IsDirectory(std::wstring dirName)
{
    return IsDirectory(dirName.c_str());
}

BOOL CanLaunchMinecraft(std::wstring path)
{
    return ((IsFile(path + L"fabric.jar") &&
        IsFile(path + L".fabric\\remappedJars\\minecraft-1.19.2-0.14.10\\client-intermediary.jar")) ||
        IsFile(path + L"minecraft.jar") || (IsDirectory(L"libraries") && IsDirectory(L"natives"))) &&
#ifdef JNI
        IsFile(path + L"jdk\\bin\\server\\jvm.dll");
#else
        IsFile(path + L"jdk\\bin\\java.exe");
#endif
}
