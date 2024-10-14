using Grpc.Core;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace HalalCloud.RpcClient
{
    public class NativeSession
    {
        [UnmanagedCallersOnly(
           CallConvs = [typeof(CallConvCdecl)],
           EntryPoint = "HccRpcCreateSession")]
        public static unsafe int CreateSession(
           IntPtr* Instance)
        {
            StatusCode Result = StatusCode.OK;

            try
            {
                if (Instance == null)
                {
                    throw new ArgumentNullException();
                }

                *Instance = new Session().ToIntPtr();
            }
            catch
            {
                Result = StatusCode.Internal;
            }

            return Convert.ToInt32(Result);
        }

        [UnmanagedCallersOnly(
            CallConvs = [typeof(CallConvCdecl)],
            EntryPoint = "HccRpcCloseSession")]
        public static unsafe void CloseSession(
            IntPtr Instance)
        {
            try
            {
                Instance.ToGcHandle().Free();
            }
            catch
            {

            }
        }

        [UnmanagedCallersOnly(
            CallConvs = [typeof(CallConvCdecl)],
            EntryPoint = "HccRpcSetAccessToken")]
        public static unsafe int SetAccessToken(
            IntPtr Instance,
            IntPtr AccessToken)
        {
            StatusCode Result = StatusCode.OK;

            try
            {
                Instance.ToObject<Session>().AccessToken =
                    AccessToken.ToUtf8String();
            }
            catch
            {
                Result = StatusCode.Internal;
            }

            return Convert.ToInt32(Result);
        }

        [UnmanagedCallersOnly(
            CallConvs = [typeof(CallConvCdecl)],
            EntryPoint = "HccRpcRequest")]
        public static unsafe int Request(
            IntPtr Instance,
            IntPtr MethodFullName,
            IntPtr RequestJson,
            IntPtr* ResponseJson)
        {
            StatusCode Result = StatusCode.OK;

            try
            {
                string Response = Instance.ToObject<Session>().Request(
                    MethodFullName.ToUtf8String(),
                    RequestJson.ToUtf8String());

                if (ResponseJson != null)
                {
                    *ResponseJson = Marshal.StringToCoTaskMemUTF8(Response);
                }
            }
            catch (RpcException e)
            {
                Result = e.StatusCode;
            }
            catch
            {
                Result = StatusCode.Internal;
            }

            return Convert.ToInt32(Result);
        }

        [UnmanagedCallersOnly(
            CallConvs = [typeof(CallConvCdecl)],
            EntryPoint = "HccRpcFreeMemory")]
        public static unsafe void FreeMemory(
            IntPtr Block)
        {
            try
            {
                Marshal.FreeCoTaskMem(Block);
            }
            catch
            {

            }
        }
    }
}
