using Microsoft.Extensions.Logging;

namespace BaiduBce.Util
{
    public class LogUtils
    {
        public static ILoggerFactory Factory = LoggerFactory.Create(
            Builder =>
            {
                Builder.AddConsole();
                Builder.AddEventLog();
                Builder.SetMinimumLevel(LogLevel.Trace);
            });
    }
}
