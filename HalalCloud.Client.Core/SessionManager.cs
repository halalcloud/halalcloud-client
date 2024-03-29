﻿using Grpc.Core;
using Grpc.Core.Interceptors;
using Grpc.Net.Client;
using System.Threading.Channels;
using V6.Services.Pub;

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
    }
}
