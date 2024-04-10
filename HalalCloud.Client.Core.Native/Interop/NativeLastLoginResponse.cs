using System.Runtime.InteropServices;
using V6.Services.Pub;

namespace HalalCloud.Client.Core.Interop
{
    [StructLayout(LayoutKind.Sequential)]
    public struct NativeLastLoginResponse
    {
        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Identity;

        /// <summary>
        /// millisecond
        /// </summary>
        public long LastLoginTs;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string LastLoginIp;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string LastLoginDevice;

        public NativeLastLoginResponse(LastLoginResponse Source)
        {
            Identity = Source.Identity;
            LastLoginTs = Source.LastLoginTs;
            LastLoginIp = Source.LastLoginIp;
            LastLoginDevice = Source.LastLoginDevice;
    }
    }
}
