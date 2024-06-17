namespace BaiduBce.Services.Bos;

public static class BosConstants
{
	public static class Permission
	{
		public const string FullControl = "FULL_CONTROL";

		public const string Read = "READ";

		public const string Write = "WRITE";
	}

	public static class CannedAcl
	{
		public const string Private = "private";

		public const string PublicRead = "public-read";

		public const string PublicReadWrite = "public-read-write";
	}

	public static class StorageClass
	{
		public const string Standard = "STANDARD";

		public const string StandardInfrequentAccess = "STANDARD_IA";
	}
}
