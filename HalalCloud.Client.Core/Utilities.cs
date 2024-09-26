using System.Text;
using System.Security.Cryptography;

namespace HalalCloud.Client.Core
{
    public class Utilities
    {
        public static byte[] ComputeMD5(
            string Source)
        {
            byte[] RawSource = Encoding.UTF8.GetBytes(Source);
            MD5 Provider = MD5.Create();
            return Provider.ComputeHash(RawSource);
        }

        public static string ConvertByteArrayToHexString(
            byte[] ByteArray)
        {
            return BitConverter.ToString(ByteArray).Replace("-", "");
        }

        public static string ToHashString(
            byte[] Hash)
        {
            return BitConverter.ToString(Hash).Replace("-", "");
        }

        public static void EnsurePathExists(
            string Path)
        {
            if (!Directory.Exists(Path))
            {
                Directory.CreateDirectory(Path);
            }
        }
    }
}
