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
    }
}
