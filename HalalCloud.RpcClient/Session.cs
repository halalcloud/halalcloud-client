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
                    "v6.services.pub.PubUser",
                    "CreateAuthToken",
                    JsonContext.Marshallsers["LoginRequest"],
                    JsonContext.Marshallsers["OauthTokenResponse"])
            };

        public Token? TokenObject { get; set; } = null;

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
            if (TokenObject != null &&
                !string.IsNullOrWhiteSpace(TokenObject.AccessToken))
            {
                Authorization = "Bearer " + TokenObject.AccessToken;
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
