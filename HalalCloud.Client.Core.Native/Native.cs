using System.Runtime.InteropServices;

namespace HalalCloud.Client.Core
{
    internal class Native
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct OauthTokenResponse
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
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Token
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
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct User
        {
            /// <summary>
            /// identity for user, unique in system
            /// </summary>
            [MarshalAs(UnmanagedType.LPUTF8Str)]
            public string Identity;

            /// <summary>
            /// 1: user, 2: admin, 3: super admin
            /// </summary>
            public int Type;

            /// <summary>
            /// 1: normal, 2: disabled, 3: deleted
            /// </summary>
            public int Status;

            public long UpdateTs;

            [MarshalAs(UnmanagedType.LPUTF8Str)]
            public string Password;

            [MarshalAs(UnmanagedType.LPUTF8Str)]
            public string Name;

            [MarshalAs(UnmanagedType.LPUTF8Str)]
            public string Addon;

            public long CreateTs;

            [MarshalAs(UnmanagedType.LPUTF8Str)]
            public string Hash;

            [MarshalAs(UnmanagedType.LPUTF8Str)]
            public string Icon;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct LastLoginResponse
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
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct LoginResponse
        {
            public Token Token;

            public User User;

            public LastLoginResponse LastLogin;

            [MarshalAs(UnmanagedType.LPUTF8Str)]
            public string State;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct OauthTokenCheckResponse
        {
            LoginResponse Login;

            OauthTokenResponse Oauth;

            public int Status;

            [MarshalAs(UnmanagedType.LPUTF8Str)]
            public string Message;
        }
    }
}
