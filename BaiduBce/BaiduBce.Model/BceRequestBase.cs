using BaiduBce.Auth;

namespace BaiduBce.Model;

public class BceRequestBase
{
	public IBceCredentials Credentials { get; set; }

	public BceClientConfiguration Config { get; set; }
}
