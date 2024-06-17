using System.Collections.Generic;

namespace BaiduBce.Services.Bos.Model;

public class AccessControlListRequest
{
	public List<Grant> AccessControlList { get; set; }
}
