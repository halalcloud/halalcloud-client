using Grpc.Core.Interceptors;
using Grpc.Core;
using System.Text;

namespace HalalCloud.Client.Core
{
    public class SignatureInterceptor : Interceptor
    {
        private ClientInterceptorContext<TRequest, TResponse> GetNewContext<TRequest, TResponse>(
            ClientInterceptorContext<TRequest, TResponse> context)
            where TRequest : class
            where TResponse : class
        {
            var metadata = context.Options.Headers ?? new Metadata();

            string timestamp = DateTimeOffset.Now.ToUnixTimeMilliseconds().ToString();
            string appid = "devDebugger/1.0";
            string appversion = "1.0.0";
            string appsecret = "Nkx3Y2xvZ2luLmNu";

            metadata.Add("timestamp", timestamp);
            metadata.Add("appid", appid);
            metadata.Add("appversion", appversion);

            string authorization = string.Empty;
            //string authorization = "Bearer " + "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ1c2VyLWNlbnRlci12MWJldGExLjIwMjMwOSIsInN1YiI6IntcIklkZW50aXR5XCI6XCIxZmM0NDk2Mzk2NzA0NzcxOTQ1ZGEwNjMyOWMzMjllYVwiLFwiVHlwZVwiOjAsXCJTdGF0dXNcIjowLFwiVmVyc2lvblwiOjMsXCJVcGRhdGVUc1wiOjE3MDQzNzQxOTMxMDQsXCJOYW1lXCI6XCJLdXJpa28gTW91XCIsXCJBZGRvblwiOlwiXCIsXCJDcmVhdGVUc1wiOjE3MDQzNzQxOTMxMDQsXCJIYXNoXCI6XCJcIixcIkZsYWdcIjowLFwiVmFsaWRhdGVcIjowLFwiSWNvblwiOlwiXCJ9IiwiYXVkIjpbImJzczoxIl0sImV4cCI6MTcwNTM3NjAyNSwiaWF0IjoxNzA1Mjg5NTA1LCJqdGkiOiIxZmM0NDk2Mzk2NzA0NzcxOTQ1ZGEwNjMyOWMzMjllYSJ9.yuYus7TdaeBxWZbDpEmXhC_KODqSKdsoCZjfur299n8";
            //metadata.Add("authorization", authorization);

            {
                string sign = context.Method.FullName + timestamp + appid + appversion + authorization + appsecret;

                using var provider = System.Security.Cryptography.MD5.Create();
                StringBuilder builder = new StringBuilder();
                foreach (byte b in provider.ComputeHash(Encoding.UTF8.GetBytes(sign)))
                    builder.Append(b.ToString("x2").ToLower());
                metadata.Add("sign", builder.ToString());
            }

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
