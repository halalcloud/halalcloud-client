using System;
using System.Reflection.Metadata;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using V6.Services.Pub;

namespace HalalCloud.Client.Core
{
    public class ExportedInterfaces
    {
        [UnmanagedCallersOnly(
            CallConvs = [typeof(CallConvStdcall)],
            EntryPoint = "HccCreateSessionManager")]
        public static unsafe int CreateSessionManager(
            IntPtr* Session)
        {
            try
            {
                if (Session == null)
                {
                    throw new ArgumentException();
                }

                SessionManager ManagedSession = new SessionManager();

                *Session = GCHandle.ToIntPtr(GCHandle.Alloc(ManagedSession));
                return 0;
            }
            catch (Exception e)
            {
                if (Session != null)
                {
                    *Session = IntPtr.Zero;
                }
                return e.HResult;
            }
        }

        [UnmanagedCallersOnly(
            CallConvs = [typeof(CallConvStdcall)],
            EntryPoint = "HccCloseSessionManager")]
        public static unsafe int CloseSessionManager(
            IntPtr Session)
        {
            try
            {
                if (Session == IntPtr.Zero)
                {
                    throw new ArgumentException();
                }

                GCHandle.FromIntPtr(Session).Free();

                return 0;
            }
            catch (Exception e)
            {
                return e.HResult;
            }
        }

        private static SessionManager? GetSessionManager(
            IntPtr Interface)
        {
            return GCHandle.FromIntPtr(Interface).Target as SessionManager;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct CreateAuthTokenResponse
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

        [UnmanagedCallersOnly(
            CallConvs = [typeof(CallConvStdcall)],
            EntryPoint = "HccCreateAuthToken")]
        public static unsafe int CreateAuthToken(
            IntPtr Session,
            IntPtr NativeResponse)
        {
            try
            {
                if (Session == IntPtr.Zero)
                {
                    throw new ArgumentException();
                }

                if (NativeResponse == IntPtr.Zero)
                {
                    throw new ArgumentException();
                }

                SessionManager? ManagedSession = GetSessionManager(Session);
                if (ManagedSession == null)
                {
                    throw new ArgumentException();
                }

                OauthTokenResponse Response = ManagedSession.CreateAuthToken();

                CreateAuthTokenResponse ManagedResponse =
                    new CreateAuthTokenResponse();

                ManagedResponse.Url = Response.Url;
                ManagedResponse.Addon = Response.Addon;
                ManagedResponse.Input = Response.Input;
                ManagedResponse.Type = Response.Type;
                ManagedResponse.Callback = Response.Callback;
                ManagedResponse.ReturnUrl = Response.ReturnUrl;
                ManagedResponse.ReturnType = Response.ReturnType;
                ManagedResponse.Captcha = Response.Captcha;
                ManagedResponse.State = Response.State;

                Marshal.StructureToPtr(ManagedResponse, NativeResponse, true);

                return 0;
            }
            catch (Exception e)
            {
                return e.HResult;
            }
        }
    }
}
