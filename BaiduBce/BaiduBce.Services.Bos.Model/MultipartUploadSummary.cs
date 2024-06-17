using System;

namespace BaiduBce.Services.Bos.Model;

public class MultipartUploadSummary
{
	public string Key { get; set; }

	public string UploadId { get; set; }

	public User Owner { get; set; }

	public DateTime Initiated { get; set; }

	public string StorageClass { get; set; }
}
