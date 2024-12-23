using Google.Protobuf;
using Grpc.Core;
using System.Text.Json;
using System.Text.Json.Serialization;
using V6.Rpc;
using V6.Services.P2P;
using V6.Services.Pub;
using V6.Services.Pub.Common;

namespace HalalCloud.RpcClient
{
    [JsonSourceGenerationOptions(
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
        PropertyNamingPolicy = JsonKnownNamingPolicy.SnakeCaseLower,
        UnmappedMemberHandling = JsonUnmappedMemberHandling.Skip,
        PreferredObjectCreationHandling = JsonObjectCreationHandling.Populate)]
    [JsonSerializable(typeof(BatchOperationRequest))]
    [JsonSerializable(typeof(BatchOperationResponse))]
    [JsonSerializable(typeof(ChangePasswordRequest))]
    [JsonSerializable(typeof(Complaint))]
    [JsonSerializable(typeof(ComplaintDeleteRequest))]
    [JsonSerializable(typeof(ComplaintDeleteResponse))]
    [JsonSerializable(typeof(ComplaintListRequest))]
    [JsonSerializable(typeof(ComplaintListResponse))]
    [JsonSerializable(typeof(DavConfig))]
    [JsonSerializable(typeof(DisconnectRequest))]
    [JsonSerializable(typeof(DisconnectResponse))]
    [JsonSerializable(typeof(DiskStatisticsAndQuota))]
    [JsonSerializable(typeof(DocFilePreview))]
    [JsonSerializable(typeof(DownloadAndPreviewInfo))]
    [JsonSerializable(typeof(FastLookup))]
    [JsonSerializable(typeof(FastLookupRequest))]
    [JsonSerializable(typeof(V6.Services.Pub.File))]
    [JsonSerializable(typeof(FileListRequest))]
    [JsonSerializable(typeof(FileListResponse))]
    [JsonSerializable(typeof(V6.Services.Pub.FileShare))]
    [JsonSerializable(typeof(FileShareDeleteRequest))]
    [JsonSerializable(typeof(FileShareDeleteResponse))]
    [JsonSerializable(typeof(FileShareListRequest))]
    [JsonSerializable(typeof(FileShareListResponse))]
    [JsonSerializable(typeof(GetIceCandidateRequest))]
    [JsonSerializable(typeof(GetIceCandidateResponse))]
    [JsonSerializable(typeof(LastLoginResponse))]
    [JsonSerializable(typeof(LoginRequest))]
    [JsonSerializable(typeof(LoginResponse))]
    [JsonSerializable(typeof(ManageRTCRequest))]
    [JsonSerializable(typeof(ManageRTCResponse))]
    [JsonSerializable(typeof(Meta))]
    [JsonSerializable(typeof(OauthTokenCheckResponse))]
    [JsonSerializable(typeof(OauthTokenResponse))]
    [JsonSerializable(typeof(OfflineTaskDeleteRequest))]
    [JsonSerializable(typeof(OfflineTaskDeleteResponse))]
    [JsonSerializable(typeof(OfflineTaskListRequest))]
    [JsonSerializable(typeof(OfflineTaskListResponse))]
    [JsonSerializable(typeof(OfflineTaskStatisticsAndQuota))]
    [JsonSerializable(typeof(OrderByInfo))]
    [JsonSerializable(typeof(ParseFileSliceResponse))]
    [JsonSerializable(typeof(ReceiveRequest))]
    [JsonSerializable(typeof(ReceiveResponse))]
    [JsonSerializable(typeof(RegisterRequest))]
    [JsonSerializable(typeof(RpcHeaderValue))]
    [JsonSerializable(typeof(RpcMetaData))]
    [JsonSerializable(typeof(RpcTransport))]
    [JsonSerializable(typeof(RtcPeerInfo))]
    [JsonSerializable(typeof(ScanListRequest))]
    [JsonSerializable(typeof(SearchRequest))]
    [JsonSerializable(typeof(SendIceCandidateRequest))]
    [JsonSerializable(typeof(SendIceCandidateResponse))]
    [JsonSerializable(typeof(SendPeerIceCandidateRequest))]
    [JsonSerializable(typeof(SendPeerIceCandidateResponse))]
    [JsonSerializable(typeof(SftpConfig))]
    [JsonSerializable(typeof(Slice))]
    [JsonSerializable(typeof(SliceDownloadAddressRequest))]
    [JsonSerializable(typeof(SliceDownloadAddressResponse))]
    [JsonSerializable(typeof(SliceDownloadInfo))]
    [JsonSerializable(typeof(SliceSize))]
    [JsonSerializable(typeof(SmsVeifyCodeSendRequest))]
    [JsonSerializable(typeof(SmsVeifyCodeSendRequestNotUser))]
    [JsonSerializable(typeof(SmsVeifyCodeSendResponse))]
    [JsonSerializable(typeof(StartDownloadRequest))]
    [JsonSerializable(typeof(StartDownloadResponse))]
    [JsonSerializable(typeof(StopDownloadRequest))]
    [JsonSerializable(typeof(StopDownloadResponse))]
    [JsonSerializable(typeof(TaskFile))]
    [JsonSerializable(typeof(TaskMeta))]
    [JsonSerializable(typeof(TaskParseRequest))]
    [JsonSerializable(typeof(TaskParseResponse))]
    [JsonSerializable(typeof(Token))]
    [JsonSerializable(typeof(TrafficStatisticsAndQuota))]
    [JsonSerializable(typeof(UploadToken))]
    [JsonSerializable(typeof(User))]
    [JsonSerializable(typeof(UserNameValidateResponse))]
    [JsonSerializable(typeof(UserStatisticsAndQuota))]
    [JsonSerializable(typeof(UserTask))]
    [JsonSerializable(typeof(UserValidateInfo))]
    internal partial class JsonContext : JsonSerializerContext
    {
        private static Marshaller<string> CreateMarshaller<T>() where T : new()
        {
            return new Marshaller<string>(
                (Source) =>
                {
                    IMessage Message = (IMessage)JsonSerializer.Deserialize(
                        Source,
                        typeof(T),
                        Default)!;
                    return Message.ToByteArray();
                },
                (Source) =>
                {
                    IMessage Message = (IMessage)new T();
                    Message.MergeFrom(Source);
                    return JsonSerializer.Serialize(
                        Message,
                        typeof(T),
                        Default);
                });
        }

