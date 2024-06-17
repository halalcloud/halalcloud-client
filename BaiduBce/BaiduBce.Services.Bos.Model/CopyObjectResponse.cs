using System;

namespace BaiduBce.Services.Bos.Model;

public class CopyObjectResponse : BosResponseBase
{
	public string ETag { get; set; }

	public DateTime LastModified { get; set; }
}
