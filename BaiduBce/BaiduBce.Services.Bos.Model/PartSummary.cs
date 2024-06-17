using System;

namespace BaiduBce.Services.Bos.Model;

public class PartSummary
{
	public int PartNumber { get; set; }

	public DateTime LastModified { get; set; }

	public string ETag { get; set; }

	public long Size { get; set; }
}
