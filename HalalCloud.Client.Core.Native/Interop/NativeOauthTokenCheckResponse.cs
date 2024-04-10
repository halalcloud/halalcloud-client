using System.Runtime.InteropServices;
using V6.Services.Pub;

namespace HalalCloud.Client.Core.Interop
{
    [StructLayout(LayoutKind.Sequential)]
    public struct NativeOauthTokenCheckResponse
    {
        NativeLoginResponse Login;

        NativeOauthTokenResponse Oauth;

        public int Status;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Message;

        public NativeOauthTokenCheckResponse(OauthTokenCheckResponse Source)
        {
            Login = new NativeLoginResponse(Source.Login);
            Oauth = new NativeOauthTokenResponse(Source.Oauth);
            Status = Source.Status;
            Message = Source.Message;
    }
    }
}
