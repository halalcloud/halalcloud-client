using HalalCloud.Client.Core.Interop;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

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
                    throw new ArgumentNullException();
                }

                *Session = new SessionManager().ToIntPtr();

                return 0;
            }
            catch (Exception e)
            {
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
                Session.ToGcHandle().Free();

                return 0;
            }
            catch (Exception e)
            {
                return e.HResult;
            }
        }

        [UnmanagedCallersOnly(
            CallConvs = [typeof(CallConvStdcall)],
            EntryPoint = "HccAuthenticate")]
        public static unsafe int Authenticate(
            IntPtr Session,
            IntPtr Callback)
        {
            try
            {
                Session.ToObject<SessionManager>().Authenticate(
                    Callback.ToDelegate<
                        SessionManager.AuthenticationNotifyCallback>());

                return 0;
            }
            catch (Exception e)
            {
                return e.HResult;
            }
        }

        [UnmanagedCallersOnly(
            CallConvs = [typeof(CallConvStdcall)],
            EntryPoint = "HccImpersonate")]
        public static unsafe int Impersonate(
            IntPtr Session,
            IntPtr RefreshToken)
        {
            try
            {
                Session.ToObject<SessionManager>().Impersonate(
                    RefreshToken.ToUtf8String());

                return 0;
            }
            catch (Exception e)
            {
                return e.HResult;
            }
        }

        [UnmanagedCallersOnly(
           CallConvs = [typeof(CallConvStdcall)],
           EntryPoint = "HccGetTokenInformation")]
        public static unsafe int GetTokenInformation(
           IntPtr Session,
           IntPtr Information)
        {
            try
            {
                SessionManager ManagedSession =
                    Session.ToObject<SessionManager>();

                Marshal.StructureToPtr(
                    new NativeToken(ManagedSession.AccessToken),
                    Information,
                    true);

                return 0;
            }
            catch (Exception e)
            {
                return e.HResult;
            }
        }

        [UnmanagedCallersOnly(
           CallConvs = [typeof(CallConvStdcall)],
           EntryPoint = "HccGetUserInformation")]
        public static unsafe int GetUserInformation(
           IntPtr Session,
           IntPtr Information)
        {
            try
            {
                SessionManager ManagedSession =
                    Session.ToObject<SessionManager>();

                Marshal.StructureToPtr(
                    new NativeUser(ManagedSession.GetUserInformation()),
                    Information,
                    true);

                return 0;
            }
            catch (Exception e)
            {
                return e.HResult;
            }
        }

        [UnmanagedCallersOnly(
           CallConvs = [typeof(CallConvStdcall)],
           EntryPoint = "HccLogout")]
        public static unsafe int Logout(
           IntPtr Session)
        {
            try
            {
                Session.ToObject<SessionManager>().Logout();

                return 0;
            }
            catch (Exception e)
            {
                return e.HResult;
            }
        }

        [UnmanagedCallersOnly(
            CallConvs = [typeof(CallConvStdcall)],
            EntryPoint = "HccUploadFile")]
        public static unsafe int UploadFile(
            IntPtr Session,
            IntPtr SourceFilePath,
            IntPtr TargetFilePath)
        {
            try
            {
                Session.ToObject<SessionManager>().UploadFile(
                    SourceFilePath.ToUtf8String(),
                    TargetFilePath.ToUtf8String());

                return 0;
            }
            catch (Exception e)
            {
                return e.HResult;
            }
        }
    }
}
