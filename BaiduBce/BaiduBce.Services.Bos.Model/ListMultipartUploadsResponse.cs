using System.Collections.Generic;

namespace BaiduBce.Services.Bos.Model;

public class ListMultipartUploadsResponse : BosResponseBase
{
	public string BucketName { get; set; }

	public string KeyMarker { get; set; }

	public string Prefix { get; set; }

	public string Delimiter { get; set; }

	public int MaxUploads { get; set; }

	public bool IsTruncated { get; set; }

	public string NextKeyMarker { get; set; }

	public List<MultipartUploadSummary> Uploads { get; set; }

	public List<ObjectPrefix> CommonPrefixes { get; set; }
}
