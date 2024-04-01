using Grpc.Core.Interceptors;
using Grpc.Core;

namespace HalalCloud.Client.Core
{
    public class SignatureInterceptor : Interceptor
    {
        public string AccessToken { get; set; } = string.Empty;

        private ClientInterceptorContext<TRequest, TResponse> GetNewContext<TRequest, TResponse>(
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

        public override TResponse BlockingUnaryCall<TRequest, TResponse>(
            TRequest request,
            ClientInterceptorContext<TRequest, TResponse> context,
            BlockingUnaryCallContinuation<TRequest, TResponse> continuation)
        {
            return continuation(request, GetNewContext(context));
        }

        public override AsyncUnaryCall<TResponse> AsyncUnaryCall<TRequest, TResponse>(
            TRequest request,
            ClientInterceptorContext<TRequest, TResponse> context,
            AsyncUnaryCallContinuation<TRequest, TResponse> continuation)
        {
            return continuation(request, GetNewContext(context));
        }

        public override AsyncServerStreamingCall<TResponse> AsyncServerStreamingCall<TRequest, TResponse>(
            TRequest request,
            ClientInterceptorContext<TRequest, TResponse> context,
            AsyncServerStreamingCallContinuation<TRequest, TResponse> continuation)
        {
            return continuation(request, GetNewContext(context));
        }

        public override AsyncClientStreamingCall<TRequest, TResponse> AsyncClientStreamingCall<TRequest, TResponse>(
            ClientInterceptorContext<TRequest, TResponse> context,
            AsyncClientStreamingCallContinuation<TRequest, TResponse> continuation)
        {
            return continuation(GetNewContext(context));
        }

        public override AsyncDuplexStreamingCall<TRequest, TResponse> AsyncDuplexStreamingCall<TRequest, TResponse>(
            ClientInterceptorContext<TRequest, TResponse> context,
            AsyncDuplexStreamingCallContinuation<TRequest, TResponse> continuation)
        {
            return continuation(GetNewContext(context));
        }
    }
}
