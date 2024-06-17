using System.Collections.Generic;

namespace BaiduBce.Services.Bos.Model;

public class SetBucketAclRequest : BucketRequestBase
{
	public string CannedAcl { get; set; }

	public List<Grant> AccessControlList { get; set; }
}
