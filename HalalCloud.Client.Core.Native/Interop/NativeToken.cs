using System.Runtime.InteropServices;
using V6.Services.Pub;

namespace HalalCloud.Client.Core.Interop
{
    [StructLayout(LayoutKind.Sequential)]
    public struct NativeToken
    {
        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Identity;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string UserIdentity;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Device;

        public int Version;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string AccessToken;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string RefreshToken;

        public long UpdateTs;

        public long AccessTokenExpireTs;

        public long RefreshTokenExpireTs;

        public NativeToken(Token Source)
        {
            Identity = Source.Identity;
            UserIdentity = Source.UserIdentity;
            Device = Source.Device;
            Version = Source.Version;
            AccessToken = Source.AccessToken;
            RefreshToken = Source.RefreshToken;
            UpdateTs = Source.UpdateTs;
            AccessTokenExpireTs = Source.AccessTokenExpireTs;
            RefreshTokenExpireTs = Source.RefreshTokenExpireTs;
    }
    }
}
