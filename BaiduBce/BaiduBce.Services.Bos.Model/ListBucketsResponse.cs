using System.Collections.Generic;

namespace BaiduBce.Services.Bos.Model;

public class ListBucketsResponse : BosResponseBase
{
	public User Owner { get; set; }

	public List<BucketSummary> Buckets { get; set; }
}
