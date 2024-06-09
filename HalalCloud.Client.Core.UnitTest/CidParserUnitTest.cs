namespace HalalCloud.Client.Core.UnitTest
{
    [TestClass]
    public class CidParserUnitTest
    {
        /// <summary>
        /// Unit test for <see cref="CidParser.ParseUnsignedVarint(byte[])"/>.
        /// </summary>
        [TestMethod]
        public void TestParseUnsignedVarint()
        {
            // Reference: https://github.com/multiformats/unsigned-varint#spec

            Assert.AreEqual(
                (1, 1),
                CidParser.ParseUnsignedVarint(new byte[] { 0x01 }));
            Assert.AreEqual(
                (127, 1),
                CidParser.ParseUnsignedVarint(new byte[] { 0x7F }));
            Assert.AreEqual(
                (128, 2),
                CidParser.ParseUnsignedVarint(new byte[] { 0x80, 0x01 }));
            Assert.AreEqual(
                (255, 2),
                CidParser.ParseUnsignedVarint(new byte[] { 0xFF, 0x01 }));
            Assert.AreEqual(
                (300, 2),
                CidParser.ParseUnsignedVarint(new byte[] { 0xAC, 0x02 }));
            Assert.AreEqual(
                (16384, 3),
                CidParser.ParseUnsignedVarint(new byte[] { 0x80, 0x80, 0x01 }));
            Assert.AreEqual(
                (16384, 3),
                CidParser.ParseUnsignedVarint(new byte[] { 0x80, 0x80, 0x01, 0x00 }));
        }
    }
}
