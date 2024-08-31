using Google.Protobuf;
using Grpc.Core;
using Grpc.Core.Interceptors;
using Grpc.Net.Client;
using V6.Services.Pub;

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
                    JsonContext.Marshallsers["StartDownloadRequest"],
                    JsonContext.Marshallsers["StartDownloadResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.p3p.WebRTCManageService",
                    "StopDownload",
                    JsonContext.Marshallsers["StopDownloadRequest"],
                    JsonContext.Marshallsers["StopDownloadResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.p4p.WebRTCManageService",
                    "Disconnect",
                    JsonContext.Marshallsers["DisconnectRequest"],
                    JsonContext.Marshallsers["DisconnectResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.p5p.WebRTCManageService",
                    "SendPeerIceCandidate",
                    JsonContext.Marshallsers["SendPeerIceCandidateRequest"],
                    JsonContext.Marshallsers["SendPeerIceCandidateResponse"]),
                new Method<string, string>(
                    MethodType.ServerStreaming,
                    "v6.services.p6p.WebRTCManageService",
                    "Receive",
                    JsonContext.Marshallsers["ReceiveRequest"],
                    JsonContext.Marshallsers["ReceiveResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubOfflineTask",
                    "Parse",
                    JsonContext.Marshallsers["TaskParseRequest"],
                    JsonContext.Marshallsers["TaskParseResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubOfflineTask",
                    "Add",
                    JsonContext.Marshallsers["UserTask"],
                    JsonContext.Marshallsers["UserTask"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubOfflineTask",
                    "List",
                    JsonContext.Marshallsers["OfflineTaskListRequest"],
                    JsonContext.Marshallsers["OfflineTaskListResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubOfflineTask",
                    "Delete",
                    JsonContext.Marshallsers["OfflineTaskDeleteRequest"],
                    JsonContext.Marshallsers["OfflineTaskDeleteResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSftpConfig",
                    "Get",
                    JsonContext.Marshallsers["SftpConfig"],
                    JsonContext.Marshallsers["SftpConfig"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSftpConfig",
                    "Update",
                    JsonContext.Marshallsers["SftpConfig"],
                    JsonContext.Marshallsers["SftpConfig"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSftpConfig",
                    "Enable",
                    JsonContext.Marshallsers["SftpConfig"],
                    JsonContext.Marshallsers["SftpConfig"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSftpConfig",
                    "Disable",
                    JsonContext.Marshallsers["SftpConfig"],
                    JsonContext.Marshallsers["SftpConfig"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSftpConfig",
                    "UpdateKeys",
                    JsonContext.Marshallsers["SftpConfig"],
                    JsonContext.Marshallsers["SftpConfig"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSftpConfig",
                    "ValidateUserName",
                    JsonContext.Marshallsers["SftpConfig"],
                    JsonContext.Marshallsers["UserNameValidateResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "Create",
                    JsonContext.Marshallsers["FileShare"],
                    JsonContext.Marshallsers["FileShare"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "Get",
                    JsonContext.Marshallsers["FileShare"],
                    JsonContext.Marshallsers["FileShare"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "Like",
                    JsonContext.Marshallsers["FileShare"],
                    JsonContext.Marshallsers["FileShare"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "Dislike",
                    JsonContext.Marshallsers["FileShare"],
                    JsonContext.Marshallsers["FileShare"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "ComplaintShare",
                    JsonContext.Marshallsers["Complaint"],
                    JsonContext.Marshallsers["Complaint"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "List",
                    JsonContext.Marshallsers["FileShareListRequest"],
                    JsonContext.Marshallsers["FileShareListResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "ListComplaint",
                    JsonContext.Marshallsers["ComplaintListRequest"],
                    JsonContext.Marshallsers["ComplaintListResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "Delete",
                    JsonContext.Marshallsers["FileShareDeleteRequest"],
                    JsonContext.Marshallsers["FileShareDeleteResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "DeleteComplaint",
                    JsonContext.Marshallsers["ComplaintDeleteRequest"],
                    JsonContext.Marshallsers["ComplaintDeleteResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubFileShare",
                    "Save",
                    JsonContext.Marshallsers["FileShare"],
                    JsonContext.Marshallsers["FileShare"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSlice",
                    "GetSlice",
                    JsonContext.Marshallsers["Slice"],
                    JsonContext.Marshallsers["Slice"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSlice",
                    "CreateSlice",
                    JsonContext.Marshallsers["Slice"],
                    JsonContext.Marshallsers["Slice"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSlice",
                    "GetMeta",
                    JsonContext.Marshallsers["Meta"],
                    JsonContext.Marshallsers["Meta"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSlice",
                    "CreateMeta",
                    JsonContext.Marshallsers["Meta"],
                    JsonContext.Marshallsers["Meta"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSlice",
                    "AddFastLookup",
                    JsonContext.Marshallsers["FastLookup"],
                    JsonContext.Marshallsers["FastLookup"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubSlice",
                    "GetFastLookup",
                    JsonContext.Marshallsers["FastLookupRequest"],
                    JsonContext.Marshallsers["FastLookup"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "Get",
                    JsonContext.Marshallsers["User"],
                    JsonContext.Marshallsers["User"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "Login",
                    JsonContext.Marshallsers["LoginRequest"],
                    JsonContext.Marshallsers["LoginResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "Refresh",
                    JsonContext.Marshallsers["Token"],
                    JsonContext.Marshallsers["Token"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "Logoff",
                    JsonContext.Marshallsers["Token"],
                    JsonContext.Marshallsers["User"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "ResetPassword",
                    JsonContext.Marshallsers["LoginRequest"],
                    JsonContext.Marshallsers["User"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "ChangePassword",
                    JsonContext.Marshallsers["ChangePasswordRequest"],
                    JsonContext.Marshallsers["User"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "Register",
                    JsonContext.Marshallsers["RegisterRequest"],
                    JsonContext.Marshallsers["User"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "Delete",
                    JsonContext.Marshallsers["User"],
                    JsonContext.Marshallsers["User"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "Update",
                    JsonContext.Marshallsers["User"],
                    JsonContext.Marshallsers["User"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "SendSmsVerifyCode",
                    JsonContext.Marshallsers["SmsVeifyCodeSendRequest"],
                    JsonContext.Marshallsers["SmsVeifyCodeSendResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "SendSmsVerifyCodeNotUser",
                    JsonContext.Marshallsers["SmsVeifyCodeSendRequestNotUser"],
                    JsonContext.Marshallsers["SmsVeifyCodeSendResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "VerifyAuthToken",
                    JsonContext.Marshallsers["LoginRequest"],
                    JsonContext.Marshallsers["OauthTokenCheckResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "CreateAuthToken",
                    JsonContext.Marshallsers["LoginRequest"],
                    JsonContext.Marshallsers["OauthTokenResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUser",
                    "ValidateUserInfo",
                    JsonContext.Marshallsers["UserValidateInfo"],
                    JsonContext.Marshallsers["UserNameValidateResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Create",
                    JsonContext.Marshallsers["File"],
                    JsonContext.Marshallsers["File"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Get",
                    JsonContext.Marshallsers["File"],
                    JsonContext.Marshallsers["File"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Rename",
                    JsonContext.Marshallsers["File"],
                    JsonContext.Marshallsers["BatchOperationResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Trash",
                    JsonContext.Marshallsers["BatchOperationRequest"],
                    JsonContext.Marshallsers["BatchOperationResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Move",
                    JsonContext.Marshallsers["BatchOperationRequest"],
                    JsonContext.Marshallsers["BatchOperationResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Copy",
                    JsonContext.Marshallsers["BatchOperationRequest"],
                    JsonContext.Marshallsers["BatchOperationResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Delete",
                    JsonContext.Marshallsers["BatchOperationRequest"],
                    JsonContext.Marshallsers["BatchOperationResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "DeleteTrash",
                    JsonContext.Marshallsers["BatchOperationRequest"],
                    JsonContext.Marshallsers["BatchOperationResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Recover",
                    JsonContext.Marshallsers["BatchOperationRequest"],
                    JsonContext.Marshallsers["BatchOperationResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "BatchOperation",
                    JsonContext.Marshallsers["BatchOperationRequest"],
                    JsonContext.Marshallsers["BatchOperationResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "List",
                    JsonContext.Marshallsers["FileListRequest"],
                    JsonContext.Marshallsers["FileListResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "ListTrash",
                    JsonContext.Marshallsers["FileListRequest"],
                    JsonContext.Marshallsers["FileListResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "Search",
                    JsonContext.Marshallsers["SearchRequest"],
                    JsonContext.Marshallsers["FileListResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "CreateManageRTCOffer",
                    JsonContext.Marshallsers["ManageRTCRequest"],
                    JsonContext.Marshallsers["ManageRTCResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "SendClientIceCandidate",
                    JsonContext.Marshallsers["SendIceCandidateRequest"],
                    JsonContext.Marshallsers["SendIceCandidateResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "GetServerIceCandidate",
                    JsonContext.Marshallsers["GetIceCandidateRequest"],
                    JsonContext.Marshallsers["GetIceCandidateResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "ParseFileSlice",
                    JsonContext.Marshallsers["File"],
                    JsonContext.Marshallsers["ParseFileSliceResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "GetSliceDownloadAddress",
                    JsonContext.Marshallsers["SliceDownloadAddressRequest"],
                    JsonContext.Marshallsers["SliceDownloadAddressResponse"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "GetDownloadAndPreviewInfo",
                    JsonContext.Marshallsers["File"],
                    JsonContext.Marshallsers["DownloadAndPreviewInfo"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "PreviewDoc",
                    JsonContext.Marshallsers["File"],
                    JsonContext.Marshallsers["DocFilePreview"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "CreateDoc",
                    JsonContext.Marshallsers["File"],
                    JsonContext.Marshallsers["DocFilePreview"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubUserFile",
                    "CreateUploadToken",
                    JsonContext.Marshallsers["File"],
                    JsonContext.Marshallsers["UploadToken"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubDavConfig",
                    "Get",
                    JsonContext.Marshallsers["DavConfig"],
                    JsonContext.Marshallsers["DavConfig"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubDavConfig",
                    "Update",
                    JsonContext.Marshallsers["DavConfig"],
                    JsonContext.Marshallsers["DavConfig"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubDavConfig",
                    "Enable",
                    JsonContext.Marshallsers["DavConfig"],
                    JsonContext.Marshallsers["DavConfig"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubDavConfig",
                    "Disable",
                    JsonContext.Marshallsers["DavConfig"],
                    JsonContext.Marshallsers["DavConfig"]),
                new Method<string, string>(
                    MethodType.Unary,
                    "v6.services.pub.PubDavConfig",
                    "ValidateUserName",
                    JsonContext.Marshallsers["DavConfig"],
                    JsonContext.Marshallsers["UserNameValidateResponse"])
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
            if (string.IsNullOrWhiteSpace(AccessToken))
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

        private static void SerializeMessageHelper(
            IMessage Message,
            SerializationContext Context)
        {
            Context.Complete(MessageExtensions.ToByteArray(Message));
        }

        private static IMessage DeserializeMessageHelper(
            DeserializationContext Context, MessageParser Parser)
        {
            return Parser.ParseFrom(Context.PayloadAsNewBuffer());
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
