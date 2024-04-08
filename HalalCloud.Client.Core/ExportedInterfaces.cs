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
            IntPtr* Interface)
        {
            try
            {
                if (Interface == null)
                {
                    throw new ArgumentException();
                }

                *Interface = GCHandle.ToIntPtr(GCHandle.Alloc(
                    new SessionManager(),
                    GCHandleType.Pinned));
                return 0;
            }
            catch (Exception e)
            {
                *Interface = IntPtr.Zero;
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
            IntPtr Interface)
        {
            try
            {
                if (Interface == IntPtr.Zero)
                {
                    throw new ArgumentException();
                }

                SessionManager? Session = GetSessionManager(Interface);
                if (Session == null)
                {
                    throw new ArgumentException();
                }

                OauthTokenResponse Response = Session.CreateAuthToken();
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
