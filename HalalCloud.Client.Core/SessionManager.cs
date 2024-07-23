﻿using BaiduBce.Auth;
using BaiduBce.Services.Bos.Model;
using BaiduBce.Services.Bos;
using BaiduBce;
using Grpc.Core;
using Grpc.Core.Interceptors;
using Grpc.Net.Client;
using V6.Services.Pub;
using static V6.Services.Pub.PubUserFile;

namespace HalalCloud.Client.Core
{
    public class SessionManager : ClientInterceptor
    {
        private string RpcServerUrl = "https://grpcuserapi.2dland.cn";
        private string RpcClientApplicationId = "devDebugger/1.0";
        private string RpcClientApplicationVersion = "1.0.0";
        private string RpcClientApplicationSecret = "Nkx3Y2xvZ2luLmNu";

        private GrpcChannel? RpcChannel = null;
        private CallInvoker? RpcInvoker = null;

        public Token? AccessToken { get; set; } = null;

        public SessionManager()
        {
            RpcChannel = GrpcChannel.ForAddress(RpcServerUrl);
            RpcInvoker = RpcChannel.Intercept(this);
        }

        public override ClientInterceptorContext<TRequest, TResponse> ContextHandler<TRequest, TResponse>(
            ClientInterceptorContext<TRequest, TResponse> context)
            where TRequest : class
            where TResponse : class
        {
            var metadata = context.Options.Headers ?? new Metadata();

            long TimeStamp = DateTimeOffset.Now.ToUnixTimeMilliseconds();

            metadata.Add("timestamp", TimeStamp.ToString());
            metadata.Add("appid", RpcClientApplicationId);
            metadata.Add("appversion", RpcClientApplicationVersion);

            string Authorization = string.Empty;
            if (AccessToken != null &&
                !string.IsNullOrWhiteSpace(AccessToken.AccessToken))
            {
                Authorization = "Bearer " + AccessToken.AccessToken;
            }
            if (!string.IsNullOrWhiteSpace(Authorization))
            {
                metadata.Add("authorization", Authorization);
            }

            metadata.Add(
                "sign",
                Utilities.ComputeSignature(
                    RpcClientApplicationId,
                    RpcClientApplicationVersion,
                    RpcClientApplicationSecret,
                    Authorization,
                    context.Method.FullName,
                    TimeStamp).ToLower());

            var deadline = DateTime.UtcNow.AddSeconds(5);

            return new ClientInterceptorContext<TRequest, TResponse>(
                context.Method,
                context.Host,
                context.Options.WithHeaders(metadata).WithDeadline(deadline));
        }

        public delegate void AuthenticationNotifyCallback(
            string AuthenticationUri);

        public void Authenticate(
            AuthenticationNotifyCallback Callback)
        {
            PubUser.PubUserClient Client =
                new PubUser.PubUserClient(RpcInvoker);

            LoginRequest Request = new LoginRequest();
            Request.ReturnType = 2;
            OauthTokenResponse CreateResponse = Client.CreateAuthToken(Request);

            Callback(CreateResponse.ReturnUrl);

            Request.Type = "2";
            Request.Callback = CreateResponse.Callback;

            while (true)
            {
                OauthTokenCheckResponse VerifyResponse =
                    Client.VerifyAuthToken(Request);
                if (VerifyResponse.Status == 6)
                {
                    AccessToken = VerifyResponse.Login.Token;
                    return;
                }

                Thread.Sleep(200);
            }
        }

        public void Impersonate(
            string RefreshToken)
        {
            PubUser.PubUserClient Client =
               new PubUser.PubUserClient(RpcInvoker);
            Token Request = new Token();
            Request.RefreshToken = RefreshToken;
            AccessToken = Client.Refresh(Request);
        }

        public void Logout()
        {
            if (AccessToken != null)
            {
                PubUser.PubUserClient Client =
                    new PubUser.PubUserClient(RpcInvoker);
                Token Request = AccessToken;
                Client.Logoff(AccessToken);
                AccessToken = null;
            }
        }

        public V6.Services.Pub.User GetUserInformation()
        {
            PubUser.PubUserClient Client =
                new PubUser.PubUserClient(RpcInvoker);
            V6.Services.Pub.User Request = new V6.Services.Pub.User();
            return Client.Get(Request);
        }

        public CallInvoker Invoker
        {
            get
            {
                if (RpcInvoker == null)
                {
                    throw new InvalidOperationException();
                }
                return RpcInvoker;
            }
        }

        public V6.Services.Pub.File CreateDirectory(
            string Path,
            string Name)
        {
            PubUserFileClient Client = new PubUserFileClient(RpcInvoker);
            V6.Services.Pub.File Request = new V6.Services.Pub.File();
            Request.Path = Path;
            Request.Name = Name;
            return Client.Create(Request);
        }

        public void UploadFile(
            string SourceFilePath,
            string TargetDirectoryPath,
            string TargetFileName)
        {
            PubUserFileClient RpcClient = new PubUserFileClient(RpcInvoker);
            V6.Services.Pub.File RpcRequest = new V6.Services.Pub.File();
            RpcRequest.Path = TargetDirectoryPath;
            RpcRequest.Name = TargetFileName;
            UploadToken RpcResponse = RpcClient.CreateUploadToken(RpcRequest);

            BceClientConfiguration UploadConfiguration =
                new BceClientConfiguration();
            UploadConfiguration.Credentials =
                new DefaultBceSessionCredentials(
                    RpcResponse.AccessKey,
                    RpcResponse.SecretKey,
                    RpcResponse.Token);
            UploadConfiguration.Endpoint = RpcResponse.Endpoint;
            BosClient UploadClient = new BosClient(UploadConfiguration);

            FileInfo UploadFile = new FileInfo(SourceFilePath);
            PutObjectResponse UploadResponse = UploadClient.PutObject(
                RpcResponse.Bucket,
                RpcResponse.Key,
                UploadFile);
        }

        public List<FileInformation> EnumerateFiles(
            string Path)
        {
            PubUserFileClient Client = new PubUserFileClient(RpcInvoker);

            FileListRequest Request = new FileListRequest();
            Request.Parent = new V6.Services.Pub.File();
            Request.Parent.Path = Path;

            List<FileInformation> Result = new List<FileInformation>();

            string NextToken = string.Empty;

            do
            {
                FileListResponse Response = Client.List(Request);
                NextToken = Response.ListInfo.Token;
                foreach (var Item in Response.Files)
                {
                    FileInformation Information = new FileInformation();
                    Information.CreationTime =
                        DateTimeOffset.FromUnixTimeMilliseconds(
                            Item.CreateTs).DateTime;
                    Information.LastWriteTime =
                        DateTimeOffset.FromUnixTimeMilliseconds(
                            Item.UpdateTs).DateTime;
                    Information.FileSize = Convert.ToUInt64(Item.Size);
                    if (Item.Dir)
                    {
                        Information.FileAttributes |= FileAttributes.Directory;
                    }
                    if (Item.Hidden)
                    {
                        Information.FileAttributes |= FileAttributes.Hidden;
                    }
                    Information.FileName = Item.Name;
                    Result.Add(Information);
                }

            } while (!string.IsNullOrEmpty(NextToken));

            return Result;
        }
    }
}
