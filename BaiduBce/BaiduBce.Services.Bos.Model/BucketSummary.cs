using System;

namespace BaiduBce.Services.Bos.Model;

public class BucketSummary
{
	public string Name { get; set; }

	public DateTime CreationDate { get; set; }

	public string Location { get; set; }
}
