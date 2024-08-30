using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text.Json;

namespace HalalCloud.RpcClient
{
    public class NativeSession
    {
        [UnmanagedCallersOnly(
           CallConvs = [typeof(CallConvStdcall)],
           EntryPoint = "HccRpcCreateSession")]
        public static unsafe int CreateSession(
           IntPtr* Instance)
        {
            try
            {
                if (Instance == null)
                {
                    throw new ArgumentNullException();
                }

                *Instance = new Session().ToIntPtr();

                return 0;
            }
            catch (Exception e)
            {
                return e.HResult;
            }
        }

        [UnmanagedCallersOnly(
            CallConvs = [typeof(CallConvStdcall)],
            EntryPoint = "HccRpcCloseSession")]
        public static unsafe int CloseSession(
            IntPtr Instance)
        {
            try
            {
                Instance.ToGcHandle().Free();

                return 0;
            }
            catch (Exception e)
            {
                return e.HResult;
            }
        }

        [UnmanagedCallersOnly(
            CallConvs = [typeof(CallConvStdcall)],
            EntryPoint = "HccRpcSetToken")]
        public static unsafe int SetToken(
            IntPtr Instance,
            IntPtr AccessTokenJson)
        {
            try
            {

                Instance.ToObject<Session>().TokenObject =
                    JsonSerializer.Deserialize(
                        AccessTokenJson.ToUtf8String(),
                        JsonContext.Default.Token)!;

                return 0;
            }
            catch (Exception e)
            {
                return e.HResult;
            }
        }

        [UnmanagedCallersOnly(
            CallConvs = [typeof(CallConvStdcall)],
            EntryPoint = "HccRpcRequest")]
        public static unsafe int Request(
            IntPtr Instance,
            IntPtr MethodFullName,
            IntPtr RequestJson,
            IntPtr* ResponseJson)
        {
            try
            {
                string Response = Instance.ToObject<Session>().Request(
                    MethodFullName.ToUtf8String(),
                    RequestJson.ToUtf8String());

                if (ResponseJson != null)
                {
                    *ResponseJson = Marshal.StringToCoTaskMemUTF8(Response);
                }

                return 0;
            }
            catch (Exception e)
            {
                return e.HResult;
            }
        }
    }
}
