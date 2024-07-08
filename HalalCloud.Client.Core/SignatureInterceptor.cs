using Grpc.Core.Interceptors;
using Grpc.Core;

namespace HalalCloud.Client.Core
{
    public class SignatureInterceptor : ClientInterceptor
    {
        public string AccessToken { get; set; } = string.Empty;

        public override ClientInterceptorContext<TRequest, TResponse> ContextHandler<TRequest, TResponse>(
            ClientInterceptorContext<TRequest, TResponse> context)
            where TRequest : class
            where TResponse : class
        {
            var metadata = context.Options.Headers ?? new Metadata();

            string ApplicationId = "devDebugger/1.0";
            string ApplicationVersion = "1.0.0";
            string ApplicationSecret = "Nkx3Y2xvZ2luLmNu";
            long TimeStamp = DateTimeOffset.Now.ToUnixTimeMilliseconds();

            metadata.Add("timestamp", TimeStamp.ToString());
            metadata.Add("appid", ApplicationId);
            metadata.Add("appversion", ApplicationVersion);

            string Authorization = Utilities.GenerateAuthorization(AccessToken);
            if (!string.IsNullOrWhiteSpace(Authorization))
            {
                metadata.Add("authorization", Authorization);
            }

            metadata.Add(
                "sign",
                Utilities.ComputeSignature(
                    ApplicationId,
                    ApplicationVersion,
                    ApplicationSecret,
                    Authorization,
                    context.Method.FullName,
                    TimeStamp).ToLower());

            var deadline = DateTime.UtcNow.AddSeconds(5);

            return new ClientInterceptorContext<TRequest, TResponse>(
                context.Method,
                context.Host,
                context.Options.WithHeaders(metadata).WithDeadline(deadline));
        }
    }
}
