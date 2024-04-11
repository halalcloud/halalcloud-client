using System.Runtime.InteropServices;
using V6.Services.Pub;

namespace HalalCloud.Client.Core.Interop
{
    [StructLayout(LayoutKind.Sequential)]
    public struct NativeLoginResponse
    {
        public NativeToken Token;

        public NativeUser User;

        public NativeLastLoginResponse LastLogin;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string State = string.Empty;

        public NativeLoginResponse(LoginResponse? Source)
        {
            if (Source != null)
            {
                Token = new NativeToken(Source.Token);
                User = new NativeUser(Source.User);
                LastLogin = new NativeLastLoginResponse(Source.LastLogin);
                State = Source.State;
            }
        }
    }
}
