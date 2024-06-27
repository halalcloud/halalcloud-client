using BaiduBce.Auth;
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
    public class SessionManager
    {
        private string RpcServerUrl = "https://grpcuserapi.2dland.cn";
        private GrpcChannel? RpcChannel = null;
        private SignatureInterceptor? RpcInterceptor = null;
        private CallInvoker? RpcInvoker = null;

        public SessionManager()
        {
            RpcChannel = GrpcChannel.ForAddress(RpcServerUrl);
            RpcInterceptor = new SignatureInterceptor();
            RpcInvoker = RpcChannel.Intercept(RpcInterceptor);
        }

        public OauthTokenResponse CreateAuthToken()
        {
            PubUser.PubUserClient Client=
                new PubUser.PubUserClient(RpcInvoker);
            LoginRequest Request = new LoginRequest();
            Request.ReturnType = 2;
            return Client.CreateAuthToken(Request);
        }

        public OauthTokenCheckResponse VerifyAuthToken(
            string Callback)
        {
            PubUser.PubUserClient Client =
               new PubUser.PubUserClient(RpcInvoker);
            LoginRequest Request = new LoginRequest();
            Request.Type = "2";
            Request.ReturnType = 2;
            Request.Callback = Callback;
            return Client.VerifyAuthToken(Request);
        }

        public Token LoginWithRefreshToken(
            string RefreshToken)
        {
            PubUser.PubUserClient Client =
               new PubUser.PubUserClient(RpcInvoker);
            Token Request = new Token();
            Request.RefreshToken = RefreshToken;
            return Client.Refresh(Request);
        }

        public void Logout(
            string RefreshToken)
        {
            PubUser.PubUserClient Client =
               new PubUser.PubUserClient(RpcInvoker);
            Token Request = new Token();
            Request.RefreshToken = RefreshToken;
            Client.Logoff(Request);
        }

        public string AccessToken
        {
            get
            {
                if (RpcInterceptor == null)
                {
                    throw new InvalidOperationException();
                }
                return RpcInterceptor.AccessToken;
            }
            set
            {
                if (RpcInterceptor == null)
                {
                    throw new InvalidOperationException();
                }
                RpcInterceptor.AccessToken = value;
            }
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
