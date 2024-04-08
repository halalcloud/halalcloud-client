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
                *Session = IntPtr.Zero;
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
            EntryPoint = "HccCreateAuthToken")]
        public static unsafe int CreateAuthToken(
            IntPtr Session)
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

                OauthTokenResponse Response = ManagedSession.CreateAuthToken();
                Console.WriteLine(Response.ReturnUrl);
                Console.WriteLine(Response.Callback);

                return 0;
            }
            catch (Exception e)
            {
                return e.HResult;
            }
        }
    }
}
