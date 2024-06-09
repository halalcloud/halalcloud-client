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

        /// <summary>
        /// Unit test for <see cref="CidParser.ParseCid(string)"/>.
        /// </summary>
        [TestMethod]
        public void TestParseCid()
        {
            Assert.AreEqual(
                "C3C4733EC8AFFD06CF9E9FF50FFC6BCD2EC85A6170004BB709669C31DE94391A",
                Utilities.ToHashString(CidParser.ParseCid(
                    "bafybeigdyrzt5sfp7udm7hu76uh7y26nf3efuylqabf3oclgtqy55fbzdi").HashValue));

            Assert.AreEqual(
                "E5D8A2FF507EF9A8F88FE8634137CEFDE185D55D45E21B9201D5BC7DF09DDA70",
                Utilities.ToHashString(CidParser.ParseCid(
                    "bafybeihf3crp6ud67guprd7imnatptx54gc5kxkf4inzeaovxr67bho2oa").HashValue));

            Assert.AreEqual(
                "61A92E0904D6DB8850E208090BB243623C7EBADA4A47F5F894335AB35D31F651",
                Utilities.ToHashString(CidParser.ParseCid(
                    "bafkreidbvexasbgw3oefbyqibef3eq3chr7lvwski727rfbtlkzv2mpwke").HashValue));
        }
    }
}
