namespace HalalCloud.Client.Core
{
    /// <summary>
    /// The type of multibase encoding.
    /// </summary>
    public enum MultibaseEncoding
    {
        /// <summary>
        /// No base encoding
        /// </summary>
        None,

        /// <summary>
        /// Unsupported base encoding
        /// </summary>
        Unsupported,

        /// <summary>
        /// Hexadecimal (lowercase)
        /// </summary>
        Base16,

        /// <summary>
        /// Hexadecimal (uppercase)
        /// </summary>
        Base16Upper,

        /// <summary>
        /// RFC4648 case-insensitive - no padding (lowercase)
        /// </summary>
        Base32,

        /// <summary>
        /// RFC4648 case-insensitive - no padding (uppercase)
        /// </summary>
        Base32Upper,

        /// <summary>
        /// Base58 Bitcoin
        /// </summary>
        Base58Btc,

        /// <summary>
        /// RFC4648 no padding
        /// </summary>
        Base64,

        /// <summary>
        /// RFC4648 no padding
        /// </summary>
        Base64Url,

        /// <summary>
        /// RFC4648 with padding
        /// </summary>
        Base64UrlPad
    }
}
