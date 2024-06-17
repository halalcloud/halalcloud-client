using System;
using System.Collections.Generic;

namespace BaiduBce.Services.Bos.Model;

public class ObjectMetadata
{
	public IDictionary<string, string> UserMetadata { get; set; }

	public string BceContentSha256 { get; set; }

	public string ContentDisposition { get; set; }

	public string ContentEncoding { get; set; }

	public long ContentLength { get; set; }

	public string ContentMd5 { get; set; }

	public string ContentType { get; set; }

	public string ETag { get; set; }

	public long InstanceLength { get; set; }

	public DateTime LastModified { get; set; }

	public string ContentRange { get; set; }

	public string StorageClass { get; set; }

	public ObjectMetadata()
	{
		UserMetadata = new Dictionary<string, string>();
	}
}
