using BaiduBce.Model;
using BaiduBce.Services.Bos.Model;
using BaiduBce.Services.Sms.Model;
using BaiduBce.Services.Sts.Model;
using System.Text.Json.Serialization;

namespace BaiduBce.Util
{
    [JsonSourceGenerationOptions(
         DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
         PropertyNamingPolicy = JsonKnownNamingPolicy.CamelCase,
         UnmappedMemberHandling = JsonUnmappedMemberHandling.Skip)]
    [JsonSerializable(typeof(BceServiceException.BceErrorResponse))]
    [JsonSerializable(typeof(CopyObjectResponse))]
    [JsonSerializable(typeof(CreateBucketResponse))]
    [JsonSerializable(typeof(ListBucketsResponse))]
    [JsonSerializable(typeof(DeleteBucketResponse))]
    [JsonSerializable(typeof(GetBucketAclResponse))]
    [JsonSerializable(typeof(BosResponseBase))]
    [JsonSerializable(typeof(GetBucketLocationResponse))]
    [JsonSerializable(typeof(ListObjectsResponse))]
    [JsonSerializable(typeof(PutObjectResponse))]
    [JsonSerializable(typeof(InitiateMultipartUploadResponse))]
    [JsonSerializable(typeof(ListPartsResponse))]
    [JsonSerializable(typeof(CompleteMultipartUploadResponse))]
    [JsonSerializable(typeof(ListMultipartUploadsResponse))]
    [JsonSerializable(typeof(SendMessageResponse))]
    [JsonSerializable(typeof(TemplateCreateResponse))]
    [JsonSerializable(typeof(BceResponseBase))]
    [JsonSerializable(typeof(TemplateResponse))]
    [JsonSerializable(typeof(SignatureCreateResponse))]
    [JsonSerializable(typeof(SignatureResponse))]
    [JsonSerializable(typeof(QuotaQueryResponse))]
    [JsonSerializable(typeof(BlackListResponse))]
    [JsonSerializable(typeof(StatisticsTemplateListResponse))]
    [JsonSerializable(typeof(ListStatisticsResponse))]
    [JsonSerializable(typeof(GetSessionTokenResponse))]
    [JsonSerializable(typeof(AccessControlListRequest))]
    [JsonSerializable(typeof(PartETags))]
    [JsonSerializable(typeof(SendMessageRequest))]
    [JsonSerializable(typeof(TemplateCreateRequest))]
    [JsonSerializable(typeof(TemplateUpdateRequest))]
    [JsonSerializable(typeof(SignatureCreateRequest))]
    [JsonSerializable(typeof(SignatureUpdateRequest))]
    [JsonSerializable(typeof(QuotaUpdateRequest))]
    [JsonSerializable(typeof(BlackCreateRequest))]
    internal partial class JsonContext : JsonSerializerContext
    {
    }
}
