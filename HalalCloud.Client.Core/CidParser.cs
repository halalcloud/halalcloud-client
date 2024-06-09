namespace HalalCloud.Client.Core
{
    public class CidParser
    {
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
