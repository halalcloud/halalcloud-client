using System.Runtime.InteropServices;
using V6.Services.Pub;

namespace HalalCloud.Client.Core.Interop
{
    [StructLayout(LayoutKind.Sequential)]
    public struct NativeOauthTokenResponse
    {
        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Url = string.Empty;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Addon = string.Empty;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Input = string.Empty;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Type = string.Empty;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Callback = string.Empty;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string ReturnUrl = string.Empty;

        public int ReturnType;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Captcha = string.Empty;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string State = string.Empty;

        public NativeOauthTokenResponse(OauthTokenResponse? Source)
        {
            if (Source != null)
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
}
