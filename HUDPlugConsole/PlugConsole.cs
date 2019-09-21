using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Windows;

namespace HUDPlugConsole
{
    class PlugConsole
    {

        
        private delegate void SetHookFn(uint threadId);

        [DllImport("kernel32.dll", EntryPoint = "LoadLibrary")]
        public static extern int LoadLibrary(
            [MarshalAs(UnmanagedType.LPStr)] string lpLibFileName);

        [DllImport("kernel32.dll", EntryPoint = "GetProcAddress")]
        public static extern IntPtr GetProcAddress(int hModule,
            [MarshalAs(UnmanagedType.LPStr)] string lpProcName);

        [DllImport("kernel32.dll", EntryPoint = "FreeLibrary")]
        public static extern bool FreeLibrary(int hModule);

        /// <summary>
        /// 查找窗口
        /// </summary>
        /// <param name="lpClassName">窗口类别名</param>
        /// <param name="lpWindowName">窗口名</param>
        /// <returns>原型时HWND类型，窗口的句柄</returns>
        [DllImport("user32.dll", EntryPoint = "FindWindow")]
        private extern static IntPtr FindWindow(string lpClassName, string lpWindowName);

        /// <summary>
        /// 获取窗口进程ID和线程ID
        /// </summary>
        /// <param name="hwnd">窗口句柄</param>
        /// <param name="processId">接收进程ID</param>
        /// <returns>线程ID</returns>
        [DllImport("User32.dll", CharSet = CharSet.Auto)]
        private static extern uint GetWindowThreadProcessId(IntPtr hwnd, out uint processId);

        /// <summary>
        /// 应用程序入口
        /// </summary>
        /// <param name="args">（未使用）应用程序初始化参数</param>
        [STAThread]
        public static void Main(string[] args)
        {

            //var hwnd = FindWindow("EGO_WindowClass", "F1 2018(DirectX 11)");
            var hwnd = FindWindow(null, "远程桌面连接");
            if(hwnd != IntPtr.Zero)
            {
                uint processId;
                uint threadId = GetWindowThreadProcessId(hwnd, out processId);
                int hModule = LoadLibrary(System.Environment.CurrentDirectory + "\\HUDPlug.dll");
                IntPtr fnPtr = GetProcAddress(hModule, "SetHook");
                SetHookFn setHook = (SetHookFn)Marshal.GetDelegateForFunctionPointer(fnPtr, typeof(SetHookFn));
                setHook(threadId);
            }
            // 创建并运行窗口
            App app = new App();
            app.InitializeComponent();
            app.Run(); 
        }
    }
}
