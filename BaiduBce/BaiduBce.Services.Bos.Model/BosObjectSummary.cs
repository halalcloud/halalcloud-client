using System;

namespace BaiduBce.Services.Bos.Model;

public class BosObjectSummary
{
	public string BucketName { get; set; }

	public string Key { get; set; }

	public string ETag { get; set; }

	public long Size { get; set; }

	public DateTime LastModified { get; set; }

	public User Owner { get; set; }

	public string StorageClass { get; set; }
}