        public static Marshaller<string> BatchOperationRequestMarshaller =
            CreateMarshaller<BatchOperationRequest>();
        public static Marshaller<string> BatchOperationResponseMarshaller =
            CreateMarshaller<BatchOperationResponse>();
        public static Marshaller<string> ChangePasswordRequestMarshaller =
            CreateMarshaller<ChangePasswordRequest>();
        public static Marshaller<string> ComplaintMarshaller =
            CreateMarshaller<Complaint>();
        public static Marshaller<string> ComplaintDeleteRequestMarshaller =
            CreateMarshaller<ComplaintDeleteRequest>();
        public static Marshaller<string> ComplaintDeleteResponseMarshaller =
            CreateMarshaller<ComplaintDeleteResponse>();
        public static Marshaller<string> ComplaintListRequestMarshaller =
            CreateMarshaller<ComplaintListRequest>();
        public static Marshaller<string> ComplaintListResponseMarshaller =
            CreateMarshaller<ComplaintListResponse>();
        public static Marshaller<string> DavConfigMarshaller =
            CreateMarshaller<DavConfig>();
        public static Marshaller<string> DisconnectRequestMarshaller =
            CreateMarshaller<DisconnectRequest>();
        public static Marshaller<string> DisconnectResponseMarshaller =
            CreateMarshaller<DisconnectResponse>();
        public static Marshaller<string> DocFilePreviewMarshaller =
            CreateMarshaller<DocFilePreview>();
        public static Marshaller<string> DownloadAndPreviewInfoMarshaller =
            CreateMarshaller<DownloadAndPreviewInfo>();
        public static Marshaller<string> FastLookupMarshaller =
            CreateMarshaller<FastLookup>();
        public static Marshaller<string> FastLookupRequestMarshaller =
            CreateMarshaller<FastLookupRequest>();
        public static Marshaller<string> FileMarshaller =
            CreateMarshaller<V6.Services.Pub.File>();
        public static Marshaller<string> FileListRequestMarshaller =
            CreateMarshaller<FileListRequest>();
        public static Marshaller<string> FileListResponseMarshaller =
            CreateMarshaller<FileListResponse>();
        public static Marshaller<string> FileShareMarshaller =
            CreateMarshaller<V6.Services.Pub.FileShare>();
        public static Marshaller<string> FileShareDeleteRequestMarshaller =
            CreateMarshaller<FileShareDeleteRequest>();
        public static Marshaller<string> FileShareDeleteResponseMarshaller =
            CreateMarshaller<FileShareDeleteResponse>();
        public static Marshaller<string> FileShareListRequestMarshaller =
            CreateMarshaller<FileShareListRequest>();
        public static Marshaller<string> FileShareListResponseMarshaller =
            CreateMarshaller<FileShareListResponse>();
        public static Marshaller<string> GetIceCandidateRequestMarshaller =
            CreateMarshaller<GetIceCandidateRequest>();
        public static Marshaller<string> GetIceCandidateResponseMarshaller =
            CreateMarshaller<GetIceCandidateResponse>();
        public static Marshaller<string> LastLoginResponseMarshaller =
            CreateMarshaller<LastLoginResponse>();
        public static Marshaller<string> LoginRequestMarshaller =
            CreateMarshaller<LoginRequest>();
        public static Marshaller<string> LoginResponseMarshaller =
            CreateMarshaller<LoginResponse>();
        public static Marshaller<string> ManageRTCRequestMarshaller =
            CreateMarshaller<ManageRTCRequest>();
        public static Marshaller<string> ManageRTCResponseMarshaller =
            CreateMarshaller<ManageRTCResponse>();
        public static Marshaller<string> MetaMarshaller =
            CreateMarshaller<Meta>();
        public static Marshaller<string> OauthTokenCheckResponseMarshaller =
            CreateMarshaller<OauthTokenCheckResponse>();
        public static Marshaller<string> OauthTokenResponseMarshaller =
            CreateMarshaller<OauthTokenResponse>();
        public static Marshaller<string> OfflineTaskDeleteRequestMarshaller =
            CreateMarshaller<OfflineTaskDeleteRequest>();
        public static Marshaller<string> OfflineTaskDeleteResponseMarshaller =
            CreateMarshaller<OfflineTaskDeleteResponse>();
        public static Marshaller<string> OfflineTaskListRequestMarshaller =
            CreateMarshaller<OfflineTaskListRequest>();
        public static Marshaller<string> OfflineTaskListResponseMarshaller =
            CreateMarshaller<OfflineTaskListResponse>();
        public static Marshaller<string> OrderByInfoMarshaller =
            CreateMarshaller<OrderByInfo>();
        public static Marshaller<string> ParseFileSliceResponseMarshaller =
            CreateMarshaller<ParseFileSliceResponse>();
        public static Marshaller<string> ReceiveRequestMarshaller =
            CreateMarshaller<ReceiveRequest>();
        public static Marshaller<string> ReceiveResponseMarshaller =
            CreateMarshaller<ReceiveResponse>();
        public static Marshaller<string> RegisterRequestMarshaller =
            CreateMarshaller<RegisterRequest>();
        public static Marshaller<string> RpcHeaderValueMarshaller =
            CreateMarshaller<RpcHeaderValue>();
        public static Marshaller<string> RpcMetaDataMarshaller =
            CreateMarshaller<RpcMetaData>();
        public static Marshaller<string> RpcTransportMarshaller =
            CreateMarshaller<RpcTransport>();
        public static Marshaller<string> RtcPeerInfoMarshaller =
            CreateMarshaller<RtcPeerInfo>();
        public static Marshaller<string> ScanListRequestMarshaller =
            CreateMarshaller<ScanListRequest>();
        public static Marshaller<string> SearchRequestMarshaller =
            CreateMarshaller<SearchRequest>();
        public static Marshaller<string> SendIceCandidateRequestMarshaller =
            CreateMarshaller<SendIceCandidateRequest>();
        public static Marshaller<string> SendIceCandidateResponseMarshaller =
            CreateMarshaller<SendIceCandidateResponse>();
        public static Marshaller<string> SendPeerIceCandidateRequestMarshaller =
            CreateMarshaller<SendPeerIceCandidateRequest>();
        public static Marshaller<string> SendPeerIceCandidateResponseMarshaller =
            CreateMarshaller<SendPeerIceCandidateResponse>();
        public static Marshaller<string> SftpConfigMarshaller =
            CreateMarshaller<SftpConfig>();
        public static Marshaller<string> SliceMarshaller =
            CreateMarshaller<Slice>();
        public static Marshaller<string> SliceDownloadAddressRequestMarshaller =
            CreateMarshaller<SliceDownloadAddressRequest>();
        public static Marshaller<string> SliceDownloadAddressResponseMarshaller =
            CreateMarshaller<SliceDownloadAddressResponse>();
        public static Marshaller<string> SliceDownloadInfoMarshaller =
            CreateMarshaller<SliceDownloadInfo>();
        public static Marshaller<string> SliceSizeMarshaller =
            CreateMarshaller<SliceSize>();
        public static Marshaller<string> SmsVeifyCodeSendRequestMarshaller =
            CreateMarshaller<SmsVeifyCodeSendRequest>();
        public static Marshaller<string> SmsVeifyCodeSendRequestNotUserMarshaller =
            CreateMarshaller<SmsVeifyCodeSendRequestNotUser>();
        public static Marshaller<string> SmsVeifyCodeSendResponseMarshaller =
            CreateMarshaller<SmsVeifyCodeSendResponse>();
        public static Marshaller<string> StartDownloadRequestMarshaller =
            CreateMarshaller<StartDownloadRequest>();
        public static Marshaller<string> StartDownloadResponseMarshaller =
            CreateMarshaller<StartDownloadResponse>();
        public static Marshaller<string> StopDownloadRequestMarshaller =
            CreateMarshaller<StopDownloadRequest>();
        public static Marshaller<string> StopDownloadResponseMarshaller =
            CreateMarshaller<StopDownloadResponse>();
        public static Marshaller<string> TaskFileMarshaller =
            CreateMarshaller<TaskFile>();
        public static Marshaller<string> TaskMetaMarshaller =
            CreateMarshaller<TaskMeta>();
        public static Marshaller<string> TaskParseRequestMarshaller =
            CreateMarshaller<TaskParseRequest>();
        public static Marshaller<string> TaskParseResponseMarshaller =
            CreateMarshaller<TaskParseResponse>();
        public static Marshaller<string> TokenMarshaller =
            CreateMarshaller<Token>();
        public static Marshaller<string> UploadTokenMarshaller =
            CreateMarshaller<UploadToken>();
        public static Marshaller<string> UserMarshaller =
            CreateMarshaller<User>();
        public static Marshaller<string> UserNameValidateResponseMarshaller =
            CreateMarshaller<UserNameValidateResponse>();
        public static Marshaller<string> UserStatisticsAndQuotaMarshaller =
            CreateMarshaller<UserStatisticsAndQuota>();
        public static Marshaller<string> UserTaskMarshaller =
            CreateMarshaller<UserTask>();
        public static Marshaller<string> UserValidateInfoMarshaller =
            CreateMarshaller<UserValidateInfo>();
    }
}
