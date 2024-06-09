namespace HalalCloud.Client.Core
{
    /// <summary>
    /// Information about a CID.
    /// </summary>
    public struct CidInformation
    {
        /// <summary>
        /// The multibase encoding of the CID.
        /// </summary>
        public MultibaseEncoding Encoding;

        /// <summary>
        /// The version of the CID.
        /// </summary>
        public long Version;

        /// <summary>
        /// The content type of the CID.
        /// </summary>
        public long ContentType;

        /// <summary>
        /// The hash type of the CID.
        /// </summary>
        public long HashType;

        /// <summary>
        /// The hash value of the CID.
        /// </summary>
        public byte[] HashValue;
    }
}
