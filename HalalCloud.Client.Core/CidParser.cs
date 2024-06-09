namespace HalalCloud.Client.Core
{
    public class CidParser
    {
        /// <summary>
        /// Parse a multibase string.
        /// </summary>
        /// <param name="InputString">The input multibase string.</param>
        /// <returns>The parse result.</returns>
        public static (MultibaseEncoding Type, string Content) ParseMultibase(
            string InputString)
        {
            if (string.IsNullOrEmpty(InputString))
            {
                return (MultibaseEncoding.None, string.Empty);
            }

            MultibaseEncoding Encoding = InputString[0] switch
            {
                '\x0000' => MultibaseEncoding.None,
                '\x0001' => MultibaseEncoding.None,
                'f' => MultibaseEncoding.Base16,
                'F' => MultibaseEncoding.Base16Upper,
                'b' => MultibaseEncoding.Base32,
                'B' => MultibaseEncoding.Base32Upper,
                'z' => MultibaseEncoding.Base58Btc,
                'm' => MultibaseEncoding.Base64,
                'u' => MultibaseEncoding.Base64Url,
                'U' => MultibaseEncoding.Base64UrlPad,
                'Q' => MultibaseEncoding.None,
                '/' => MultibaseEncoding.None,
                _ => MultibaseEncoding.Unsupported,
            };

            return (Encoding, InputString.Substring(1));
        }

        /// <summary>
        /// Parse an integer from a byte array with unsigned varint (VARiable
        /// INTeger) format.
        /// </summary>
        /// <param name="InputBytes">
        /// The byte array with unsigned varint (VARiable INTeger) format.
        /// </param>
        /// <returns>The parsed integer.</returns>
        /// <see cref="https://github.com/multiformats/unsigned-varint"/>
        /// <remarks>
        /// For security, to avoid memory attacks, we use a "practical max" of
        /// 9 bytes. Though there is no theoretical limit, and future specs can
        /// grow this number if it is truly necessary to have code or length
        /// values equal to or larger than 2^63.
        /// </remarks>
        public static (long Integer, int Parsed) ParseUnsignedVarint(
            byte[] InputBytes)
        {
            const int MaximumBytes = 9;

            (long Integer, int Parsed) Result = (0, 0);
            for (int i = 0; i < InputBytes.Length && i < MaximumBytes; ++i)
            {
                Result.Integer |=
                    (long)(InputBytes[i] & 0x7F) << (Result.Parsed * 7);
                ++Result.Parsed;
                if ((InputBytes[i] & 0x80) == 0)
                {
                    break;
                }
            }

            return Result;
        }
    }
}
