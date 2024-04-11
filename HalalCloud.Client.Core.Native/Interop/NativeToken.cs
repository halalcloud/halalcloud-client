using System.Runtime.InteropServices;
using V6.Services.Pub;

namespace HalalCloud.Client.Core.Interop
{
    [StructLayout(LayoutKind.Sequential)]
    public struct NativeToken
    {
        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Identity = string.Empty;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string UserIdentity = string.Empty;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Device = string.Empty;

        public int Version;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string AccessToken = string.Empty;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string RefreshToken = string.Empty;

        public long UpdateTs;

        public long AccessTokenExpireTs;

        public long RefreshTokenExpireTs;

        public NativeToken(Token? Source)
        {
            if (Source != null)
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
}
