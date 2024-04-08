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
            IntPtr* NativeSession)
        {
            try
            {
                if (NativeSession == null)
                {
                    throw new ArgumentException();
                }

                SessionManager Session = new SessionManager();

                *NativeSession = GCHandle.ToIntPtr(GCHandle.Alloc(Session));
                return 0;
            }
            catch (Exception e)
            {
                *NativeSession = IntPtr.Zero;
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
            IntPtr NativeSession)
        {
            try
            {
                if (NativeSession == IntPtr.Zero)
                {
                    throw new ArgumentException();
                }

                SessionManager? Session = GetSessionManager(NativeSession);
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
