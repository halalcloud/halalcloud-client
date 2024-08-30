using Google.Protobuf;
using Grpc.Core;
using System.Text.Json;
using System.Text.Json.Serialization;
using V6.Services.Pub;

namespace HalalCloud.RpcClient
{
    [JsonSourceGenerationOptions(
         DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
         PropertyNamingPolicy = JsonKnownNamingPolicy.SnakeCaseLower,
         UnmappedMemberHandling = JsonUnmappedMemberHandling.Skip)]
    [JsonSerializable(typeof(LoginRequest))]
    [JsonSerializable(typeof(OauthTokenResponse))]
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

        public static Dictionary<string, Marshaller<string>> Marshallsers =
           new Dictionary<string, Marshaller<string>>()
           {
                {
                    "LoginRequest",
                    CreateMarshaller<LoginRequest>()
                },
                {
                    "OauthTokenResponse",
                    CreateMarshaller<OauthTokenResponse>()
                }
           };
    }
}
