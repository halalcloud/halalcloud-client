using System.Runtime.InteropServices;

namespace HalalCloud.RpcClient
{
    public static class IntPtrExtensions
    {
        public static GCHandle ToGcHandle(
            this object Value)
        {
            return GCHandle.Alloc(Value);
        }

        public static GCHandle ToGcHandle(
            this IntPtr Value)
        {
            return GCHandle.FromIntPtr(Value);
        }

        public static IntPtr ToIntPtr(
            this object Value)
        {
            return GCHandle.ToIntPtr(Value.ToGcHandle());
        }

        public static T ToObject<T>(
            this IntPtr Value)
        {
            T? Object = (T?)Value.ToGcHandle().Target;
            if (Object == null)
            {
                throw new InvalidOperationException();
            }
            return Object;
        }

        public static T ToDelegate<T>(
            this IntPtr Value)
        {
            return Marshal.GetDelegateForFunctionPointer<T>(Value);
        }

        public static string ToUtf8String(
            this IntPtr Value)
        {
            return Marshal.PtrToStringUTF8(Value) ?? string.Empty;
        }
    }
}
