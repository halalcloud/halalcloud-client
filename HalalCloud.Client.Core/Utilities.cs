using System.Text;
using System.Security.Cryptography;

namespace HalalCloud.Client.Core
{
    public class Utilities
    {
        public static string ConvertToMD5String(string Source)
        {
            byte[] RawSource = Encoding.UTF8.GetBytes(Source);
            MD5 Provider = MD5.Create();
            byte[] RawResult = Provider.ComputeHash(RawSource);
            return BitConverter.ToString(RawResult).Replace("-", "");
        }
    }
}
