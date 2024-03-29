﻿using HalalCloud.Client.Core;
using V6.Services.Pub;

namespace HalalCloud.Client.SharpPlayground
{
    internal class Program
    {
        static void Main(string[] args)
        {
            SessionManager Session = new SessionManager();

            OauthTokenResponse Response = Session.CreateAuthToken();
            Console.WriteLine(Response.ReturnUrl);

            for (; ; )
            {
                OauthTokenCheckResponse CheckResponse = Session.VerifyAuthToken(
                    Response.Callback);
                if (CheckResponse.Status == 6)
                {
                    string AccessToken = CheckResponse.Login.Token.AccessToken;
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
