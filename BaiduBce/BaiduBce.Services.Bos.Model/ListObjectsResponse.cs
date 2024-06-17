using System.Collections.Generic;

namespace BaiduBce.Services.Bos.Model;

public class ListObjectsResponse : BosResponseBase
{
	public string BucketName { get; set; }

	public string NextMarker { get; set; }

	public bool IsTruncated { get; set; }

	public string Prefix { get; set; }

	public string Marker { get; set; }

	public int MaxKeys { get; set; }

	public string Delimiter { get; set; }

	public List<BosObjectSummary> Contents { get; set; }

	public List<ObjectPrefix> CommonPrefixes { get; set; }
}
