using System.Collections.Generic;

namespace BaiduBce.Services.Bos.Model;

public class Grant
{
	public List<Grantee> Grantee { get; set; }

	public List<string> Permission { get; set; }
}
