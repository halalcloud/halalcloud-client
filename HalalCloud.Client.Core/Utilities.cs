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

        public static string ComputeSignature(
            string ApplicationId,
            string ApplicationVersion,
            string ApplicationSecret,
            string Authorization,
            string MethodName,
            long TimeStamp)
        {
            StringBuilder Source = new StringBuilder();
            Source.Append(MethodName);
            Source.Append(TimeStamp.ToString());
            Source.Append(ApplicationId);
            Source.Append(ApplicationVersion);
            if (!string.IsNullOrWhiteSpace(Authorization))
            {
                Source.Append(Authorization);
            }
            Source.Append(ApplicationSecret);
            return ConvertByteArrayToHexString(ComputeMD5(Source.ToString()));
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
