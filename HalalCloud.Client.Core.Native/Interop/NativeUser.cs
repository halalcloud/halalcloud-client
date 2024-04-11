using System.Runtime.InteropServices;
using V6.Services.Pub;

namespace HalalCloud.Client.Core.Interop
{
    [StructLayout(LayoutKind.Sequential)]
    public struct NativeUser
    {
        /// <summary>
        /// identity for user, unique in system
        /// </summary>
        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Identity = string.Empty;

        /// <summary>
        /// 1: user, 2: admin, 3: super admin
        /// </summary>
        public int Type;

        /// <summary>
        /// 1: normal, 2: disabled, 3: deleted
        /// </summary>
        public int Status;

        public long UpdateTs;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Password = string.Empty;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Name = string.Empty;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Addon = string.Empty;

        public long CreateTs;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Hash = string.Empty;

        [MarshalAs(UnmanagedType.LPUTF8Str)]
        public string Icon = string.Empty;

        public NativeUser(User? Source)
        {
            if (Source != null)
            {
                Identity = Source.Identity;
                Type = Source.Type;
                Status = Source.Status;
                UpdateTs = Source.UpdateTs;
                Password = Source.Password;
                Name = Source.Name;
                Addon = Source.Addon;
                CreateTs = Source.CreateTs;
                Hash = Source.Hash;
                Icon = Source.Icon;
            }
        }
    }
}
