using HalalCloud.Client.Core.Interop;
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

        [UnmanagedCallersOnly(
            CallConvs = [typeof(CallConvStdcall)],
            EntryPoint = "HccLoginWithAuthenticationUri")]
        public static unsafe int LoginWithAuthenticationUri(
            IntPtr Session,
            IntPtr Callback)
        {
            try
            {
                if (Session == IntPtr.Zero)
                {
                    throw new ArgumentException();
                }

                if (Callback == IntPtr.Zero)
                {
                    throw new ArgumentException();
                }

                SessionManager? ManagedSession = GetSessionManager(Session);
                if (ManagedSession == null)
                {
                    throw new ArgumentException();
                }

                ManagedSession.LoginWithAuthenticationUri(
                    Marshal.GetDelegateForFunctionPointer<
                        SessionManager.LoginNotifyAuthenticationUriCallback>(
                        Callback));

                return 0;
            }
            catch (Exception e)
            {
                return e.HResult;
            }
        }

        [UnmanagedCallersOnly(
            CallConvs = [typeof(CallConvStdcall)],
            EntryPoint = "HccLoginWithRefreshToken")]
        public static unsafe int LoginWithRefreshToken(
            IntPtr Session,
            IntPtr RefreshToken)
        {
            try
            {
                if (Session == IntPtr.Zero)
                {
                    throw new ArgumentException();
                }

                SessionManager? ManagedSession = GetSessionManager(Session);
                if (ManagedSession == null)
                {
                    throw new ArgumentException();
                }

                ManagedSession.LoginWithRefreshToken(
                    Marshal.PtrToStringUTF8(RefreshToken) ?? string.Empty);

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
                if (Session == IntPtr.Zero)
                {
                    throw new ArgumentException();
                }

                if (Information == IntPtr.Zero)
                {
                    throw new ArgumentException();
                }

                SessionManager? ManagedSession = GetSessionManager(Session);
                if (ManagedSession == null)
                {
                    throw new ArgumentException();
                }

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
            EntryPoint = "HccUploadFile")]
        public static unsafe int UploadFile(
            IntPtr Session,
            IntPtr SourceFilePath,
            IntPtr TargetDirectoryPath,
            IntPtr TargetFileName)
        {
            try
            {
                if (Session == IntPtr.Zero)
                {
                    throw new ArgumentException();
                }

                if (SourceFilePath == IntPtr.Zero)
                {
                    throw new ArgumentException();
                }

                if (TargetDirectoryPath == IntPtr.Zero)
                {
                    throw new ArgumentException();
                }

                if (TargetFileName == IntPtr.Zero)
                {
                    throw new ArgumentException();
                }

                SessionManager? ManagedSession = GetSessionManager(Session);
                if (ManagedSession == null)
                {
                    throw new ArgumentException();
                }

                ManagedSession.UploadFile(
                    Marshal.PtrToStringUTF8(SourceFilePath) ?? string.Empty,
                    Marshal.PtrToStringUTF8(TargetDirectoryPath) ?? string.Empty,
                    Marshal.PtrToStringUTF8(TargetFileName) ?? string.Empty);

                return 0;
            }
            catch (Exception e)
            {
                return e.HResult;
            }
        }
    }
}
