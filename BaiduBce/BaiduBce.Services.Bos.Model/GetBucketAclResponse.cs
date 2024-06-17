using System.Collections.Generic;

namespace BaiduBce.Services.Bos.Model;

public class GetBucketAclResponse : BosResponseBase
{
	public const int MaxSupportedAclVersion = 1;

	public int Version { get; set; }

	public Grantee Owner { get; set; }

	public List<Grant> AccessControlList { get; set; }
}
