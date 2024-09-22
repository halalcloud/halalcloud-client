using Grpc.Core;
using Grpc.Core.Interceptors;
using Grpc.Net.Client;

namespace HalalCloud.RpcClient
{
    public class Session : ClientInterceptor
    {
        private string RpcServerUrl = "https://grpcuserapi.2dland.cn";
        private string RpcClientApplicationId = "devDebugger/1.0";
        private string RpcClientApplicationVersion = "1.0.0";
        private string RpcClientApplicationSecret = "Nkx3Y2xvZ2luLmNu";

        private GrpcChannel? RpcChannel = null;
        private CallInvoker? RpcInvoker = null;

        private static readonly List<Method<string, string>> Methods =
            new List<Method<string, string>>()
            {
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.p2p.WebRTCManageService",
                    "StartDownload",
                    JsonContext.StartDownloadRequestMarshaller,
                    JsonContext.StartDownloadResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.p3p.WebRTCManageService",
                    "StopDownload",
                    JsonContext.StopDownloadRequestMarshaller,
                    JsonContext.StopDownloadResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.p4p.WebRTCManageService",
                    "Disconnect",
                    JsonContext.DisconnectRequestMarshaller,
                    JsonContext.DisconnectResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.p5p.WebRTCManageService",
                    "SendPeerIceCandidate",
                    JsonContext.SendPeerIceCandidateRequestMarshaller,
                    JsonContext.SendPeerIceCandidateResponseMarshaller),
                new Method<string, string>(
                    MethodType.ServerStreaming,
                    "v6.services.p6p.WebRTCManageService",
                    "Receive",
                    JsonContext.ReceiveRequestMarshaller,
                    JsonContext.ReceiveResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubOfflineTask",
                    "Parse",
                    JsonContext.TaskParseRequestMarshaller,
                    JsonContext.TaskParseResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubOfflineTask",
                    "Add",
                    JsonContext.UserTaskMarshaller,
                    JsonContext.UserTaskMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubOfflineTask",
                    "List",
                    JsonContext.OfflineTaskListRequestMarshaller,
                    JsonContext.OfflineTaskListResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubOfflineTask",
                    "Delete",
                    JsonContext.OfflineTaskDeleteRequestMarshaller,
                    JsonContext.OfflineTaskDeleteResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSftpConfig",
                    "Get",
                    JsonContext.SftpConfigMarshaller,
                    JsonContext.SftpConfigMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSftpConfig",
                    "Update",
                    JsonContext.SftpConfigMarshaller,
                    JsonContext.SftpConfigMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSftpConfig",
                    "Enable",
                    JsonContext.SftpConfigMarshaller,
                    JsonContext.SftpConfigMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSftpConfig",
                    "Disable",
                    JsonContext.SftpConfigMarshaller,
                    JsonContext.SftpConfigMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSftpConfig",
                    "UpdateKeys",
                    JsonContext.SftpConfigMarshaller,
                    JsonContext.SftpConfigMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSftpConfig",
                    "ValidateUserName",
                    JsonContext.SftpConfigMarshaller,
                    JsonContext.UserNameValidateResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "Create",
                    JsonContext.FileShareMarshaller,
                    JsonContext.FileShareMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "Get",
                    JsonContext.FileShareMarshaller,
                    JsonContext.FileShareMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "Like",
                    JsonContext.FileShareMarshaller,
                    JsonContext.FileShareMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "Dislike",
                    JsonContext.FileShareMarshaller,
                    JsonContext.FileShareMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "ComplaintShare",
                    JsonContext.ComplaintMarshaller,
                    JsonContext.ComplaintMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "List",
                    JsonContext.FileShareListRequestMarshaller,
                    JsonContext.FileShareListResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "ListComplaint",
                    JsonContext.ComplaintListRequestMarshaller,
                    JsonContext.ComplaintListResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "Delete",
                    JsonContext.FileShareDeleteRequestMarshaller,
                    JsonContext.FileShareDeleteResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "DeleteComplaint",
                    JsonContext.ComplaintDeleteRequestMarshaller,
                    JsonContext.ComplaintDeleteResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "Save",
                    JsonContext.FileShareMarshaller,
                    JsonContext.FileShareMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSlice",
                    "GetSlice",
                    JsonContext.SliceMarshaller,
                    JsonContext.SliceMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSlice",
                    "CreateSlice",
                    JsonContext.SliceMarshaller,
                    JsonContext.SliceMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSlice",
                    "GetMeta",
                    JsonContext.MetaMarshaller,
                    JsonContext.MetaMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSlice",
                    "CreateMeta",
                    JsonContext.MetaMarshaller,
                    JsonContext.MetaMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSlice",
                    "AddFastLookup",
                    JsonContext.FastLookupMarshaller,
                    JsonContext.FastLookupMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSlice",
                    "GetFastLookup",
                    JsonContext.FastLookupRequestMarshaller,
                    JsonContext.FastLookupMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "Get",
                    JsonContext.UserMarshaller,
                    JsonContext.UserMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "Login",
                    JsonContext.LoginRequestMarshaller,
                    JsonContext.LoginResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "Refresh",
                    JsonContext.TokenMarshaller,
                    JsonContext.TokenMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "Logoff",
                    JsonContext.TokenMarshaller,
                    JsonContext.UserMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "ResetPassword",
                    JsonContext.LoginRequestMarshaller,
                    JsonContext.UserMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "ChangePassword",
                    JsonContext.ChangePasswordRequestMarshaller,
                    JsonContext.UserMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "Register",
                    JsonContext.RegisterRequestMarshaller,
                    JsonContext.UserMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "Delete",
                    JsonContext.UserMarshaller,
                    JsonContext.UserMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "Update",
                    JsonContext.UserMarshaller,
                    JsonContext.UserMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "SendSmsVerifyCode",
                    JsonContext.SmsVeifyCodeSendRequestMarshaller,
                    JsonContext.SmsVeifyCodeSendResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "SendSmsVerifyCodeNotUser",
                    JsonContext.SmsVeifyCodeSendRequestNotUserMarshaller,
                    JsonContext.SmsVeifyCodeSendResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "VerifyAuthToken",
                    JsonContext.LoginRequestMarshaller,
                    JsonContext.OauthTokenCheckResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "CreateAuthToken",
                    JsonContext.LoginRequestMarshaller,
                    JsonContext.OauthTokenResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "ValidateUserInfo",
                    JsonContext.UserValidateInfoMarshaller,
                    JsonContext.UserNameValidateResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Create",
                    JsonContext.FileMarshaller,
                    JsonContext.FileMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Get",
                    JsonContext.FileMarshaller,
                    JsonContext.FileMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Rename",
                    JsonContext.FileMarshaller,
                    JsonContext.BatchOperationResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Trash",
                    JsonContext.BatchOperationRequestMarshaller,
                    JsonContext.BatchOperationResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Move",
                    JsonContext.BatchOperationRequestMarshaller,
                    JsonContext.BatchOperationResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Copy",
                    JsonContext.BatchOperationRequestMarshaller,
                    JsonContext.BatchOperationResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Delete",
                    JsonContext.BatchOperationRequestMarshaller,
                    JsonContext.BatchOperationResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "DeleteTrash",
                    JsonContext.BatchOperationRequestMarshaller,
                    JsonContext.BatchOperationResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Recover",
                    JsonContext.BatchOperationRequestMarshaller,
                    JsonContext.BatchOperationResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "BatchOperation",
                    JsonContext.BatchOperationRequestMarshaller,
                    JsonContext.BatchOperationResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "List",
                    JsonContext.FileListRequestMarshaller,
                    JsonContext.FileListResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "ListTrash",
                    JsonContext.FileListRequestMarshaller,
                    JsonContext.FileListResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Search",
                    JsonContext.SearchRequestMarshaller,
                    JsonContext.FileListResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "CreateManageRTCOffer",
                    JsonContext.ManageRTCRequestMarshaller,
                    JsonContext.ManageRTCResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "SendClientIceCandidate",
                    JsonContext.SendIceCandidateRequestMarshaller,
                    JsonContext.SendIceCandidateResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "GetServerIceCandidate",
                    JsonContext.GetIceCandidateRequestMarshaller,
                    JsonContext.GetIceCandidateResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "ParseFileSlice",
                    JsonContext.FileMarshaller,
                    JsonContext.ParseFileSliceResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "GetSliceDownloadAddress",
                    JsonContext.SliceDownloadAddressRequestMarshaller,
                    JsonContext.SliceDownloadAddressResponseMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "GetDownloadAndPreviewInfo",
                    JsonContext.FileMarshaller,
                    JsonContext.DownloadAndPreviewInfoMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "PreviewDoc",
                    JsonContext.FileMarshaller,
                    JsonContext.DocFilePreviewMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "CreateDoc",
                    JsonContext.FileMarshaller,
                    JsonContext.DocFilePreviewMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "CreateUploadToken",
                    JsonContext.FileMarshaller,
                    JsonContext.UploadTokenMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubDavConfig",
                    "Get",
                    JsonContext.DavConfigMarshaller,
                    JsonContext.DavConfigMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubDavConfig",
                    "Update",
                    JsonContext.DavConfigMarshaller,
                    JsonContext.DavConfigMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubDavConfig",
                    "Enable",
                    JsonContext.DavConfigMarshaller,
                    JsonContext.DavConfigMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubDavConfig",
                    "Disable",
                    JsonContext.DavConfigMarshaller,
                    JsonContext.DavConfigMarshaller),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubDavConfig",
                    "ValidateUserName",
                    JsonContext.DavConfigMarshaller,
                    JsonContext.UserNameValidateResponseMarshaller)
            };

        public string AccessToken { get; set; } = string.Empty;

        public Session()
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
            if (!string.IsNullOrWhiteSpace(AccessToken))
            {
                Authorization = "Bearer " + AccessToken;
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

        public string Request(
            string MethodFullName,
            string RequestJson)
        {
            return RpcInvoker!.BlockingUnaryCall(
                Methods.Find(Current => Current.FullName == MethodFullName)!,
                null,
                new CallOptions(),
                RequestJson);
        }
    }
}
