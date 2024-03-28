using Grpc.Core;
using Grpc.Core.Interceptors;
using Grpc.Net.Client;
using System.Runtime.InteropServices;
using System.Text;
using V6.Services.Pub;

namespace HalalCloud.Client.Core
{
    public class ExportedInterfaces
    {
        [UnmanagedCallersOnly(EntryPoint = "HccCreateInvoker")]
        public static IntPtr CreateInvoker()
        {
            GrpcChannel Channel = GrpcChannel.ForAddress(
                "https://grpcuserapi.2dland.cn");

            CallInvoker Invoker = Channel.Intercept(
                new SignatureInterceptor());

            return GCHandle.ToIntPtr(
                GCHandle.Alloc(Invoker, GCHandleType.Pinned));
        }

        [UnmanagedCallersOnly(EntryPoint = "HccFreeInvoker")]
        public static void FreeInvoker(IntPtr Invoker)
        {
            GCHandle.FromIntPtr(Invoker).Free();
        }

        private static CallInvoker? GetInvoker(IntPtr Invoker)
        {
            return GCHandle.FromIntPtr(Invoker).Target as CallInvoker;
        }

        //[UnmanagedCallersOnly(EntryPoint = "HccCreateAuthToken")]
        //public static int CreateAuthToken(int a, int b)
        //{
        //    return a + b;
        //}

        [UnmanagedCallersOnly(EntryPoint = "HccCreateSessionManager")]
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

        [UnmanagedCallersOnly(EntryPoint = "HccCreateAuthToken")]
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
