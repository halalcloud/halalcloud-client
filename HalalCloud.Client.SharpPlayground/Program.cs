using HalalCloud.Client.Core;
using System.Diagnostics;
using V6.Services.Pub;
using static V6.Services.Pub.PubUserFile;

namespace HalalCloud.Client.SharpPlayground
{
    internal class Program
    {
        static void Main(string[] args)
        {
            SessionManager Session = new SessionManager();

            //var y = Session.LoginWithRefreshToken(
            //    @"rt__1fc4496396704771945da06329c329ea_c4b3676b-3339-4c8f-8300-6f24423a79ce");
            //Session.AccessToken = y.AccessToken;

            //Session.Logout(y.RefreshToken);

            OauthTokenResponse Response = Session.CreateAuthToken();
            Console.WriteLine(Response.ReturnUrl);
            Process.Start(new ProcessStartInfo
            {
                FileName = Response.ReturnUrl,
                UseShellExecute = true
            });

            Console.Write("Waiting");
            for (; ; )
            {
                OauthTokenCheckResponse CheckResponse = Session.VerifyAuthToken(
                    Response.Callback);
                if (CheckResponse.Status == 6)
                {
                    Console.WriteLine(".");
                    Session.AccessToken = CheckResponse.Login.Token.AccessToken;
                    Console.WriteLine(Session.AccessToken);
                    break;
                }

                Console.Write(".");
                Thread.Sleep(200);
            }

            //Session.CreateDirectory("/", "Folder2");

            //Session.UploadFile(
            //    @"C:\Users\mouri\Downloads\RISC-V Manual\unpriv-isa-asciidoc.html",
            //    "/",
            //    "unpriv-isa-asciidoc.html");

            PubUserFileClient File = new PubUserFileClient(Session.Invoker);

            FileListRequest fileListRequest = new FileListRequest();
            fileListRequest.Parent = new V6.Services.Pub.File();
            fileListRequest.Parent.Path = "/";//"/复制的文件";
            //fileListRequest.Filter = new V6.Services.Pub.File();
            //fileListRequest.Filter.Deleted = true;
            //fileListRequest.Filter.Dir = true;
            //fileListRequest.Filter.Hidden = true;
            //fileListRequest.Filter.Locked = true;
            //fileListRequest.Filter.Shared = true;
            //fileListRequest.Filter.Starred = true;
            //fileListRequest.Filter.Trashed = true;
            fileListRequest.ListInfo = new V6.Services.Pub.Common.ScanListRequest();
            fileListRequest.ListInfo.Limit = 2000;

            var x = File.List(fileListRequest);

            foreach (var item in x.Files)
            {
                Console.WriteLine(
                    "{0}\t{1}\t{2}\t{3}",
                    DateTimeOffset.FromUnixTimeMilliseconds(item.CreateTs),
                    item.Dir ? "<DIR>" : string.Empty,
                    0 != item.Size ? item.Size.ToString() : string.Empty,
                    item.Name);
            }

            var y = x.Files.First().Type;

            Console.WriteLine("Hello, World!");
            Console.ReadKey();
        }
    }
}
