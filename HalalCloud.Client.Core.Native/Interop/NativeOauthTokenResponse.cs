using System.Runtime.InteropServices;
using V6.Services.Pub;

namespace HalalCloud.Client.Core.Interop
{
    [StructLayout(LayoutKind.Sequential)]
    public struct NativeOauthTokenResponse
    {
        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Url;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Addon;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Input;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Type;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Callback;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string ReturnUrl;

        public int ReturnType;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Captcha;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string State;

        public NativeOauthTokenResponse(OauthTokenResponse Source)
        {
            Url = Source.Url;
            Addon = Source.Addon;
            Input = Source.Input;
            Type = Source.Type;
            Callback = Source.Callback;
            ReturnUrl = Source.ReturnUrl;
            ReturnType = Source.ReturnType;
            Captcha = Source.Captcha;
            State = Source.State;
        }
    }
}
