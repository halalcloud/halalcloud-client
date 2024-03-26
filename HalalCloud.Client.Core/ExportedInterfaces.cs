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

        static string MD5(string s)
        {
            using var provider = System.Security.Cryptography.MD5.Create();
            StringBuilder builder = new StringBuilder();

            foreach (byte b in provider.ComputeHash(Encoding.UTF8.GetBytes(s)))
                builder.Append(b.ToString("x2").ToLower());

            return builder.ToString();
        }

        [UnmanagedCallersOnly(EntryPoint = "HccConsoleAppMain")]
        public static void ConsoleAppMain()
        {
            var Channel = GrpcChannel.ForAddress("https://grpcuserapi.2dland.cn");

            var Invoker = Channel.Intercept(new SignatureInterceptor());

            var Client = new PubUser.PubUserClient(Invoker);

            string Callback = string.Empty;

            {
                LoginRequest loginRequest = new LoginRequest();
                loginRequest.ReturnType = 2;
                var Response = Client.CreateAuthToken(loginRequest);
                Callback = Response.Callback;
                Console.WriteLine(Response.ReturnUrl);
            }

            for (; ; )
            {
                LoginRequest loginRequest = new LoginRequest();
                loginRequest.Type = "2";
                loginRequest.ReturnType = 2;
                loginRequest.Callback = Callback;
                var Response = Client.VerifyAuthToken(
                    loginRequest);
                if (Response.Status == 6)
                {
                    string AccessToken = Response.Login.Token.AccessToken;
                    Console.WriteLine(AccessToken);
                    break;
                }
                Console.WriteLine("Waiting...");
                Thread.Sleep(200);
            }

            //PubUserFileClient File = new PubUserFile.PubUserFileClient(Invoker);

            //FileListRequest fileListRequest = new FileListRequest();
            //fileListRequest.Parent = new V6.Services.Pub.File();
            //fileListRequest.Parent.Path = "/复制的文件";
            //fileListRequest.Filter = new V6.Services.Pub.File();
            //fileListRequest.Filter.Deleted = true;
            ////fileListRequest.Filter.Dir = true;
            //fileListRequest.Filter.Hidden = true;
            //fileListRequest.Filter.Locked = true;
            //fileListRequest.Filter.Shared = true;
            //fileListRequest.Filter.Starred = true;
            //fileListRequest.Filter.Trashed = true;
            ////fileListRequest.ListInfo = new V6.Services.Pub.Common.ScanListRequest();
            ////fileListRequest.ListInfo.Limit = 2000;

            //var x = File.List(fileListRequest);

            Console.WriteLine("Hello, World!");
            Console.ReadKey();
        }
    }
}
