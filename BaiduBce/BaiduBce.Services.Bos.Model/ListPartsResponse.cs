using System;
using System.Collections.Generic;

namespace BaiduBce.Services.Bos.Model;

public class ListPartsResponse : BosResponseBase
{
	public string BucketName { get; set; }

	public DateTime Initiated { get; set; }

	public bool IsTruncated { get; set; }

	public string Key { get; set; }

	public int? MaxParts { get; set; }

	public int NextPartNumberMarker { get; set; }

	public User Owner { get; set; }

	public int PartNumberMarker { get; set; }

	public List<PartSummary> Parts { get; set; }

	public string UploadId { get; set; }

	public string StorageClass { get; set; }
}
