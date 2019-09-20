using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace HUDPlugConsole
{
    class PlugConsole
    {

        /// <summary>
        /// 将HUD绘图插件注入游戏进程
        /// </summary>
        /// <param name="threadId">游戏进程主线程</param>
        [DllImport("HUDPlug.dll")]
        private static extern void SetHook(uint threadId);


        /// <summary>
        /// 应用程序入口
        /// </summary>
        /// <param name="args">（未使用）应用程序初始化参数</param>
        [STAThread]
        public static void Main(string[] args)
        {

            // 创建并运行窗口
            App app = new App();
            MainWindow window = new MainWindow();
            app.MainWindow = window;
            app.InitializeComponent();
            app.Run();
            
        }
    }
}
