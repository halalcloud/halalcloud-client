using System.Runtime.InteropServices;
using V6.Services.Pub;

namespace HalalCloud.Client.Core.Interop
{
    [StructLayout(LayoutKind.Sequential)]
    public struct NativeLastLoginResponse
    {
        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Identity = string.Empty;

        /// <summary>
        /// millisecond
        /// </summary>
        public long LastLoginTs;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string LastLoginIp = string.Empty;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string LastLoginDevice = string.Empty;

        public NativeLastLoginResponse(LastLoginResponse? Source)
        {
            if (Source != null)
            {
                Identity = Source.Identity;
                LastLoginTs = Source.LastLoginTs;
                LastLoginIp = Source.LastLoginIp;
                LastLoginDevice = Source.LastLoginDevice;
            }
        }
    }
}
